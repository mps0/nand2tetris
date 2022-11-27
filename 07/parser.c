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
};

void advance()
{
    bool goToNextLine = false;

    char c = getc(fREAD);
    if (c == '/' && peek()  == '/')
    {
        goToNextLine = true;
    }
    // can assume the VM code doesn't have leading spaces
    // since it's generated, so just skip these lines.
    else if(isspace(c) != 0)
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
    char c = getc(fREAD);

    //get command
    char command[7]; //fits "return"
    int n = 0;
    while(c != ' ' && c != '\n')
    {
        command[n] = c;
        c = getc(fREAD);
        n++;
    }
    command[n] = '\0';

    Command currCommand = {};
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
                n = 0;
                c = getc(fREAD);
                while(c != ' ')
                {
                    currCommand.arg1[n] = c;
                    c = getc(fREAD);
                    n++;
                }
                currCommand.arg1[n] = '\0';

                n = 0;
                c = getc(fREAD);
                char arg2[7]; // big enough to fit smallest 16bit int
                while(c != '\n')
                {
                    arg2[n] = c;
                    c = getc(fREAD);
                    n++;
                }
                arg2[n] = '\0';
                currCommand.arg2 = atoi(arg2);
                break;
            }
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
                fputs(itoa(command.arg2, buff, 10), fWRITE);
                break;
            }
        case C_POP:
            {
                fputs("pop", fWRITE);
                fputs(" ", fWRITE);
                fputs(command.arg1, fWRITE);
                fputs(" ", fWRITE);
                char buff[7];
                fputs(itoa(command.arg2, buff, 10), fWRITE);
                break;
            }
    }
    fputs("\n", fWRITE);
}
