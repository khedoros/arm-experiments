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
    "AL",
    "NV"
};

char alu_ops[][4] {
    "AND", "EOR", "SUB", "RSB",
    "ADD", "ADC", "SBC", "RSC",
    "TST", "TEQ", "CMP", "CMN",
    "ORR", "MOV", "BIC", "MVN"};


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
        union {
            struct { // 0
                unsigned operand2_0:12;
                unsigned rd_0:4;
                unsigned rn_0:4;
                unsigned s_0:1;
                unsigned opcode_0:4;
            } __attribute__((packed));
            struct { // 1
                unsigned rm_1:4;
                unsigned id3_1:4; //1001
                unsigned rs_1:4;
                unsigned rn_1:4;
                unsigned rd_1:4;
                unsigned s_1:1;
                unsigned a_1:1;
                unsigned id2_1:3; //000
            } __attribute__((packed));
            struct { // 2
                unsigned rm_2:4;
                unsigned id3_2:4; //1001
                unsigned rn_2:4;
                unsigned rd_lo_2:4;
                unsigned rd_hi_2:4;
                unsigned s_2:1;
                unsigned a_2:1;
                unsigned u_2:1;
                unsigned id2_2:2; //01
            } __attribute__((packed));
            struct { // 3
                unsigned rm_3:4;
                unsigned id4_3:8; //00001001
                unsigned rd_3:4;
                unsigned rn_3:4;
                unsigned id3_3:2; //00
                unsigned b_3:1;
                unsigned id2_3:2; //10
            } __attribute__((packed));
            struct { // 4
                unsigned rn_4:4;
                unsigned id2_4:21; //100101111111111110001
            } __attribute__((packed));
            struct { // 5
                unsigned rm_5:4;
                unsigned id4_5:1; //1
                unsigned h_5:1;
                unsigned s_5:1;
                unsigned id3_5:5; //00001
                unsigned rd_5:4;
                unsigned rn_5:4;
                unsigned l_5:1;
                unsigned w_5:1;
                unsigned id2_5:1; //0
                unsigned u_5:1;
                unsigned p_5:1;
            } __attribute__((packed));
            struct { // 6
                unsigned offset2_6:4;
                unsigned id4_6:1; //1
                unsigned h_6:1;
                unsigned s_6:1;
                unsigned id3_6:1; //1
                unsigned offset1_6:4;
                unsigned rd_6:4;
                unsigned rn_6:4;
                unsigned l_6:1;
                unsigned w_6:1;
                unsigned id2_6:1; //1
                unsigned u_6:1;
                unsigned p_6:1;
            } __attribute__((packed));
            struct { // 7
                unsigned offset_7:12;
                unsigned rd_7:4;
                unsigned rn_7:4;
                unsigned l_7:1;
                unsigned w_7:1;
                unsigned b_7:1;
                unsigned u_7:1;
                unsigned p_7:1;
            } __attribute__((packed));
            struct { // 8
                unsigned dont_care2_8:4;
                unsigned id2_8:1; //1
                unsigned dont_care1_8:20;
            } __attribute__((packed));
            struct { // 9
                unsigned reg_list_9:16;
                unsigned rn_9:4;
                unsigned l_9:1;
                unsigned w_9:1;
                unsigned s_9:1;
                unsigned u_9:1;
                unsigned p_9:1;
            } __attribute__((packed));
            struct { // a
                unsigned offset_a:24;
                unsigned l_a:1;
            } __attribute__((packed));
            struct { // b
                unsigned offset_b:8;
                unsigned CPnum_b:4;
                unsigned crd_b:4;
                unsigned rn_b:4;
                unsigned l_b:1;
                unsigned w_b:1;
                unsigned n_b:1;
                unsigned u_b:1;
                unsigned p_b:1;
            } __attribute__((packed));
            struct { // c
                unsigned CRm_c:4;
                unsigned id3_c:1; //0
                unsigned CP_c:3;
                unsigned CPnum_c:4;
                unsigned crd_c:4;
                unsigned crn_c:4;
                unsigned l_c:1;
                unsigned cp_op_c:3;
                unsigned id2_c:1; //0
            } __attribute__((packed));
            struct { // d
                unsigned CRm_d:4;
                unsigned id3_d:1; //0
                unsigned CP_d:3;
                unsigned CPnum_d:4;
                unsigned rd_d:4;
                unsigned crn_d:4;
                unsigned l_d:1;
                unsigned cp_op_d:3;
                unsigned id2_d:1; //0
            } __attribute__((packed));
            struct { // e
                unsigned dont_care:24;
                unsigned id2_e:1; //1
            } __attribute__((packed));
            unsigned blob:25;
        } __attribute__((packed));
        unsigned id:3;
        unsigned cond:4;
    } __attribute__((packed));
    uint32_t val:32;
} __attribute__((packed));
