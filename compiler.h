//
// Created by LXY on 2022/3/6.
//

#ifndef C_COMPILER_COMPILER_H
#define C_COMPILER_COMPILER_H
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "io.h"

#define SEG_SIZE (1024*1024)
#define SYMBOL_TABLE_SIZE (1024*1024)

enum {
    Num = 128, Fun, Sys, Glo, Loc, Id,
    Char, Else, Enum, If, Int, Return, Sizeof, While,
    Assign, Cond, Lor, Land, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

// 标识符在symbol_table的偏移量
enum {
    Token,      // 标识符的标记值
    Hash,       // 哈希值
    Name,       // 标识符本身的字符串首地址
    Type,       // 标识符的内存类型(int, char, ptr)
    Class,      // 标识符的类别(内置函数，数字，全局变量，局部变量, 函数)
    Value,      // 标识符的值
    GType,      // 用于全局标识符
    GClass,     //
    GValue,     //
    Offset      //总偏移量
};

enum{
    CHAR, INT, PTR
};

int read_src(const char *filename);

int init_compiler();
void tokenize();
void keyword();

void d_printSymbolTable();


#endif //C_COMPILER_COMPILER_H
