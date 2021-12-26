#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"

#define LINE_MAX_BYTES 512

extern FILE* fp;

int peek()
{
    char c = getc(fp);
    fseek(fp, -1, SEEK_CUR);

    return c;
}

bool hasMoreLines()
{
    if(peek() == EOF)
    {
        return false;
    }

    return true;
};

void advance()
{
    bool goToNextLine = false;

    char c = getc(fp);
    while(c == ' ')
    {
        c = getc(fp);
    }

    if (c == '/' && peek()  == '/')
    {
        goToNextLine = true;
    }

    if(goToNextLine)
    {
        while(c != '\n')
        {
            c = getc(fp);
        }
        advance();
        return;
    }
    fseek(fp, -1, SEEK_CUR);
}

void process(char* line, int* n)
{
    *n = 0;
    int startOfSpace;
    char c = getc(fp);
    while(c != '\n' && c != EOF && *n < LINE_MAX_BYTES - 2)
    {
        if (c == '/' && peek()  == '/')
        {
            *n = startOfSpace;
            fseek(fp, -1, SEEK_CUR);
            advance();
            break;
        }
        else if (c == ' ')
        {
            startOfSpace = *n;
        }

        line[*n] = c;
        ++(*n);
        c = getc(fp);
    }
    line[*n] = '\0';
}

void parse()
{

    char line[LINE_MAX_BYTES];
    int length;
    while(hasMoreLines())
    {
        advance();
        process(line, &length);
        if(length == 0) {
            continue;
        }
        printf("extracted line: %s\n", line);
    }
}
