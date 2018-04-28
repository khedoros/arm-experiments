#pragma once
#include<cstdint>
#include "arm-defs.h"

class Arm7tdmi {
    public:
    arm7tdmi();
    int run(uint64_t run_to);
    private:
    uint32_t decode();
    void execute();
    enum state {
        ARM,
        THUMB
    };

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
    bool decoded; //An instruction was previously decoded, and is ready to be executed
    uint32_t decoded_instr;
    uint32_t decoded_instr_type;
};
