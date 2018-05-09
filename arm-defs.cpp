#include "arm-defs.h"

const char cond_str[][3] = {
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

const char alu_ops[][4] = {
    "AND", "EOR", "SUB", "RSB",
    "ADD", "ADC", "SBC", "RSC",
    "TST", "TEQ", "CMP", "CMN",
    "ORR", "MOV", "BIC", "MVN"};

const char shift_type[][4] = {"LSL", "LSR", "ASR", "ROR"};

const uint32_t instr_mask[] = {
    0x0FFFFFF0,
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

const uint32_t instr_match[] = {
    0x012FFF10,
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

const char instr_names[][40] = {
    "BX                         ",
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


const uint16_t instr_mask_t[] = {
    0xF800,
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
    0xF000
};


const uint16_t instr_match_t[] = {
    0x1800,
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
    0xF000
};


const char instr_names_t[][40] = {
    "Add/Subtract reg/immed",
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
    "Long branch+link"
};

