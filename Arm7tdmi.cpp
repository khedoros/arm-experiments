#include<algorithm>
#include<iostream>
#include "Arm7tdmi.h"
#include "gba_types.h"

using namespace std;

Arm7tdmi::Arm7tdmi(shared_ptr<Gba_memmap>& b) : bus(b) {

}

int Arm7tdmi::run(uint64_t run_to) {
    int status = 0;
    while(cycle < run_to && !status) {
        if(s == ARM) {
            status = runa(run_to);
        }
        else {
            status = runt(run_to);
        }
    }
    return 0;
}

int Arm7tdmi::runa(uint64_t run_to) {
    while(cycle < run_to && s == ARM) {
        if(decoded) {
            to_execute = decoded_instr;
            to_execute_type = decoded_instr_type;
            execute_ready = true;
        }
        if(fetched) {
            to_decode = fetched_instr;
            decode_ready = true;
        }
        uint64_t cyclef = fetcha();
        uint64_t cycled = decodea();
        uint64_t cyclee = executea();
        cycle += max(max(cyclef, cycled), cyclee);
        r[15].ureg += 4;
    }
}

int Arm7tdmi::runt(uint64_t run_to) {
    while(cycle < run_to && s == THUMB) {
        if(decoded) {
            to_execute = decoded_instr;
            to_execute_type = decoded_instr_type;
            execute_ready = true;
        }
        if(fetched) {
            to_decode = fetched_instr;
            decode_ready = true;
        }
        uint64_t cyclef = fetcht();
        uint64_t cycled = decodet();
        uint64_t cyclee = executet();
        cycle += max(max(cyclef, cycled), cyclee);
        r[15].ureg += 4;
    }
}

uint64_t Arm7tdmi::fetcha() {
    read_response inst = bus->read32(r[15].ureg, cycle);
    cout<<hex<<r[15].ureg<<":\t"<<inst.data.word<<endl;
    fetched = true;
    fetched_instr = inst.data.word;
    return inst.time;
}

uint64_t Arm7tdmi::decodea() {
    if(!decode_ready) {
        return 0;
    }
    //TODO: Actually decode the instruction
    return 1;
}

uint64_t Arm7tdmi::executea() {
    if(!execute_ready) {
        return 0;
    }
    //TODO: Actually do the work
    return 1;
}
uint64_t Arm7tdmi::fetcht() {
    read_response inst = bus->read16(r[15].ureg, cycle);
    fetched = true;
    fetched_instr = inst.data.hw[0];
    return inst.time;
}

uint64_t Arm7tdmi::decodet() {
    if(!decode_ready) {
        return 0;
    }
    //TODO: Actually decode
    return 1;
}

uint64_t Arm7tdmi::executet() {
    if(!execute_ready) {
        return 0;
    }
    //TODO: Execute
    return 1;
}

void Arm7tdmi::flush_pipeline() {
    fetched = false;
    decode_ready = false;
    decoded = false;
    execute_ready = false;
}

uint64_t Arm7tdmi::op_noop(uint32_t opcode) {
    return 1;
}

uint64_t Arm7tdmi::op_b(uint32_t opcode) {
    union format {
        struct { 
            unsigned offset:24;
            unsigned identifier:4;
            unsigned condition:4;
        };
        uint32_t val;
    } f;
    f.val = opcode;
    flush_pipeline();
    int32_t offset = f.offset<<2;
    offset |= ((offset & (1<<25)) * 0x7f); //Check 26th bit, fill in the 6 bits above it if it's set
    if(s == THUMB) {
        r[15].ureg += offset;
    }
    else {
        r[15].ureg += offset;
    }
    return 3; //2S + 1N cycles
}

uint64_t Arm7tdmi::op_bl(uint32_t opcode) {
    union format {
        struct { 
            unsigned offset:24;
            unsigned identifier:4;
            unsigned condition:4;
        };
        uint32_t val;
    } f;
    f.val = opcode;
    flush_pipeline();
    int32_t offset = f.offset<<2;
    offset |= ((offset & (1<<25)) * 0x7f); //Check 26th bit, fill in the 6 bits above it if it's set
    if(s == THUMB) {
        r[14].ureg = r[15].ureg - 2;
        r[15].ureg += offset;
    }
    else {
        r[14].ureg = r[15].ureg - 4;
        r[15].ureg += offset;
    }
    return 3; //2S + 1N cycles
}

uint64_t Arm7tdmi::op_bx(uint32_t opcode) {
    union format {
        struct { 
            unsigned reg:4;
            unsigned identifier:24;
            unsigned condition:4;
        };
        uint32_t val;
    } f;
    f.val = opcode;
    flush_pipeline();
    if(r[f.reg].ureg & 1) {
        r[15].ureg = (r[f.reg].ureg & 0xfffffffe);
        s = THUMB;
    }
    else {
        r[15].ureg = (r[f.reg].ureg & 0xfffffffc);
        s = ARM;
    }
    return 3; //2S + 1N cycles
}

const uint32_t Arm7tdmi::inst_mask[] = {
    0xfff, //BX
    0xf00, //B
    0xf00, //BL
    0x000  //Unrecognized/Nop
};

const uint32_t Arm7tdmi::inst_mask_match[] = {
    0x121, //BX
    0xa00, //B
    0xb00, //BL
    0x000  //Unrecognized/Nop
};

const Arm7OpPtr Arm7tdmi::inst_mask_ops[] = {
    &Arm7tdmi::op_bx,
    &Arm7tdmi::op_b,
    &Arm7tdmi::op_bl,
    &Arm7tdmi::op_noop
};
