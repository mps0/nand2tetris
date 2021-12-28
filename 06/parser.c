#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parser.h"

#define LINE_MAX_BYTES 512
#define WORD_BIT_SIZE 16

extern FILE* fREAD;
extern FILE* fWRITE;

typedef enum {
    A_INSTRUCTION,
    C_INSTRUCTION,
    L_INSTRUCTION
} INSTRUCTION_TYPE;

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
    while(c != '\n' && c != EOF && *n < LINE_MAX_BYTES - 2)
    {
        if (c == '/' && peek()  == '/')
        {
            *n = startOfSpace;
            fseek(fREAD, -1, SEEK_CUR);
            advance();
            break;
        }
        else if (c == ' ')
        {
            startOfSpace = *n;
        }

        line[*n] = c;
        ++(*n);
        c = getc(fREAD);
    }
    line[*n] = '\0';
}

// positive only
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
        //binary[15 - i] = n % 2 + 48;
        binary[i] = n % 2 + 48;
        n /= 2;
    }
    binary = strrev(binary);
}

INSTRUCTION_TYPE instructionType(char firstByte)
{
    switch(firstByte)
    {
        case '@' :
            printf("@\n");
            return A_INSTRUCTION;

        case '(' :
            printf("(\n");
            return L_INSTRUCTION;

        default :
            printf("DEFAULT\n");
            return C_INSTRUCTION;
    }
}


typedef struct {
    char dest[4];
    char bits[4];
} DMap;

typedef struct {
    char comp[7];
    char bits[7];
} CMap;

typedef struct {
    char jump[4];
    char bits[4];
} JMap;



DMap dMap[7] =
{
    {"M", "001"},
    {"D", "010"},
    {"DM", "011"},
    {"A", "100"},
    {"AM", "101"},
    {"AD", "110"},
    {"ADM", "111"}
};

CMap cMap[18] =
{
    {"0", "101010"},
    {"1", "111111"},
    {"-1", "111010"},
    {"D", "001100"},
    {"X", "110000"},
    {"!D", "001101"},
    {"!X", "110001"},
    {"-D", "001111"},
    {"-X", "110011"},
    {"D+1", "011111"},
    {"X+1", "110111"},
    {"D-1", "001110"},
    {"X-1", "110010"},
    {"D+X", "000010"},
    {"D-X", "010011"},
    {"X-D", "000111"},
    {"D&A", "000000"},
    {"D|A", "010101"},
};

JMap jMap[7] =
{
    {"JGT", "001"},
    {"JEQ", "010"},
    {"JGE", "011"},
    {"JLT", "100"},
    {"JNE", "101"},
    {"JLE", "110"},
    {"JMP", "111"},
};

char* dest(char* line)
{
    int eqLoc = 0;
    while(line[eqLoc] != '=')
    {
        eqLoc++;
    }
    printf("EQLOC: %d\n", eqLoc);
    char* rval = calloc(eqLoc + 1, sizeof(char));
    memcpy(rval, line, eqLoc);
    rval[eqLoc] = '\0';
    return rval;
}


char* comp(char* line)
{
    int eqLoc = 0;
    while(line[eqLoc] != '=')
    {
        //printf("line[eqLoc] = %c\n", line[eqLoc]);
        //dest[eqLoc] = line[eqLoc];
        eqLoc++;
    }

    return line + eqLoc + 1;
}

char* destBin(char* d) {

    char* rval = calloc(4, sizeof(char));
    // get ddd
    for(int i = 0; i < 7; i++) {
        if(strcmp(d, dMap[i].dest) == 0)
        {
           memcpy(rval, dMap[i].bits, 4);
           break;
        }
    }
    return rval;
}

char* compBin(char* c)
{

    char* rval = calloc(7, sizeof(char));

    // get cccccc
    for(int i = 0; i < 18; i++) {
        if(strcmp(c, cMap[i].comp) == 0)
        {
           memcpy(rval, cMap[i].bits, 7);
           break;
        }
    }
           return rval;
}



void parseCinstruction(char* line, char* bin)
{

    //remember to clean
    char* d = dest(line);
    printf("dest return: %s\n", d);
    char* ddd = destBin(d);
    printf("ddd: %s\n", ddd);

    // get cccccc
    //char* RHS = line + eqLoc + 1;
    char* c = comp(line);
    printf("comp Returns: %s\n", c);
    char* cccccc = compBin(c);
    printf("cccccc: %s\n", ddd);


    //TODO
    char jjj[4] = "000";
    // get jjj
    //
    char* res = strcat(cccccc, ddd);
    res = strcat(res, jjj);
    memcpy(bin + 4, res, 15);
    free(ddd);
    free(cccccc);
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

        char bin[WORD_BIT_SIZE + 1];
        bin[0] = '1';
        bin[0] = '1';
        bin[0] = '1';
        bin[16] = '\0';
        printf("line: %s\n", line);
        switch(instructionType(line[0])) {
            case A_INSTRUCTION:
                {

                    int dec = decimalStringToInt(line + 1);
                    //printf("decString: %d", decString);
                    decimalToBinary(dec, bin);
                    //printf("converted line: %s\n", bin);
                    break;
                }

            case C_INSTRUCTION:
                {
                    printf("PARSING C INSTRUCTION\n");
                    parseCinstruction(line, bin);
            printf("bin: %s\n", bin);
                    break;
                }

            case L_INSTRUCTION:
                {

                    break;
                }
        }

        fputs(bin, fWRITE);
        fputc('\n', fWRITE);

        //printf("extracted line: %s\n", line);
    }
}
