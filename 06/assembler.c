#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"
#include "code.h"



FILE* fREAD;
FILE* fWRITE;

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
    int extensionLoc = 0;
    int i = 0;
    while(*pc != '\0')
    {
        if(*pc == '.')
        {
            extensionFound = true;
            extensionLoc = i;

            if(strcmp(pc + 1, "asm") != 0)
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
        printf(".asm file extension not found\n");
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

     fWRITE= fopen(strcat(name, ".hack"), "w");
    if(!fWRITE)
    {
        printf("Could not create output machine code file!\n");
        return -1;
    }
    //printSymbolTable();

    parse(true);
    fseek(fREAD, 0, SEEK_SET);
    parse(false);

    printSymbolTable();

    fclose(fREAD);
    fclose(fWRITE);

    return 0;
}
