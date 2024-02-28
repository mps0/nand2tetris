/* parser.h */

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"

//since these is VM code we can assume:
// - no extra leading spaces
// - no extra spcaces between commands & args
// - comments on their own lines

extern FILE* fREAD;
extern FILE* fWRITE;

int peek();
bool hasMoreLines();
void advance();

Command getCurrentCommand();
CommandType commandType();
CommandType getCommandTypeFromString(char* command);
void writeCommandAsComment(Command command);


void readNextWord(char* buff);
