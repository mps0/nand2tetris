/* parser.h */

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LINE_MAX_BYTES 512


extern FILE* fREAD;
extern FILE* fWRITE;
typedef enum CommandType
{
    C_ARITHEMATIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
    C_UNDEFINED
} CommandType;

typedef struct
{
    CommandType type;
    char arg1[10]; ///TODO... how many bytes are really needed here?
    char arg2[10]; ///TODO... how many bytes are really needed here?
} Command;

int peek();

bool hasMoreLines();

void advance();
void setCurrentCommand();

CommandType commandType();
CommandType getCommandTypeFromString(char* command);

void arg1(char* rStr, int len);

void writeCurrentCommandAsComment();








