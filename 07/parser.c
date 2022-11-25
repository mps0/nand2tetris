/* parser.c */
#include <ctype.h>

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
    while(p == '\n')
    {
        getc(fREAD);
        p = peek();
    }
    printf("hasMoreLines, peek: %c, ascii: %d\n", p, p);
    //if(peek() == EOF)
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
    //while(c == ' ')
    //{
    //    c = getc(fREAD);
    //}

    if (c == '/' && peek()  == '/')
    {
        goToNextLine = true;
    }
    //TODO
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
            printf("GOING TO NEXT LINE\n");
            advance();
        }
        return;
    }
    fseek(fREAD, -1, SEEK_CUR);
}

void setCurrentCommand()
{
    char c = getc(fREAD);

    //since these is VM code we can assume:
    // - no extra leading spaces
    // - no extra spcaces between commands & args
    // - comments on their own lines

    //get command
    char command[8]; //TODO
    int n = 0;
    while(c != ' ' && c != '\n')
    {
        printf("n: %d, c: %c\n", n, c);
        command[n] = c;
        c = getc(fREAD);
        n++;
    }
    command[n] = '\0';
    currCommand.type = getCommandTypeFromString(command);

    printf("COMMAND: %s, currCommand.type: %d\n", command, currCommand.type);

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
                    printf("arg0 n: %d, c: %c\n", n, c);
                    currCommand.arg1[n] = c;
                    c = getc(fREAD);
                    n++;
                }
                currCommand.arg1[n] = '\0';
                printf("done0\n");

                n = 0;
                c = getc(fREAD);
                while(c != '\n')
                {
                    printf("arg1 n: %d, c: %c ascii: %u\n", n, c, c);
                    currCommand.arg2[n] = c;
                    c = getc(fREAD);
                    n++;
                }
                currCommand.arg2[n] = '\0';
                printf("done1\n");
                break;
            }
    }
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

void writeCurrentCommandAsComment()
{
    fputs("\\\\", fWRITE);

    switch(currCommand.type)
    {
        case C_ARITHEMATIC:
            fputs(currCommand.arg1, fWRITE);
            break;

        case C_PUSH:
            fputs("push", fWRITE);
            fputs(" ", fWRITE);
            fputs(currCommand.arg1, fWRITE);
            fputs(" ", fWRITE);
            fputs(currCommand.arg2, fWRITE);
            break;
        case C_POP:
            fputs("pop", fWRITE);
            fputs(" ", fWRITE);
            fputs(currCommand.arg1, fWRITE);
            fputs(" ", fWRITE);
            fputs(currCommand.arg2, fWRITE);
            break;
    }
    fputs("\n", fWRITE);
}
