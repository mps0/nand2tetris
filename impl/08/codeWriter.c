/* codeWpiter.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codeWriter.h"
#include "parser.h"

int  STACK_IDX = 0;
STACK_ELE STACK[10000];
char FILENAME[MAX_STR_LENGTH];
char CURR_FUNC_NAME[MAX_STR_LENGTH];

void init()
{
    /* @256
     * D=A
     * @SP
     * M=D
     */
    writeAt(BASE_STACK_ADDRESS_STRING);
    writeAssignment("D", "A");
    writeAt("SP");
    writeAssignment("M", "D");
}

void finish()
{
    fputs("(INFLOOP)\n", fWRITE);
    writeAt("INFLOOP");
    fputs("0;JMP", fWRITE);
}

void setFileName(const char* name)
{
    strcpy(FILENAME, name);
}

void writePushPop(const Command command)
{
    switch(command.type)
    {
        case C_PUSH:
            writePush(command);
            break;

        case C_POP:
            writePop(command);
            break;
    }
}

void writePush(const Command command)
{
    if(strcmp(command.arg1, "constant") == 0)
    {
        char buff[100];
        sprintf(buff, "%d", command.arg2);
        //itoa(command.arg2, buff, 10);
        writeAt(buff);
        writeAssignment("D", "A");
        writeAt("SP");
        writeAssignment("M", "M+1");
        writeAssignment("A", "M-1");
        writeAssignment("M", "D");
    }
    else
    {
            if(command.arg2 == 0)
            {
                // go to segment
                writeAt(command.arg1);
                if(command.followPointer == 1)
                {
                    writeAssignment("A", "M");
                }
                // copy value
                writeAssignment("D", "M");
            }
            else
            {
                char buff[100];
                //itoa(command.arg2, buff, 10);
                sprintf(buff, "%d", command.arg2);
                writeAt(buff);
                writeAssignment("D","A");
                writeAt(command.arg1);
                if(command.followPointer == 1)
                {
                    writeAssignment("D", "D+M");
                }
                else
                {
                    writeAssignment("D", "D+A");
                }
                writeAssignment("A", "D");
                writeAssignment("D", "M");
            }

            //push onto stack
            writeAt("SP");
            writeAssignment("M", "M+1");
            writeAssignment("A", "M-1");
            writeAssignment("M", "D");
    }
}

void writePop(const Command command)
{
    writeAt("SP");
    // decrement SP
    writeAssignment("M", "M-1");
    writeAssignment("A", "M");
    //now at SP -1 (last number) address...
    //save the value into D
    writeAssignment("D", "M");

    if(command.followPointer == 1)
    {
        int base = 0;
        if(strcmp(command.arg1, "THIS") == 0)
            base = THIS;
        else if(strcmp(command.arg1, "THAT") == 0)
            base = THAT;
        else if(strcmp(command.arg1, "LCL") == 0)
            base = LCL;
        else if(strcmp(command.arg1, "ARG") == 0)
            base = ARG;

        writeAt(command.arg1);
        if(command.arg2 == 0)
        {
            writeAssignment("A", "M");
        }
        else
        {
            //Save the value into R13
            writeAt("R13");
            writeAssignment("M","D");
            char buff[100];
            //itoa(command.arg2, buff, 10);
            sprintf(buff, "%d", command.arg2);
            // Save address into R14
            writeAt(buff);
            writeAssignment("D","A");
            writeAt(command.arg1);
            writeAssignment("D", "D+M");
            writeAt("R14");
            writeAssignment("M","D");

            // Save value into segment + index
            writeAt("R13");
            writeAssignment("D", "M");
            writeAt("R14");
            writeAssignment("A","M");
        }
    }
    else
    {
        int base = 0;
        if(strcmp(command.arg1, "temp") == 0)
            base = TEMP;
        else if(strcmp(command.arg1, "R13") == 0)
            base = 13;
        else if(strcmp(command.arg1, "R14") == 0)
            base = 14;
        else if(strcmp(command.arg1, "R15") == 0)
            base = 15;

        if(command.arg2 == 0)
        {
            writeAt(command.arg1);
        }
        else
        {
            int loc = base + command.arg2;
            char buff[MAX_STR_LENGTH];
            sprintf(buff, "%d", loc);
            writeAt(buff);
        }
    }
    writeAssignment("M","D");
}

