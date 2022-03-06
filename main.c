#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "vm.h"
#include "compiler.h"


int open_file();

int write_bytecode_test();

int read_bytecode(const char *filename);


int text[10];

extern char *src;
int main() {
//    write_bytecode_test();
//    read_bytecode("src.cbc");

    init_compiler();
    read_src("src.c");
    keyword();
    while(*src != 0){
        tokenize();
    }
//    d_printSymbolTable();
    return 0;
}

int open_file() {
    FILE *fp;
    char *filename = "src.cbc";

}


int write_bytecode_test() {
    int nbytecode = 10;
    int i = 0;
    text[i++] = 0;
    text[i++] = 0;
    text[i++] = IMM;
    text[i++] = 16;
    text[i++] = PUSH;
    text[i++] = IMM;
    text[i++] = 2;
    text[i++] = ADD;
    text[i++] = PUSH;
    text[i++] = EXIT;


    FILE *fp;
    char *filename = "src.cbc";
    fp = fopen(filename, "rb+");
    if (fp == NULL) {
        printf("ASM is not found! Create new one.\n");
    }
    fwrite(text, sizeof(int), nbytecode, fp);
    fclose(fp);
    for (int i = 0; i < nbytecode; i++) {
        printf("%d 0x%08X\n", text[i], text[i]);
    }
    printf("\n");
}

