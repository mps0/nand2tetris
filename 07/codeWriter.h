/* codeWriter.h */

#pragma once

#include <stdio.h>

#include "common.h"

#define BASE_STACK_ADDRESS_STRING "256"

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

void writePush(const Command command);
void writePop(const Command command);
void writeAt(const char* addr);
void writeAssignment(const char* lh, const char* rh);
void writeLogical(const ArithType arithType);
void writeLabel(const char* label);
void writeJump(const char* label, const char* cond, const char* jump);

ArithType getArithType(const Command command);

void createIncLabel(const char* label, char* buff, int inc);

