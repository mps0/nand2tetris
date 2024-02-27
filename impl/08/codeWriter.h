/* codeWriter.h */

#pragma once

#include <stdio.h>

#include "common.h"

#define BASE_STACK_ADDRESS_STRING "256"
//#define BASE_LCL_ADDRESS_STRING ""
//#define BASE_ARG_ADDRESS_STRING ""
//#define BASE_THIS_ADDRESS_STRING ""
//#define BASE_THAT_ADDRESS_STRING ""
//


typedef struct
{
    int rCount;
    char name[MAX_STR_LENGTH];
} STACK_ELE;


typedef enum
{
    A_ADD,
    A_SUB,
    A_NEG,
    A_EQ,
    A_GT,
    A_LT,
    A_AND,
    A_OR,
    A_NOT,
    A_UNDEFINED
} ArithType;

extern FILE* fWRITE;

// API
void writeArithmetic(const Command command);
void writePushPop(const Command command);

// helpers
void init();
void finish();

void setFileName(const char* name);

void writePush(const Command command);
void writePop(const Command command);
void writeAt(const char* addr);
void writeAssignment(const char* lh, const char* rh);
void writeLogical(const char* jump);
void writeLabel(const char* label);
void writeJump(const char* label, const char* cond, const char* jump);
void writeOneInputOp(const char* op);
void writeTwoInputOp(const char* op);
void writeGoto(const char* label);
void writeIf(const char* label);
void writeFunction(const char* name, const int nVars);
void writeCall(const char* name, const int nArgs);
void writeReturn();

void writePushPtr(const char* ptr);
void pushNameStack(char* name, int rCount);
STACK_ELE popNameStack();
STACK_ELE* peekNameStack();
void getRetLabel(const STACK_ELE* ele, char* retLabel);

ArithType getArithType(const Command command);

void createIncLabel(const char* label, char* buff, int inc);

