#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LINE_MAX_BYTES 512

FILE* fp;

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

    if (c = '/' && peek()  == '/')
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
        if (c == '/' && peek(c)  == '/')
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

int main(int argc, char **argv)
{
    if(argc == 1)
    {
        printf("No assembly file provided\n");
        return -1;
    }
    else if (argc > 2)
    {
        printf("Too many input arguments\n");
        return -1;
    }

    char* pc = argv[1];
    bool extensionFound = false;
    while(*pc != '\0')
    {
        if(*pc == '.')
        {
            extensionFound = true;

            if(strcmp(pc + 1, "asm") != 0)
            {
                printf("Incaptible file extension: .%s\n", pc + 1);
                return -1;
            }
        }
        ++pc;
    }

    if(!extensionFound)
    {
        printf("No file extension found\n");
        return -1;
    }

    fp = fopen(argv[1], "r");
    if(!fp)
    {
        printf("Could not open file %s\n", argv[1]);
        return -1;
    }

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


    fclose(fp);

    return 0;
}
