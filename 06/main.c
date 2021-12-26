#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"

FILE* fp;

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
    parse();

    fclose(fp);

    return 0;
}
