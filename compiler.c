//
// Created by LXY on 2022/3/6.
//
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "io.h"

#include "compiler.h"
#include "vm.h"

int nSrcFileLen;
char *src;
char *src_bk;
int nsrc = 0;
int line = 0;

int token;
int token_val;


int *symbol_table;
int *current_id;
int *main_sym;

char *file_data;        //字节码中的.data段
char *file_data_bk;     //字节码中的.data段，总是指向.data段首
int fdata_offest = 0;

int init_compiler() {
    if (!(symbol_table = malloc(SYMBOL_TABLE_SIZE))) {
        printf("Can not malloc for symbol table!\n");
        return -1;
    }
    if (!(file_data = malloc(SEG_SIZE))) {
        printf("Can not malloc for .data in file!\n");
        return -1;
    }
    memset(symbol_table, 0, SYMBOL_TABLE_SIZE);
    memset(file_data, 0, SEG_SIZE);
    file_data_bk = file_data;
    return 0;
}

// 词法分析,向下解析一个词
void tokenize() {
    printf("Start:\n");
    char *last_pos;
    int hash;

    while (token = *src) {
        printf("%c", token);
        src++;
        // 跳过换行
        if (token == '\n') {
            line++;
            //printf("[\\n!]\n");
        } else if (token == '#') { //跳过宏定义
            //printf("[#]\n");
            while (*src != 0 && *src != '\n') {
                src++;
            }
            //printf("[-#]\n");
        } else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_')) { // 解析标识符
            printf("[token]\n");
            last_pos = src - 1;// 记录标识符首个字符位置
            hash = token;
            // 计算标识符的哈希值
            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src == '_')) {
                hash = hash * 147 + *src;
                src++;
            }
            // 查找是否有已存在的标识符
            current_id = symbol_table;
            while (current_id[Token] != 0) {
                if (current_id[Hash] == hash && memcmp((char *) current_id[Name], last_pos, src - last_pos) == 0) {
                    token = current_id[Token];

                    printf("name:");
                    int n = src - last_pos;
                    for (int i = 0; i < n; i++) {
                        printf("%c", ((char *) current_id[Name])[i]);
                    }
                    printf("\nSame\n");
                    printf("token:%d\n",current_id[Token]);
                    printf("type:%d\n",current_id[Type]);
                    printf("hash:%d\n\n", current_id[Hash]);

                    return;
                }
                current_id = current_id + Offset;
            }
            // 存储新的标识符数据
            current_id[Name] = (int) last_pos;
            current_id[Hash] = hash;
            token = current_id[Token] = Id;

            printf("name:");
            int n = src - last_pos;
            for (int i = 0; i < n; i++) {
                printf("%c", ((char *) current_id[Name])[i]);
            }
            printf("\ntoken:%d\n",current_id[Token]);
            printf("type:%d\n",current_id[Type]);
            printf("hash:%d\n\n", current_id[Hash]);

            return;
        } else if (token >= '0' && token <= '9') { //解析数字
            token_val = token - '0';
            if (token_val > 0) { //十进制，以1-9开头
                while (*src >= '0' && *src <= '9') {
                    token_val = token_val * 10 + *src++ - '0';
                }
            } else {//以0开头
                if (*src == 'x' || *src == 'X') { //HEX
                    while ((token >= '0' && token <= '9') || (token >= 'a' && token <= 'f') ||
                           (token >= 'A' && token <= 'F')) {
                        token_val = token_val * 16 + (token & 15) + (token >= 'A' ? 9 : 0);
                        token = *++src;
                    }
                } else { //OCT
                    while (*src >= '0' && *src <= '7') {
                        token_val = token_val * 8 + *src++ - '0';
                    }
                }
            }
            token = Num;
            return;
        } else if (token == '"' || token == '\'') { // 处理字符串或字符
            int first_pos = fdata_offest;     // 保存字符串起始偏移
            while (*src != 0 && *src != token) {
                token_val = *src++;
                if (token_val == '\\') {
                    token_val = *src++;
                    if (token_val == 'n') {
                        token_val = '\n';
                    }
                }
                if (token == '"') { // 是字符串
                    *file_data++ = token_val;
                    fdata_offest++;
                }
            }
            if (token == '"') {   // 是字符串
                *file_data++ = 0;
                fdata_offest++;
                token_val = (int) first_pos; // token_val设为字符串首在.data区的偏移
            } else {      //是单个字符
                token = Num;
            }
            src++;
            printf("strlen:%d\n", fdata_offest);
            printf("val:%d\n", token_val);
            printf("type:%d\n", token);
            for (int i = 0; i < 20; i++) {
                printf("%d ", file_data_bk[i]);
            }
            printf("\n");
            return;
        } else if (token == '/') {
            if (*src == '/') {
                while (*src != 0 && *src != '\n') {
                    src++;
                }
            } else {
                token = Div;
                return;
            }
        } else if (token == '=') {
            if (*src == '=') {
                src++;
                token = Eq;
            } else {
                token = Assign;
            }
            return;
        } else if (token == '+') {
            if (*src == '+') {
                src++;
                token = Inc;
            } else
                token = Add;
            return;
        } else if (token == '-') {
            if (*src == '-') {
                src++;
                token = Dec;
            } else
                token = Sub;
            return;
        } else if (token == '!') {
            if (*src == '=') {
                src++;
                token = Ne;
            }
            return;
        } else if (token == '<') {
            if (*src == '=') {
                src++;
                token = Le;
            } else if (*src == '<') {
                src++;
                token = Shl;
            } else
                token = Lt;
            return;
        } else if (token == '>') {
            if (*src == '=') {
                src++;
                token = Ge;
            } else if (*src == '>') {
                src++;
                token = Shr;
            } else
                token = Gt;
            return;
        } else if (token == '|') {
            if (*src == '|') {
                src++;
                token = Lor;
            } else
                token = Or;
            return;
        } else if (token == '&') {
            if (*src == '&') {
                src++;
                token = Land;
            } else
                token = And;
            return;
        } else if (token == '^') {
            token = Xor;
            return;
        } else if (token == '%') {
            token = Mod;
            return;
        } else if (token == '*') {
            token = Mul;
            return;
        } else if (token == '[') {
            token = Brak;
            return;
        } else if (token == '?') {
            token = Cond;
            return;
        } else if (token == '~' || token == ';' || token == '{' ||
                   token == '}' || token == '(' || token == ')' ||
                   token == ']' || token == ',' || token == ':') {
            return;
        }
    }
}
void keyword(){
    int i = 0;
    src = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";
    i = Char;
    while(i <= While){
        tokenize();
        current_id[Token] = i++;
    }
    i = OPEN;
    while(i <= EXIT){
        tokenize();
        current_id[Class] = Sys;
        current_id[Type] = INT;
        current_id[Value] = i++;
    }
    tokenize();
    current_id[Token] = Char; //处理空类型
    tokenize();
    main_sym = current_id;
    src = src_bk;
}

