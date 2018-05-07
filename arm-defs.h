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

char shift_type[][4] {"LSL", "LSR", "ASR", "ROR"};

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

uint16_t instr_mask_t[] = { 0xF800,
                            0xE000,
                            0xE000,
                            0xFC00,
                            0xFC00,
                            0xF800,
                            0xF200,
                            0xF200,
                            0xE000,
                            0xF000,
                            0xF000,
                            0xF000,
                            0xFF00,
                            0xF600,
                            0xF000,
                            0xFF00,
                            0xF000,
                            0xF800,
                            0xF000};

uint16_t instr_match_t[] = { 0x1800,
                             0x0000,
                             0x2000,
                             0x4000,
                             0x4400,
                             0x4800,
                             0x5000,
                             0x5200,
                             0x6000,
                             0x8000,
                             0x9000,
                             0xA000,
                             0xB000,
                             0xB400,
                             0xC000,
                             0xDF00,
                             0xD000,
                             0xE000,
                             0xF000};

char instr_names_t[][40] = { "Add/Subtract reg/immed",
                             "Mov shifted register",
                             "MOV, ADD, SUB, CMP",
                             "reg-reg ALU operations",
                             "ADD, CMP, MOV, BX (hi+low regs)",
                             "LDR immediate",
                             "LDR,STR reg",
                             "LDR,STR sign-extnd byte/half-word reg",
                             "LDR,STR immed offset",
                             "LDR,STR half-word",
                             "LDR,STR SP-relative",
                             "Load Address",
                             "Add offset to SP",
                             "Push/pop registers",
                             "Multiple LDR,STR",
                             "Software Interrupt",
                             "Conditional branch",
                             "Unconditional branch",
                             "Long branch+link"};

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

union word {
    uint32_t word;
    uint16_t hw[2];
    uint8_t byte[4];
};

struct read_response {
    word data; //data that was read
    uint64_t time; //cycles it took to read
};
