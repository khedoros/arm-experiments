#pragma once
#include<cstdint>
#include<memory>
#include "arm-defs.h"
#include "Gba_memmap.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object)->*(ptrToMember))
//#define CALL_MEMBER_FN(object,ptrToMember)  ((object)->(ptrToMember))

class Arm7tdmi;
typedef uint64_t (Arm7tdmi::*Arm7OpPtr)(uint32_t);

class Arm7tdmi {
public:
    Arm7tdmi(std::shared_ptr<Gba_memmap>& b);
    int run(uint64_t run_to);
    int runa(uint64_t run_to);
    int runt(uint64_t run_to);
private:
    uint64_t fetcha();
    uint64_t fetcht();
    uint64_t decodea();
    uint64_t decodet();
    uint64_t executea();
    uint64_t executet();

    Arm7OpPtr op_map[256 * 16];

    void flush_pipeline();

    uint64_t op_noop(uint32_t);
    uint64_t op_b(uint32_t);
    uint64_t op_bl(uint32_t);
    uint64_t op_bx(uint32_t);

    static const uint32_t inst_mask[];
    static const uint32_t inst_mask_match[];
    static const Arm7OpPtr inst_mask_ops[];
    static const int op_count;
    enum state {
        ARM,
        THUMB
    };

    std::shared_ptr<Gba_memmap> bus;

    enum mode {
        OLD_USR = 0,
        OLD_FIQ,
        OLD_IRQ,
        OLD_SVC,
        USR = 0x10,
        FIQ,
        IRQ,
        SVC,
        ABT = 0x17,
        UND = 0x1b,
        SYS = 0x1f,
        INVALID
    };

    union reg {
        int32_t  sreg;
        uint32_t ureg;
    };

    union mreg {
        int32_t sreg;
        uint32_t ureg;
        struct {
            unsigned mode:5;
            unsigned t:1;
            unsigned fiq_disable:1;
            unsigned irq_disable:1;
            unsigned reserved:20;
            unsigned overflow:1;
            unsigned carry:1;
            unsigned zero:1;
            unsigned sign:1;
        };
    };
    reg r[16]; //regular user regs
    reg r_fiq[7]; //r8_fiq..r14_fiq
    reg r_svc[2]; //r13_svc+r14_svc
    reg r_abt[2]; //r13_abt+r14_abt
    reg r_irq[2]; //r13_abt+r14_abt
    reg r_und[2]; //r13_abt+r14_abt
    mreg cspr;
    mreg spsr[5]; //spsr_fiq, spsr_svc, spsr_abt, spsr_irq, spsr_und

    mode m;
    state s;

    uint64_t cycle;
    bool fetched; //An instruction was previously fetched, and is ready to be decoded
    uint32_t fetched_instr;
    bool decode_ready; //An instruction was previously fetched, and is ready to be decoded
    uint32_t to_decode; //An instruction ready to be decoded
    bool decoded; //An instruction was previously decoded, and is ready to be executed
    uint32_t decoded_instr; //An instruction ready to be executed
    uint32_t decoded_instr_type; //The type of that instruction
    uint32_t to_execute;
    uint32_t to_execute_type;
    bool execute_ready;

};