void d_printSymbolTable() {
    int i = 0;
    while (symbol_table[i] != 0) {
        printf("token:%d\n", symbol_table[i + Token]);
        printf("name:%d\n", symbol_table[i + Name]);
        printf("hash:%d\n\n", symbol_table[i + Hash]);

        i = i + Offset;
    }
}

int read_src(const char *filename) {
    FILE *fp;
    fp = fopen(filename, "rb+");

    if (fp == NULL) {
        printf("C source file is not found!\n");
        return -1;
    }
    // 获取C源文件大小(字节数)
    fseek(fp, 0, SEEK_END);
    nSrcFileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    printf("%d\n", nSrcFileLen);

    if (nSrcFileLen > 0) {
        if (!(src = malloc(nSrcFileLen + 1))) {
            printf("Can not malloc for Source File!\n");
            fclose(fp);
            return -1;
        }
    }
    src_bk = src;
    memset(src, 0, nSrcFileLen + 1);
    fread(src, sizeof(char), nSrcFileLen / sizeof(char), fp);
    if (ferror(fp)) {
        printf("Error reading.\n");
        clearerr(fp);
        return -1;
    }
    fclose(fp);

    for (int i = 0; i < nSrcFileLen + 1; i++) {
        printf("%c", src[i]);
    }
    printf("\n");
//    for (int i = 0; i < nSrcFileLen + 1; i++) {
//        printf("%02X ", src[i]);
//    }
//    printf("\n");
}