ArithType getArithType(const Command command)
{
    if(strcmp(command.arg1, "add") == 0)
        return A_ADD;

    else if(strcmp(command.arg1, "sub") == 0)
        return A_SUB;

    else if(strcmp(command.arg1, "neg") == 0)
        return A_NEG;

    else if(strcmp(command.arg1, "eq") == 0)
        return A_EQ;

    else if(strcmp(command.arg1, "gt") == 0)
        return A_GT;

    else if(strcmp(command.arg1, "lt") == 0)
        return A_LT;

    else if(strcmp(command.arg1, "and") == 0)
        return A_AND;

    else if(strcmp(command.arg1, "or") == 0)
        return A_OR;

    else if(strcmp(command.arg1, "not") == 0)
        return A_NOT;

    else
        return A_UNDEFINED;
}

void writeArithmetic(const Command command)
{
    ArithType arithType = getArithType(command);

    switch(arithType)
    {
        case A_ADD:
            writeTwoInputOp("+");
            break;

        case A_SUB:
            writeTwoInputOp("-");
            break;

        case A_NEG:
            writeOneInputOp("-");
            break;

        case A_EQ:
            writeLogical("JEQ");
            break;

        case A_GT:
            writeLogical("JLT");
            break;

        case A_LT:
            writeLogical("JGT");
            break;

        case A_AND:
            writeTwoInputOp("&");
            break;

        case A_OR:
            writeTwoInputOp("|");
            break;

        case A_NOT:
            writeOneInputOp("!");
            break;
    }
}

void writeOneInputOp(const char* op)
{
    writeAt("SP");
    writeAssignment("A", "M-1");

    char buf[6];
    sprintf(buf, "%sM\n", op);
    writeAssignment("M", buf);
}

void writeTwoInputOp(const char* op)
{
    // save y into D
    writeAt("SP");

    // decrement the SP here since the result only occupies 1 from 2
    writeAssignment("M", "M-1");
    writeAssignment("A", "M");
    writeAssignment("D", "M");

    // add D (y) to x
    writeAssignment("A", "A-1");
    //TODO
    //writeAssignment("M", "M+D");
    //fputs("M=M", fWRITE);
    //fputs(op, fWRITE);
    //fputs("D\n", fWRITE);

    char buf[8];
    sprintf(buf, "M%sD\n", op);
    writeAssignment("M", buf);
}


void writeLogical(const char* jump)
{
    static int inc = 0;

    char buff[32];
    createIncLabel("TRUE", buff, inc);
    char buff2[32];
    createIncLabel("TRUERETURN", buff2, inc);

    // save y into D
    writeAt("SP");

    // decrement the SP here since the result only occupies 1 from 2
    writeAssignment("M", "M-1");
    writeAssignment("A", "M");
    writeAssignment("D", "M");

    // sub x from D (y)
    // if this is 0... they're equal
    writeAssignment("A", "A-1");
    // D = y - x
    writeAssignment("D", "D-M");

    writeJump(buff, "D", jump);

    //DIDNTJUMP: set to 0 (FALSE)
    writeAssignment("D", "0");
    writeJump(buff2, "0", "JMP");

    // set to -1 (TRUE)
    writeLabel(buff);
    writeAssignment("D", "-1");

    //RETURN
    writeLabel(buff2);
    writeAt("SP");
    writeAssignment("A", "M-1");
    writeAssignment("M", "D");

    inc++;
}

void writeJump(const char* label, const char* cond, const char* jump)
{
    fputs("@", fWRITE);
    fputs(label, fWRITE);
    fputs("\n", fWRITE);

    fputs(cond, fWRITE);
    fputs(";", fWRITE);
    fputs(jump, fWRITE);
    fputs("\n", fWRITE);
}

void writeLabel(const char* label)
{
    printf("WRITING LABEL: %s\n", label);
    printf("LENGHT LABEL: %d\n", strlen(label));

    fputs("(", fWRITE);
    fputs(label, fWRITE);
    fputs(")\n", fWRITE);
}

void writeAt(const char* addr)
{
    fputs("@", fWRITE);
    fputs(addr, fWRITE);
    fputs("\n", fWRITE);
}

void writeAssignment(const char* lh, const char* rh)
{
    fputs(lh, fWRITE);
    fputs("=", fWRITE);
    fputs(rh, fWRITE);
    fputs("\n", fWRITE);
}

void writeGoto(const char* label)
{
    writeJump(label, "0", "JMP");
}

// true is anything not 0
// false is 0
void writeIf(const char* label)
{
    ////pop value from stack
    //Command command0 = {C_POP, "R13", 0, 0};
    //writePop(command0);
    //writeAssignment("R13");
    //writeJump(label, "M", "JNE");


    writeAt("SP");
    // decrement SP
    writeAssignment("M", "M-1");
    writeAssignment("A", "M");
    //now at SP -1 (last number) address...
    //save the value into D
    writeAssignment("D", "M");
    writeJump(label, "D", "JNE");


}

