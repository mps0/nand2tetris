#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LINE_MAX_BYTES 512

extern FILE* fp;

int peek();

bool hasMoreLines();

void advance();

void process(char* line, int* n);

void parse(bool firstPass);

int decimalStringToInt(char* s);
