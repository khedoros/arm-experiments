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

union instr {
    struct {
        union {
            struct { // 0
                unsigned operand2_0:12;
                unsigned rd_0:4;
                unsigned rn_0:4;
                unsigned s_0:1;
                unsigned opcode_0:4;
            };
            struct { // 1
                unsigned rm_1:4;
                unsigned id3_1:4; //1001
                unsigned rs_1:4;
                unsigned rn_1:4;
                unsigned rd_1:4;
                unsigned s_1:1;
                unsigned a_1:1;
                unsigned id2_1:3; //000
            };
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
            };
            struct { // 3
                unsigned rm_3:4;
                unsigned id4_3:8; //00001001
                unsigned rd_3:4;
                unsigned rn_3:4;
                unsigned id3_3:2; //00
                unsigned b_3:1;
                unsigned id2_3:2; //10
            };
            struct { // 4
                unsigned rn_4:4;
                unsigned id2_4:21; //100101111111111110001
            };
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
            };
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
            };
            struct { // 7
                unsigned offset_7:12;
                unsigned rd_7:4;
                unsigned rn_7:4;
                unsigned l_7:1;
                unsigned w_7:1;
                unsigned b_7:1;
                unsigned u_7:1;
                unsigned p_7:1;
            };
            struct { // 8
                unsigned dont_care2_8:4;
                unsigned id2_8:1; //1
                unsigned dont_care1_8:20;
            };
            struct { // 9
                unsigned reg_list_9:16;
                unsigned rn_9:4;
                unsigned l_9:1;
                unsigned w_9:1;
                unsigned s_9:1;
                unsigned u_9:1;
                unsigned p_9:1;
            };
            struct { // a
                unsigned offset_a:24;
                unsigned l_a:1;
            };
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
            };
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
            };
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
            };
            struct { // e
                unsigned dont_care:24;
                unsigned id2_e:1; //1
            };
            unsigned blob:25;
        };
        unsigned id:3;
        unsigned cond:4;
    };
    uint32_t val:32;
};
