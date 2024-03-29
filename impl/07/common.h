/* common.h */

#pragma once
typedef enum
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
    int arg2;
} Command;




