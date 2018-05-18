#pragma once
#include<cstdint>

enum condition {
    EQ, //Z==1
    NE, //Z==0
    CS, //C==1
    CC, //C==0
    MI, //N==1
    PL, //N==0
    VS, //V==1
    VC, //V==0
    HI, //C==1 and Z==0
    LS, //C==0 or Z==1
    GE, //N==V
    LT, //N!=V
    GT, //Z==0 and N==V
    LE, //Z==1 or N!=V
    AL, //Always
    NV  //Never
};

extern const char cond_str[][3];
extern const char alu_ops[][4];
extern const char shift_type[][4];
extern const uint32_t instr_mask[];
extern const uint32_t instr_match[];
extern const char instr_names[][40];
extern const uint16_t instr_mask_t[];
extern const uint16_t instr_match_t[];
extern const char instr_names_t[][40];

union instr {
    struct {
        unsigned reg3:4;
        unsigned field7:4;
        unsigned reg11:4;
        unsigned reg15:4;
        unsigned reg19:4;
        unsigned op:8;
        unsigned cond:4;
    };
    uint32_t val:32;
};

union instr_t {
    struct {
        unsigned reg2:3;
        unsigned reg5:3;
        unsigned reg8:3;
        unsigned top7:7;
    };
    struct {
        unsigned val7:8;
        unsigned top8:8;
    };
    uint16_t val:16;
};

struct status {
    unsigned mode:5;
    unsigned thumb:1;
    unsigned fiq_dis:1;
    unsigned irq_dis:1;
    unsigned reserved:20;
    unsigned v:1;
    unsigned c:1;
    unsigned z:1;
    unsigned n:1;
};

#define ALU_RN(x) r[((x>>16) & 0xf)].ureg
#define ALU_RD(x) r[((x>>12) & 0xf)].ureg
#define ALU_RM(x) r[(x & 0xf)].ureg
#define ALU_RS(x) r[((x>>8) & 0xf)].ureg
#define ALU_IMM(x) (x & 0xff)
#define ALU_IMM_ROT(x) ((x>>7) & 0x1e)
#define ALU_SHIFT_TYPE(x) ((x>>5) & 0x3)
#define ALU_SHIFT_AMNT(x) ((x>>7) & 0x1f)

#define BX_RN(x) r[(x & 0xf)].ureg

#define MUL_RD(x) r[((x>>16) & 0xf)].ureg
#define MUL_RN(x) r[((x>>12) & 0xf)].ureg
#define MUL_RS(x) r[((x>>8) & 0xf)].ureg
#define MUL_RM(x) r[(x & 0xf)].ureg
#define MUL_RDHI(x) r[((x>>16) & 0xf)].ureg
#define MUL_RDLO(x) r[((x>>12) & 0xf)].ureg


