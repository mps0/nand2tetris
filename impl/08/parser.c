/* parser.c */
#include <ctype.h>
#include <stdlib.h>

#include "parser.h"

Command currCommand;

int peek()
{
    char c = getc(fREAD);
    fseek(fREAD, -1, SEEK_CUR);

    return c;
}

bool hasMoreLines()
{
    char p = peek();
    // skip over lines with just a new line char.
    while(p == '\n')
    {
        getc(fREAD);
        p = peek();
    }

    if(p == EOF)
    {
        return false;
    }

    return true;
}

void advance()
{
    bool goToNextLine = false;

    char c = getc(fREAD);

    while(isspace(c) != 0)
    {
        if(c == '\n')
        {
            goToNextLine = true;
            break;
        }
        c = getc(fREAD);
    }

    if (c == '/' && peek() == '/')
    {
        goToNextLine = true;
    }

    if(goToNextLine)
    {
        while(c != '\n')
        {
            c = getc(fREAD);
        }

        if(hasMoreLines())
        {
            advance();
        }
        return;
    }
    fseek(fREAD, -1, SEEK_CUR);
}

Command getCurrentCommand()
{
    //get command
    char command[8]; //fits "function"
    readNextWord(command);

    Command currCommand;
    //printf("command: %s\n", command);
    currCommand.type = getCommandTypeFromString(command);
            //get args (if needed)
    switch(currCommand.type)
    {
        case  C_ARITHEMATIC:
            strcpy(currCommand.arg1, command);
            break;

        case C_PUSH:
        case C_POP:
            {
                readNextWord(currCommand.arg1);

                char arg2[7]; // big enough to fit smallest 16bit int
                readNextWord(arg2);
                currCommand.arg2 = atoi(arg2);
                currCommand.isPointer = 0;

                if(strcmp(currCommand.arg1, "local") == 0)
                {
                    memcpy(currCommand.arg1, "LCL", 4);
                    currCommand.isPointer = 1;
                }

                else if(strcmp(currCommand.arg1, "argument") == 0)
                {
                    memcpy(currCommand.arg1, "ARG", 4);
                    currCommand.isPointer = 1;
                }

                else if(strcmp(currCommand.arg1, "this") == 0)
                {
                    memcpy(currCommand.arg1, "THIS", 5);
                    currCommand.isPointer = 1;
                }

                else if(strcmp(currCommand.arg1, "that") == 0)
                {
                    memcpy(currCommand.arg1, "THAT", 5);
                    currCommand.isPointer = 1;
                }

                else if(strcmp(currCommand.arg1, "temp") == 0)
                {
                    memcpy(currCommand.arg1, "TEMP", 5);
                    currCommand.isPointer = 1;
                }

                else if(strcmp(currCommand.arg1, "pointer") == 0)
                {
                    memcpy(currCommand.arg1, "POINTER", 8);
                    currCommand.isPointer = 1;
                }
                break;
            }

        case C_LABEL:
        case C_FUNCTION:
        case C_GOTO:
        case C_IF:
            readNextWord(currCommand.fname);
            break;
    }

    return currCommand;
}

CommandType getCommandTypeFromString(char* command)
{
    //Check push/pop commands
    if(strcmp("push", command) == 0)
        return C_PUSH;

    if(strcmp("pop", command) == 0)
        return C_POP;

    //Check arithmetic commands
    if(strcmp("add", command) == 0
            || strcmp("sub", command) == 0
            || strcmp("neg", command) == 0
            || strcmp("eq", command) == 0
            || strcmp("gt", command) == 0
            || strcmp("lt", command) == 0
            || strcmp("and", command) == 0
            || strcmp("or", command) == 0
            || strcmp("not", command) == 0
      )
        return C_ARITHEMATIC;

    if(strcmp("label", command) == 0)
        return C_LABEL;

    if(strcmp("function", command) == 0)
        return C_FUNCTION;

    if(strcmp("goto", command) == 0)
        return C_GOTO;

    if(strcmp("if-goto", command) == 0)
        return C_IF;

    return C_UNDEFINED;
}

CommandType commandType()
{
    return currCommand.type;
}

void writeCommandAsComment(Command command)
{
    fputs("// ", fWRITE);

    switch(command.type)
    {
        case C_ARITHEMATIC:
            fputs(command.arg1, fWRITE);
            break;

        case C_PUSH:
            {
                fputs("push", fWRITE);
                fputs(" ", fWRITE);
                fputs(command.arg1, fWRITE);
                fputs(" ", fWRITE);
                char buff[7];
                sprintf(buff, "%d", command.arg2);
                fputs(buff, fWRITE);
                break;
            }
        case C_POP:
            {
                fputs("pop", fWRITE);
                fputs(" ", fWRITE);
                fputs(command.arg1, fWRITE);
                fputs(" ", fWRITE);
                char buff[7];
                sprintf(buff, "%d", command.arg2);
                fputs(buff, fWRITE);
                break;
            }
        case C_LABEL:
            {
                fputs("label", fWRITE);
                fputs(" ", fWRITE);
                fputs(command.fname, fWRITE);
                break;
            }
        case C_FUNCTION:
            {
                fputs("function", fWRITE);
                fputs(" ", fWRITE);
                fputs(command.fname, fWRITE);
                break;
            }
        case C_GOTO:
            {
                fputs("goto", fWRITE);
                fputs(" ", fWRITE);
                fputs(command.fname, fWRITE);
                break;
            }
        case C_IF:
            {
                fputs("if-goto", fWRITE);
                fputs(" ", fWRITE);
                fputs(command.fname, fWRITE);
                break;
            }
    }
    fputs("\n", fWRITE);
}

void readNextWord(char* buff)
{
    char c = getc(fREAD);

    while(c == ' ')
    {
        c = getc(fREAD);
    }

    //get command
    int n = 0;
    while(isspace(c) == 0)
    {
        buff[n] = c;
        c = getc(fREAD);
        n++;
    }
    buff[n] = '\0';
}

