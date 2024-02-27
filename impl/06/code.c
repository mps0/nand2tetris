#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "code.h"
#include "parser.h"

#define DMAP_SIZE 8
#define CMAP_SIZE 28
#define JMAP_SIZE 7

DMap dMap[DMAP_SIZE] =
{
    {"M", "001"},
    {"D", "010"},
    {"DM", "011"},
    {"MD", "011"},
    {"A", "100"},
    {"AM", "101"},
    {"AD", "110"},
    {"ADM", "111"}
};

CMap cMap[CMAP_SIZE] =
{
    {"0", "0101010"},
    {"1", "0111111"},
    {"-1", "0111010"},
    {"D", "0001100"},
    {"A", "0110000"},
    {"!D", "0001101"},
    {"!A", "0110001"},
    {"-D", "0001111"},
    {"-A", "0110011"},
    {"D+1", "0011111"},
    {"A+1", "0110111"},
    {"D-1", "0001110"},
    {"A-1", "0110010"},
    {"D+A", "0000010"},
    {"D-A", "0010011"},
    {"A-D", "0000111"},
    {"D&A", "0000000"},
    {"D|A", "0010101"},
    {"M","1110000"},
    {"!M", "1110001"},
    {"-M", "1110011"},
    {"M+1", "1110111"},
    {"M-1", "1110010"},
    {"D+M", "1000010"},
    {"D-M", "1010011"},
    {"M-D", "1000111"},
    {"D&M", "1000000"},
    {"D|M", "1010101"},
};

JMap jMap[JMAP_SIZE] =
{
    {"JGT", "001"},
    {"JEQ", "010"},
    {"JGE", "011"},
    {"JLT", "100"},
    {"JNE", "101"},
    {"JLE", "110"},
    {"JMP", "111"},
};

void destBin(char* d, char* bin)
{
    // get ddd
    for(int i = 0; i < DMAP_SIZE; i++) {
        if(strcmp(d, dMap[i].dest) == 0)
        {
            memcpy(bin, dMap[i].bits, 3);
            break;
        }
    }
}

void compBin(char* c, char* bin)
{
    // get cccccc
    for(int i = 0; i < CMAP_SIZE; i++) {
        if(strcmp(c, cMap[i].comp) == 0)
        {
            memcpy(bin, cMap[i].bits, 7);
            break;
        }
    }
}

void jumpBin(char* j, char* bin)
{
    // get cccccc
    for(int i = 0; i < JMAP_SIZE; i++) {
        if(strcmp(j, jMap[i].jump) == 0)
        {
            memcpy(bin, jMap[i].bits, 3);
            break;
        }
    }
}


SymbolTable symbolTable =
{
    .sd = {{"R0", "0"},
        {"R1", "1"},
        {"R2", "2"},
        {"R3", "3"},
        {"R4", "4"},
        {"R5", "5"},
        {"R6", "6"},
        {"R7", "7"},
        {"R8", "8"},
        {"R9", "9"},
        {"R10", "10"},
        {"R11", "11"},
        {"R12", "12"},
        {"R13", "13"},
        {"R14", "14"},
        {"R15", "15"},
        {"SP", "0"},
        {"LCL", "1"},
        {"ARG", "2"},
        {"THIS", "3"},
        {"THAT", "4"},
        {"SCREEN", "16384"},
        {"KBD", "24576"},
    },

    .length = 23,
    .nextAddress = 16
};

void printSymbolTable()
{
    for(int i = 0; i < symbolTable.length; i++)
    {
        printf("%s %s\n", symbolTable.sd[i].symbol, symbolTable.sd[i].dec);
    }
}

void addEntry(char* symbol, char* address)
{
    sprintf(symbolTable.sd[symbolTable.length].symbol, "%s", symbol);
    sprintf(symbolTable.sd[symbolTable.length].dec, " %s", address);
    symbolTable.length++;
}

void iaddEntry(char* symbol, int address)
{
    sprintf(symbolTable.sd[symbolTable.length].symbol, "%s", symbol);
    sprintf(symbolTable.sd[symbolTable.length].dec, "%d", address);
    symbolTable.length++;
}

bool contains(char* symbol)
{
    for(int i = 0; i < symbolTable.length; i++)
    {
        //printf("SYMBOL :%s, TABLE[%d]: %s strcmp value: %d\n", symbol, i, symbolTable.sd[i].symbol, strcmp(symbolTable.sd[i].symbol, symbol) );
        if(strcmp(symbolTable.sd[i].symbol, symbol) == 0)
        {
            return true;
        }
    }
    return false;
}

int getAddress(char* symbol)
{
    for(int i = 0; i < symbolTable.length; i++)
    {
        if(strcmp(symbolTable.sd[i].symbol, symbol) == 0)
        {
            return decimalStringToInt(symbolTable.sd[i].dec);
        }
    }
    return -1;
}
