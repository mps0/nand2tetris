/* parser.h */

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//since these is VM code we can assume:
// - no extra leading spaces
// - no extra spcaces between commands & args
// - comments on their own lines

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
    char arg1[9]; //fits "constant"
    char arg2[7]; //fits largest num string "-32768"
} Command;

int peek();
bool hasMoreLines();
void advance();

void setCurrentCommand();
CommandType commandType();
CommandType getCommandTypeFromString(char* command);
void writeCurrentCommandAsComment();
