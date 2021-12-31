#pragma once

#define MAX_SYMBOL_SIZE 100
#define SYMBOL_TABLE_SIZE 10000

typedef struct {
    char symbol[MAX_SYMBOL_SIZE + 1];
    char dec[17];
} SymbolDec;

typedef struct{
    SymbolDec sd[SYMBOL_TABLE_SIZE];
    int length;
    int nextAddress;
} SymbolTable;

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


void destBin(char* d, char* bin);
void compBin(char* c, char* bin);
void jumpBin(char* j, char* bin);

void printSymbolTable();

void addEntry(char* symbol, char* address);
void iaddEntry(char* symbol, int address);

bool contains(char* symbol);
int getAddress(char* symbol);
