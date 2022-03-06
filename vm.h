//
// Created by LXY on 2022/3/1.
//

#ifndef C_COMPILER_VM_H
#define C_COMPILER_VM_H



#define SEG_SIZE (1024*1024)

enum {
    IMM = 0,// 加载一个立即数到 ax
    LEA,    // 加载一个有效地址到 ax
    JMP,    // 无条件跳转
    JZ,     // ax 为 0 时跳转
    JNZ,    // ax 不为 0 时跳转
    CALL,   // 函数调用，保存当前 pc 到栈然后跳转
    NVAR,   // 为函数局部变量创建新的栈帧
    DARG,   // 释放函数变量栈帧
    RET,    // 从函数返回
    LI,     // 将对应地址中的整数载入 ax 中，ax 中存放地址
    LC,     // 将对应地址中的字符载入 ax 中，ax 中存放地址
    SI,     // 将 ax 中的数据作为整数存放入地址中，栈顶存放地址
    SC,     // 将 ax 中的数据作为字符存放入地址中，栈顶存放地址
    PUSH,   // 保存 ax 到栈

    // 运算指令
    // 全为二元运算，第一个参数在栈顶，第二个参数在 ax
    OR,     // 或
    XOR,    // 异或
    AND,    // 和
    EQ,     // 相等
    NE,     // 不等
    LT,     // 小于
    GT,     // 大于
    LE,     // 小于等于
    GE,     // 大于等于
    SHL,    // 左移
    SHR,    // 右移
    ADD,    // 加
    SUB,    // 减
    MUL,    // 乘
    DIV,    // 除
    MOD,    // 取余

    //本地原生指令
    OPEN,   // 打开文件
    READ,   // 读取文件到内存
    CLOS,   // 关闭文件
    PRTF,   // 输出到标准输出
    MALC,   // 申请堆内存
    FREE,   // 释放堆内存
    MSET,   // memset
    MCMP,   // memcmp
    EXIT    // 结束程序
};

int init_vm();
int init_reg();
int run_vm(int argc, char **argv);

int vm_test();
int read_bytecode(const char *filename);

#endif //C_COMPILER_VM_H
