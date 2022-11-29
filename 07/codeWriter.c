/* codeWriter.c */

#include <stdlib.h>
#include <string.h>

#include "codeWriter.h"
#include "parser.h"


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
        char buff[7];
        itoa(command.arg2, buff, 10);
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
            writeAssignment("A", "M");
            // copy value
            writeAssignment("D", "M");
        }
        else
        {
            char buff[7];
            itoa(command.arg2, buff, 10);
            writeAt(buff);
            writeAssignment("D","A");
            writeAt(command.arg1);
            writeAssignment("D", "D+M");
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

    if(command.arg2 == 0)
    {
        writeAt(command.arg1);
        writeAssignment("A", "M");
        writeAssignment("M","D");
    }
    else
    {
        //Save the value into R13
        writeAt("R13");
        writeAssignment("M","D");
        char buff[7];
        itoa(command.arg2, buff, 10);
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
        writeAssignment("M","D");
    }
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

    //TODO
    //writeAssignment("M", "-M");
    fputs("M=", fWRITE);
    fputs(op, fWRITE);
    fputs("M\n", fWRITE);
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
    fputs("M=M", fWRITE);
    fputs(op, fWRITE);
    fputs("D\n", fWRITE);
}


void writeLogical(const char* jump)
{
    static int inc = 0;

    //TODO
    char buff[32];
    createIncLabel("TRUE", buff, inc);
    //TODO
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
    fputs("(", fWRITE);
    fputs(label, fWRITE);
    fputs(")\n", fWRITE);
}

void writeAt(const char* addr)
{
    //fputs(strcat("@", addr), fWRITE);
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

void createIncLabel(const char* label, char* buff, int inc)
{
    strcpy(buff, label);
    itoa(inc, buff + strlen(label), 10);
}
