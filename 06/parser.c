#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parser.h"
#include "code.h"

#define LINE_MAX_BYTES 512
#define WORD_BIT_SIZE 16

extern FILE* fREAD;
extern FILE* fWRITE;

extern SymbolTable symbolTable;

typedef enum {
    A_INSTRUCTION,
    C_INSTRUCTION,
    L_INSTRUCTION
} INSTRUCTION_TYPE;

typedef struct {
    char* line;
    INSTRUCTION_TYPE type;
} Info;

Info info;

int peek()
{
    char c = getc(fREAD);
    fseek(fREAD, -1, SEEK_CUR);

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

    char c = getc(fREAD);
    while(c == ' ')
    {
        c = getc(fREAD);
    }

    if (c == '/' && peek()  == '/')
    {
        goToNextLine = true;
    }

    if(goToNextLine)
    {
        while(c != '\n')
        {
            c = getc(fREAD);
        }
        advance();
        return;
    }
    fseek(fREAD, -1, SEEK_CUR);
}

void process(char* line, int* n)
{
    *n = 0;
    int startOfSpace;
    char c = getc(fREAD);
    bool seenNonSpace = false;
    while(c != '\n' && c != EOF && *n < LINE_MAX_BYTES - 2)
    {
        if (c == '/' && peek()  == '/')
        {
            *n = startOfSpace;
            fseek(fREAD, -1, SEEK_CUR);
            advance();
            break;
        }
        else if (c != ' ')
        {
            seenNonSpace = true;
        }
        else if (c == ' ' && seenNonSpace)
        {
            startOfSpace = *n;
            break;
        }

        line[*n] = c;
        ++(*n);
        c = getc(fREAD);
    }
    //printf("placcing null at :%d\n", *n);
    line[*n] = '\0';
}

int decimalStringToInt(char* s)
{
    int length = strlen(s);

    int sum = 0;
    int mult = 1;
    bool negative = false;
    int end = 0;
    if(s[0] == '-') {
        negative = true;
        ++end;
    }
    int i = length - 1;
    while(i >= end)
    {
        sum += mult * (s[i] - 48);
        mult *= 10;
        --i;
    }

    if(negative) {
        return -sum;
    }
    else {
        return sum;
    }
}

void decimalToBinary(int n, char* binary)
{
    if(n < 0){
        n += 32768; // 2^15
        binary[15] = '1';
    }
    else {
        binary[15] = '0';
    }
    for(int i = 0; i < WORD_BIT_SIZE - 1; ++i)
    {
        binary[i] = n % 2 + 48;
        n /= 2;
    }
    binary = strrev(binary);
}

void symbol(char* sym)
{
    switch(info.type)
    {
        case A_INSTRUCTION:
            int length = strlen(info.line);
            memcpy(sym, info.line + 1, length - 1);
            break;

        case L_INSTRUCTION:
            int i = 1;
            while(info.line[i] != ')')
            {
                i++;
            }
            memcpy(sym, info.line + 1, i - 1);
            break;
    }
}

INSTRUCTION_TYPE instructionType(char firstByte)
{
    switch(firstByte)
    {
        case '@' :
            return A_INSTRUCTION;

        case '(' :
            return L_INSTRUCTION;

        default :
            return C_INSTRUCTION;
    }
}

//returns true if jump
bool dest(char* d)
{
    int eqLoc = 0;
    while(1)
    {
        if(info.line[eqLoc] == '=')
        {
            memcpy(d, info.line, eqLoc);
            return false;
        }
        else if (info.line[eqLoc] == ';')
        {
            memcpy(d, info.line, eqLoc);
            return true;
        }
        eqLoc++;
    }
}

void comp(char* c)
{
    int eqLoc = 0;
    while(info.line[eqLoc] != '=' && info.line[eqLoc] != ';')
    {
        eqLoc++;
    }
    int length = strlen(info.line);
    memcpy(c, info.line + eqLoc + 1, length - eqLoc - 1);
}

void parseCinstruction(char* bin)
{
    char d[MAX_SYMBOL_SIZE] = {};
    char c[MAX_SYMBOL_SIZE] = {};
    char j[4] = {};

    char jjj[4] = "000";
    char ddd[4] = {};
    char acccccc[8] = {};

    bool isJump = dest(d);
    if(isJump)
    {
        compBin(d, acccccc);
        comp(j);
        jumpBin(j, jjj);
        ddd[0] = '0';
        ddd[1] = '0';
        ddd[2] = '0';
    }
    else
    {
        destBin(d, ddd);
        comp(c);
        compBin(c, acccccc);
    }

    char* res = strcat(acccccc, ddd);
    res = strcat(res, jjj);
    memcpy(bin + 3, res, 15);
}

void parse(bool firstPass)
{
    char line[LINE_MAX_BYTES];
    int length;
    int lineNumber = -1;
    while(hasMoreLines())
    {
        advance();
        process(line, &length);
        if(length == 0) {
            continue;
        }

        char bin[WORD_BIT_SIZE + 1];
        bin[16] = '\0';
        info = (Info){line, A_INSTRUCTION};
        switch(instructionType(line[0])) {
            case A_INSTRUCTION:
                {
                    if(!firstPass)
                    {
                        char sym[MAX_SYMBOL_SIZE] = {};
                        symbol(sym);

                        int dec;
                        if(!(sym[0] == '-' || (sym[0] >= '0' && sym[0]<= '9')))
                        {
                            if(!contains(sym))
                            {
                                iaddEntry(sym, symbolTable.nextAddress);
                                symbolTable.nextAddress++;
                            }
                            dec = getAddress(sym);
                        }
                        else
                        {
                            dec = decimalStringToInt(sym);
                        }

                        decimalToBinary(dec, bin);
                        fputs(bin, fWRITE);
                        fputc('\n', fWRITE);
                    }
                    else
                    {
                        lineNumber++;
                    }
                    break;
                }

            case C_INSTRUCTION:
                {
                    if(!firstPass)
                    {
                        bin[0] = '1';
                        bin[1] = '1';
                        bin[2] = '1';
                        parseCinstruction(bin);
                        fputs(bin, fWRITE);
                        fputc('\n', fWRITE);
                    }
                    else
                    {
                        lineNumber++;
                    }
                    break;
                }

            case L_INSTRUCTION:
                {
                    if(firstPass)
                    {
                        char sym[MAX_SYMBOL_SIZE] = {};
                        symbol(sym);
                        iaddEntry(sym, lineNumber + 1);
                    }
                    break;
                }
        }
    }
}
