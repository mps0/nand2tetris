/* VMTranslator.c */
#include <stdio.h>

#include "parser.h"

FILE* fREAD;
FILE* fWRITE;

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        printf("No VM file provided\n");
        return -1;
    }
    else if (argc > 2)
    {
        printf("Too many input arguments\n");
        return -1;
    }

    char* pc = argv[1];
    bool extensionFound = false;
    int extensionLoc = 0;
    int i = 0;
    while(*pc != '\0')
    {
        if(*pc == '.')
        {
            extensionFound = true;
            extensionLoc = i;

            if(strcmp(pc + 1, "vm") != 0)
            {
                printf("Incompatible file extension: .%s\n", pc + 1);
                return -1;
            }
        }
        ++pc;
        ++i;
    }

    if(!extensionFound)
    {
        printf(".vm file extension not found\n");
        return -1;
    }

    fREAD = fopen(argv[1], "r");
    if(!fREAD)
    {
        printf("Could not open file %s\n", argv[1]);
        return -1;
    }
    char name[extensionLoc + 2];
    memcpy(name, argv[1], extensionLoc) ;

    fWRITE= fopen(strcat(name, ".asm"), "w");
    if(!fWRITE)
    {
        printf("Could not create output machine code file!\n");
        return -1;
    }

    while(hasMoreLines())
    {
        advance();

        setCurrentCommand();
        writeCurrentCommandAsComment();
    }

    fclose(fREAD);
    fclose(fWRITE);

    return 0;
}