void writeFunction(const char* name, const int nVars)
{
    writeLabel(name);

    Command command = {C_PUSH, "constant", 0};
    for(int i = 0; i < nVars; ++i)
    {
        writePush(command);
    }
}

void writePushPtr(const char* ptr)
{
    writeAt(ptr);
    Command command = {C_PUSH, "M", 0};
    writePush(command);
}

void writeCall(const char* name, const int nArgs)
{
    static unsigned int retAdd = 0;

    writePushPtr("LCL");
    writePushPtr("ARG");
    writePushPtr("THIS");
    writePushPtr("THAT");

    //write ARG = SP-5-nArgs
    // push SP onto stack
    Command command1 = {C_PUSH, "SP", 0};
    writePush(command1);

    // push 5 - nArgs
    Command command2 = {C_PUSH, "constant", 5 - nArgs};
    writePush(command2);

    // subtract
    writeTwoInputOp("-");

    // set ARG
    writeAt("SP");

    writeAssignment("A", "A-1");
    writeAssignment("D", "M");

    writeAt("ARG");
    writeAssignment("M", "D");

    // set LCL
    writeAt("SP");
    writeAssignment("D", "M");

    writeAt("LCL");
    writeAssignment("M", "D");

    // return label
    char retLabel[2 * MAX_STR_LENGTH];
    getRetLabel(peekNameStack(), retLabel);
    writeLabel(retLabel);

    // goto f
    writeGoto(name);
    strcpy(CURR_FUNC_NAME, name);
    pushNameStack(CURR_FUNC_NAME, retAdd++);
}

void writeReturn()
{
    // frame = LCL
    writeAt("LCL");
    writeAssignment("D", "M");
    writeAt("R14");
    writeAssignment("M", "D");

    //Command command0 = {C_PUSH, "R14", 0};
    //writePush(command0);
    //Command command1 = {C_PUSH, "constant", 5};
    //writePush(command1);
    //writeTwoInputOp("-");

    //Command command2 = {C_POP, "R15", 0};
    //writePop(command2);

    //writeAt("R15");
    //writeAssignment("D", "M");

    Command command0 = {C_POP, "ARG", 0, "", 1};
    writePop(command0);

    writeAt("ARG");
    writeAssignment("D", "M");
    writeAt("SP");
    writeAssignment("M", "D+1");

    // THAT = *(frame - 1)
    writeAt("R14");
    writeAssignment("M", "M-1");
    writeAssignment("D", "M");

    writeAssignment("A", "D");
    writeAssignment("D", "M");
    writeAt("THAT");
    writeAssignment("M", "D");

    // THIS = *(frame - 2)
    writeAt("R14");
    writeAssignment("M", "M-1");
    writeAssignment("D", "M");

    writeAssignment("A", "D");
    writeAssignment("D", "M");
    writeAt("THIS");
    writeAssignment("M", "D");

    // ARG = *(frame - 3)
    writeAt("R14");
    writeAssignment("M", "M-1");
    writeAssignment("D", "M");

    writeAssignment("A", "D");
    writeAssignment("D", "M");
    writeAt("ARG");
    writeAssignment("M", "D");

    // LCL  = *(frame - 4)
    writeAt("R14");
    writeAssignment("M", "M-1");
    writeAssignment("D", "M");

    writeAssignment("A", "D");
    writeAssignment("D", "M");
    writeAt("LCL");
    writeAssignment("M", "D");

    // goto retAddr
    popNameStack();
    char retLabel[2 * MAX_STR_LENGTH];
    getRetLabel(peekNameStack(), retLabel);
    writeGoto(retLabel);
}

void createIncLabel(const char* label, char* buff, int inc)
{
    strcpy(buff, label);
    //itoa(inc, buff + strlen(label), 10);
    sprintf(buff + strlen(label), "%d", inc);
}

void pushNameStack(char* name, int rCount)
{
    STACK_ELE* ele = &STACK[STACK_IDX];
    ele->rCount = rCount;
    strcpy(ele->name, name);
    ++STACK_IDX;
}

STACK_ELE popNameStack()
{
    --STACK_IDX;
    return STACK[STACK_IDX];
}

STACK_ELE* peekNameStack()
{
    return &STACK[STACK_IDX - 1];
}

void getRetLabel(const STACK_ELE* ele, char* retLabel)
{
    strcpy(retLabel, ele->name);
    char retSuffix[MAX_STR_LENGTH];
    sprintf(retSuffix, "$ret%d", ele->rCount);
    strcat(retLabel, retSuffix);
}
