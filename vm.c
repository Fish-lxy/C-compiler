//
// Created by LXY on 2022/3/1.
//

#include "vm.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "io.h"

int ax;     //ax通用寄存器
int *pc;    //程序计数器
int *sp;    //栈寄存器
int *bp;    //基地址寄存器

int *code;  //代码段
char *data; //数据段
int *stack; //栈段

int *ByteCode;
int nByteCodeFileLen;
int cycle;

int init_vm() {
    if (!(code = malloc(SEG_SIZE))) {
        printf("Can not malloc for code segment!\n");
        return -1;
    }
    if (!(data = malloc(SEG_SIZE))) {
        printf("Can not malloc for data segment!\n");
        return -1;
    }
    if (!(stack = malloc(SEG_SIZE))) {
        printf("Can not malloc for stack segment!\n");
        return -1;
    }
    memset(code, 0, SEG_SIZE);
    memset(data, 0, SEG_SIZE);
    memset(stack, 0, SEG_SIZE);

    return 0;
}

int init_reg() {
    ax = 0;
    bp = sp = (int *) ((int) stack + SEG_SIZE);
    cycle = 0;
}

int run_vm(int argc, char **argv) {
    int op, *tmp;
    while (1) {
        op = *pc++;
        cycle++;
        if (op == IMM) {
            ax = *pc++;
        } else if (op == LEA) {
            ax = (int) (bp + *pc++);
        } else if (op == LC) {
            ax = *(char *) ax;
        } else if (op == LI) {
            ax = *(int *) ax;
        } else if (op == SC) {
            ax = *(char *) *sp++ = ax;
        } else if (op == SI) {
            *(int *) *sp++ = ax;
        } else if (op == PUSH) {
            *--sp = ax;
        } else if (op == JMP) {
            pc = (int *) *pc;
        } else if (op == JZ) {
            pc = ax ? pc + 1 : (int *) *pc;
        } else if (op == JNZ) {
            pc = ax ? (int *) *pc : pc + 1;
        } else if (op == CALL) {
            *--sp = (int) (pc + 1);
            pc = (int *) *pc;
        } else if (op == NVAR) {
            *--sp = (int) bp;
            bp = sp;
            sp = sp - *pc++;
        } else if (op == DARG) {
            sp = sp + *pc++;
        } else if (op == RET) {
            sp = bp;
            bp = (int *) *sp++;
            pc = (int *) *sp++;
        } else if (op == OR) ax = *sp++ | ax;
        else if (op == XOR) ax = *sp++ ^ ax;
        else if (op == AND) ax = *sp++ & ax;
        else if (op == EQ) ax = *sp++ == ax;
        else if (op == NE) ax = *sp++ != ax;
        else if (op == LT) ax = *sp++ < ax;
        else if (op == LE) ax = *sp++ <= ax;
        else if (op == GT) ax = *sp++ > ax;
        else if (op == GE) ax = *sp++ >= ax;
        else if (op == SHL) ax = *sp++ << ax;
        else if (op == SHR) ax = *sp++ >> ax;
        else if (op == ADD) ax = *sp++ + ax;
        else if (op == SUB) ax = *sp++ - ax;
        else if (op == MUL) ax = *sp++ * ax;
        else if (op == DIV) ax = *sp++ / ax;
        else if (op == MOD) ax = *sp++ % ax;

        else if (op == OPEN) {
            ax = open((char *) sp[1], sp[0]);
        } else if (op == CLOS) {
            ax = close(*sp);
        } else if (op == READ) {
            ax = read(sp[2], (char *) sp[1], *sp);
        } else if (op == PRTF) {
            tmp = sp + pc[1] - 1;
            ax = printf((char *) tmp[0], tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5]);
        } else if (op == MALC) {
            ax = (int) malloc(*sp);
        } else if (op == FREE) {
            free((void *) *sp);
        } else if (op == MSET) {
            ax = (int) memset((char *) sp[2], sp[1], *sp);
        } else if (op == MCMP) {
            ax = memcmp((char *) sp[2], (char *) sp[1], *sp);
        } else if (op == EXIT) {
            printf("exit(%d)\n", *sp);
            return *sp;
        } else {
            printf("unknown instruction: %d, cycle: %d\n", op, cycle);
            return -1;
        }
    }

}

int vm_test() {
    init_vm();
    init_reg();

    int header_nbyte = sizeof(int) * 2;
    int entry_index = ByteCode[0];
    int data_nbyte = ByteCode[1];
    int code_nbyte = nByteCodeFileLen - (header_nbyte + data_nbyte);

    int *data_start = ByteCode + (header_nbyte) / sizeof(int);
    int *code_start = ByteCode + (header_nbyte + data_nbyte) / sizeof(int);
    printf("ndata:%d\n",data_nbyte);
    printf("ncode:%d\n",code_nbyte);
    if (data_nbyte != 0) { //复制数据区
        memcpy(data, data_start, data_nbyte);
    }
    memcpy(code, code_start, code_nbyte); //复制代码区
    free(ByteCode);

    printf("entry:%d\n", entry_index);
    for (int i = 0; i < code_nbyte / sizeof(int); i++) {
        printf("%d 0x%08X\n", code[i], code[i]);
    }

    pc = &code[entry_index];
    run_vm(0, NULL);
}

int read_bytecode(const char *filename) {
    FILE *fp;
    fp = fopen(filename, "rb+");

    if (fp == NULL) {
        printf("ASM is not found!\n");
        return -1;
    }
    // 获取字节码文件大小(字节数)
    fseek(fp, 0, SEEK_END);
    nByteCodeFileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (nByteCodeFileLen > 0) {
        if (!(ByteCode = malloc(nByteCodeFileLen))) {
            printf("Can not malloc for ByteCode!\n");
            fclose(fp);
            return -1;
        }
    }
    fread(ByteCode, sizeof(int), nByteCodeFileLen / sizeof(int), fp);
    if (ferror(fp)) {
        printf("Error reading.\n");
        clearerr(fp);
        return -1;
    }
    fclose(fp);
//    for (int i = 0; i < 9; i++) {
//        printf("%d 0x%08X\n", ByteCode[i], ByteCode[i]);
//    }
    vm_test();
}

