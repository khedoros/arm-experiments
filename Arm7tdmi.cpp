#include<algorithm>
#include<iostream>
#include "Arm7tdmi.h"
#include "gba_types.h"

using namespace std;

Arm7tdmi::Arm7tdmi(shared_ptr<Gba_memmap>& b) : bus(b), m(Arm7tdmi::SYS), s(Arm7tdmi::ARM), cycle(0), fetched(false), decode_ready(false), decoded(false), execute_ready(false) {
    cout<<"Construct ARM at "<<hex<<this<<"\n";
    r[15].ureg = 0;
    for(auto & op: op_map) {
        op = &Arm7tdmi::op_noop;
    }
    /*
    for(int inst_ptr = 0; inst_ptr < 4096; inst_ptr++) {
        for(int candidate = 0; candidate < op_count; candidate++) {
            if((inst_ptr & inst_mask[candidate]) == inst_mask_match[candidate]) {
                op_map[inst_ptr] = inst_mask_ops[candidate];
                break;
            }
        }
    }
    */
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
    return 0;
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
        r[15].ureg += 2;
    }
    return 0;
}

uint64_t Arm7tdmi::fetcha() {
    read_response inst = bus->read32(r[15].ureg, cycle);
    cout<<hex<<r[15].ureg<<":\t"<<inst.data.word<<" (fetch ARM)"<<endl;
    fetched = true;
    fetched_instr = inst.data.word;
    return inst.time;
}

uint64_t Arm7tdmi::decodea() {
    if(!decode_ready) {
        return 0;
    }
    //TODO: Actually decode the instruction
    decoded_instr = to_decode;
    decoded = true;
    return 1;
}

uint64_t Arm7tdmi::executea() {
    if(!execute_ready) {
        return 0;
    }

    //TODO: Make sure condition is correct to execute the instruction
    if(/*condition stuff*/true) {
        uint32_t short_form = ((to_execute & 0x0ff00000)>>16) | ((to_execute & 0x000000f0)>>4);
        cout<<hex<<"Run "<<short_form<<endl;
        return CALL_MEMBER_FN(this, op_map[short_form])(to_execute);
    }
    return 0;
}
uint64_t Arm7tdmi::fetcht() {
    read_response inst = bus->read16(r[15].ureg, cycle);
    //cout<<hex<<r[15].ureg<<":\t"<<inst.data.word<<" (fetch Thumb)"<<endl;
    fetched = true;
    fetched_instr = inst.data.hw[0];
    return inst.time;
}

uint64_t Arm7tdmi::decodet() {
    if(!decode_ready) {
        return 0;
    }
    //TODO: Actually decode
    decoded_instr = to_decode;
    decoded = true;
    return 1;
}

uint64_t Arm7tdmi::executet() {
    if(!execute_ready) {
        return 0;
    }
    cout<<"Haha, thumb mode"<<endl;
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
    cout<<hex<<r[15].ureg-8<<":\t"<<opcode<<" (nop, or unrecognized)"<<endl;
    return 1;
}

template<unsigned long... I>
constexpr inline auto Init_alu(std::index_sequence<I...>) {
    return std::array{ &Arm7tdmi::op_alu<I>... };
}

template<unsigned long... I>
constexpr inline auto Init_mult(std::index_sequence<I...>) {
    return std::array{ &Arm7tdmi::op_mult<I>... };
}

template<unsigned long... I>
constexpr inline auto Init_psr(std::index_sequence<I...>) {
    return std::array{ &Arm7tdmi::op_psr<I>... };
}

template<unsigned long... I>
constexpr inline auto Init_branch(std::index_sequence<I...>) {
    return std::array{ &Arm7tdmi::op_branch<I>... };
}

template<unsigned long... I>
constexpr inline auto Init_transfer(std::index_sequence<I...>) {
    return std::array{ &Arm7tdmi::op_transfer<I>... };
}

template<unsigned long... I>
constexpr inline auto Init_transfer2(std::index_sequence<I...>) {
    return std::array{ &Arm7tdmi::op_transfer2<I>... };
}

template<unsigned long... I>
constexpr inline auto Init_blktrans(std::index_sequence<I...>) {
    return std::array{ &Arm7tdmi::op_blktrans<I>... };
}

constexpr std::array alu_op_gen  =      Init_alu(std::make_index_sequence<256>());
constexpr std::array mult_op_gen =      Init_mult(std::make_index_sequence<16>());
constexpr std::array psr_op_gen =       Init_psr(std::make_index_sequence<8>());
constexpr std::array branch_op_gen =    Init_branch(std::make_index_sequence<2>());
constexpr std::array transfer_op_gen =  Init_transfer(std::make_index_sequence<128>());
constexpr std::array transfer2_op_gen = Init_transfer2(std::make_index_sequence<256>());
constexpr std::array blktrans_op_gen =  Init_blktrans(std::make_index_sequence<32>());

Arm7OpPtr Arm7tdmi::op_map[256 * 16] = {nullptr};

/*
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
    int32_t offset = (f.offset<<2) - 4;
    cout<<hex<<r[15].ureg-8<<":\t"<<opcode<<" (ARM)  b "<<offset+12<<endl;
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
    int32_t offset = (f.offset<<2) - 4;
    cout<<hex<<r[15].ureg-8<<":\t"<<opcode<<" (ARM)  bl "<<offset+12<<endl;
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

uint64_t Arm7tdmi::op_mrs(uint32_t opcode) { //copies status to register
    return 1; //1S cycles
}

uint64_t Arm7tdmi::op_msr_reg(uint32_t opcode) { //applies register to status, or to status flags only
    return 1; //1S cycles
}

uint64_t Arm7tdmi::op_msr_imm(uint32_t opcode) { //applies immediate to status flags only
    return 1; //1S cycles
}

*/

