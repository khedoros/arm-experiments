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
