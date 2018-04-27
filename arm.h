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

char cond_str[][3] {
    "EQ",
    "NE",
    "CS",
    "CC",
    "MI",
    "PL",
    "VS",
    "VC",
    "HI",
    "LS",
    "GE",
    "LT",
    "GT",
    "LE",
    "", //"AL", but always elided
    "NV"
};

char alu_ops[][4] {
    "AND", "EOR", "SUB", "RSB",
    "ADD", "ADC", "SBC", "RSC",
    "TST", "TEQ", "CMP", "CMN",
    "ORR", "MOV", "BIC", "MVN"};


uint32_t instr_mask[] = { 0x0FFFFFF0,
                          0x0FBF0FFF,
                          0x0FBFFFF0,
                          0x0FBFFFF0,
                          0x0FBFF000,
                          0x0E000010,
                          0x0E000090,
                          0x0E000000,
                          0x0FC000F0,
                          0x0F8000F0,
                          0x0E000000,
                          0x0E000010,
                          0x0FB00FF0,
                          0x0E400F90,
                          0x0E400090,
                          0x0E000000,
                          0x0E000000,
                          0x0F000000,
                          0x0F000010,
                          0x0E000000,
                          0x0F000010,
                          0x0E000010};

uint32_t instr_match[] = { 0x012FFF10,
                           0x010F0000,
                           0x0129F000,
                           0x0128F000,
                           0x0328F000,
                           0x00000000,
                           0x00000010,
                           0x02000000,
                           0x00000090,
                           0x00800090,
                           0x04000000,
                           0x06000000,
                           0x01000090,
                           0x00000090,
                           0x00400090,
                           0x08000000,
                           0x0A000000,
                           0x0F000000,
                           0x0E000000,
                           0x0C000000,
                           0x0E000010,
                           0x06000010};

char instr_names[][40] = { "BX                         ",
                           "MRS                        ",
                           "MSR(reg to PSR)            ",
                           "MSR(reg to PSR flags)      ",
                           "MSR(immediate to PSR flags)",
                           "ALU Ops (immed-reg shift)  ",
                           "ALU Ops (reg-reg shift)    ",
                           "ALU Ops (immed shift)      ",
                           "Multiply                   ",
                           "Multiply Long              ",
                           "LDR, STR (unsigned immed)  ",
                           "LDR, STR (immed-reg shift) ",
                           "SWP                        ",
                           "LDRH, STRH, LDRSB, LDRSH   ",
                           "LDRH, STRH, LDRSB, LDRSH   ",
                           "LDM, STM                   ",
                           "B/BL                       ",
                           "SWI                        ",
                           "CDP                        ",
                           "LDC/STC                    ",
                           "MRC, MCR                   ",
                           "Undefined                  "};

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

