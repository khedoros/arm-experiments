#include<algorithm>
#include<iostream>
#include<cassert>
#include "Arm7tdmi.h"
#include "gba_types.h"

using namespace std;

//Crap related to generating large numbers of template functions for my operations
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

//Actually generates arrays of function pointers
constexpr std::array alu_op_gen  =      Init_alu(std::make_index_sequence<256>());
constexpr std::array mult_op_gen =      Init_mult(std::make_index_sequence<16>());
constexpr std::array psr_op_gen =       Init_psr(std::make_index_sequence<8>());
constexpr std::array branch_op_gen =    Init_branch(std::make_index_sequence<2>());
constexpr std::array transfer_op_gen =  Init_transfer(std::make_index_sequence<128>());
constexpr std::array transfer2_op_gen = Init_transfer2(std::make_index_sequence<256>());
constexpr std::array blktrans_op_gen =  Init_blktrans(std::make_index_sequence<32>());

//alu fields
bool is_immed(uint32_t i) {
    return (((1<<9)&i)>0);
}
uint32_t opcode_num(uint32_t i) {
    return ((i>>5)&0x0f);
}
bool is_stat_set(uint32_t i) {
    return (((1<<4)&i)>0);
}
bool is_compare_inst(uint32_t i) {
    return (opcode_num(i)>=8 && opcode_num(i)<12 && is_stat_set(i));
}
uint32_t shift_tag_field(uint32_t i) {
    return (i&0x009);
}

//branch fields
bool is_link(uint32_t i) {
     return (((1<<8)&i)>0);
}

Arm7tdmi::Arm7tdmi(shared_ptr<Gba_memmap>& b) : bus(b), r(), cycle(0), fetched(false), decode_ready(false), decoded(false), execute_ready(false) {
    r[15].ureg = 0;
    for(auto & op: op_map) {
        op = &Arm7tdmi::op_noop;
    }
    //ARM instruction    1111 11111111 111111111111 1111 1111
    //op_map indices     0000 ba987654 XXXXXXXXXXXX 3210 XXXX = ba9876543210
    //alu_op_gen indices XXXX XX765432 XXXXXXXXXXXX X10X XXXX = 76543210
    //branch_op_gen inds XXXX XXX0XXXX XXXXXXXXXXXX XXXX XXXX = 0
    for(uint32_t i = 0x000; i < 0x3ff; i++) { //AND,EOR,SUB,RSB,ADD,ADC,SBC,RSC,TST,TEQ,CMP,CMN,ORR,MOV,BIC,MVN
        if(( (!is_immed(i)) && shift_tag_field(i)==9) || (is_compare_inst(i) && (!is_stat_set(i)))) {
            continue;
        }
        else {
            op_map[i] = alu_op_gen.at(((i>>1)&0xff));
        }
    }
    for(uint32_t i = 0xa00; i < 0xc00; i++) { //B,BL
        op_map[i] = branch_op_gen.at(is_link(i)?1:0);
    }
}

int Arm7tdmi::run(uint64_t run_to) {
    int status = 0;
    while(cycle < run_to && !status) {
        if(r.get_isa_state() == ARM) {
            status = runa(run_to);
        }
        else {
            status = runt(run_to);
        }
    }
    return 0;
}

int Arm7tdmi::runa(uint64_t run_to) {
    while(cycle < run_to && r.get_isa_state() == ARM) {
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
    while(cycle < run_to && r.get_isa_state() == THUMB) {
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
    //cout<<hex<<r[15].ureg<<":\t"<<inst.data.word<<" (fetch ARM)"<<endl;
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
        //cout<<hex<<"Run "<<short_form<<endl;
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

//4096-entry map of a 8+4-bit field extracted from opcodes to the operation that the field represents. This is actually defined in the object's constructor.
Arm7OpPtr Arm7tdmi::op_map[256 * 16] = {nullptr};

uint64_t Arm7tdmi::op_noop(uint32_t opcode) {
    cout<<hex<<r[15].ureg-8<<"\t "<<opcode<<" (nop, or unrecognized)"<<endl;
    return 0;
}

uint64_t Arm7tdmi::op_coproc(uint32_t opcode) {
    return 4;
}

uint64_t Arm7tdmi::op_swi(uint32_t opcode) {
    return 4;
}

uint64_t Arm7tdmi::op_undef(uint32_t opcode) {
    return 4;
}

uint64_t Arm7tdmi::op_bx(uint32_t opcode) {
    cout<<hex<<r[15].ureg-8<<"\t <op_bx> Opcode: "<<opcode<<"\n";
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
        r.set_isa_state(THUMB);
    }
    else {
        r[15].ureg = (r[f.reg].ureg & 0xfffffffc);
        r.set_isa_state(ARM);
    }
    return 3; //2S + 1N cycles
}

template<uint32_t I>
uint64_t Arm7tdmi::op_alu(uint32_t opcode) {
    cout<<hex<<r[15].ureg-8<<"\t <op_alu> Opcode: "<<opcode<<" Funct variant: "<<I<<"\n";
    union full_format {
        struct {
            unsigned operand2:12;
            unsigned rd:4;
            unsigned rn:4;
            unsigned set_status:1;
            unsigned operation:4;
            unsigned immediate:1;
            unsigned top_tag:2;
            unsigned condition:4;
        };
        uint32_t val;
    } f;

    union short_format {
        struct {
            unsigned reg_shift:1;
            unsigned shift_type:2;
            unsigned high_shift:1;
            unsigned set_status:1;
            unsigned operation:4;
            unsigned immediate:1;
            unsigned top_tag:2;
        };
        uint32_t val;
    } sf;

    f.val = opcode;
    sf.val = I;

    uint32_t operand2 = 0;
    uint32_t op2_carry = 0;

    if(sf.immediate) { //Immediate mode

    }
    else if(sf.reg_shift) { //register-shifted register mode

    }
    else { //immediate-shifted register mode

    }

    switch(sf.operation) {
        case 0x00: //AND
            break;
        case 0x01: //EOR
            break;
        case 0x02: //SUB
            break;
        case 0x03: //RSB
            break;
        case 0x04: //ADD
            break;
        case 0x05: //ADC
            break;
        case 0x06: //SBC
            break;
        case 0x07: //RSC
            break;
        case 0x08: //TST
            break;
        case 0x09: //TEQ
            break;
        case 0x0A: //CMP
            break;
        case 0x0B: //CMN
            break;
        case 0x0C: //ORR
            break;
        case 0x0D: //MOV
            break;
        case 0x0E: //BIC
            break;
        case 0x0F: //MVN
            break;
    }

    if(sf.set_status) {

    }

    return 4;
}

template<uint32_t I>
uint64_t Arm7tdmi::op_mult(uint32_t opcode) {
    cout<<hex<<r[15].ureg-8<<"\t <op_mult> Opcode: "<<opcode<<" Funct variant: "<<I<<"\n";
    return 4;
}

template<uint32_t I>
uint64_t Arm7tdmi::op_psr(uint32_t opcode) {
    cout<<hex<<r[15].ureg-8<<"\t <op_psr> Opcode: "<<opcode<<" Funct variant: "<<I<<"\n";
    return 4;
}

template<uint32_t I>
uint64_t Arm7tdmi::op_branch(uint32_t opcode) {
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

    offset |= ((offset & (1<<25)) * 0x7f); //Check 26th bit, fill in the 6 bits above it if it's set

    std::string opname;
    if(I == 0) opname = "b";
    else if(I==1) opname = "bl";
    cout<<hex<<r[15].ureg-8<<"\t <op_branch> "<<opcode<<" (ARM)  "<<opname<<"  "<<r[15].ureg + offset + 4<<endl;

    if(r.get_isa_state() == THUMB) {
        if(I == 1) { //BL
            r[14].ureg = r[15].ureg - 2;
        }
        r[15].ureg += offset;
    }
    else {
        if(I == 1) { //BL
            r[14].ureg = r[15].ureg - 4;
        }
        r[15].ureg += offset;
    }
    return 3; //2S + 1N cycles
}

template<uint32_t I>
uint64_t Arm7tdmi::op_transfer(uint32_t opcode) {
    cout<<hex<<r[15].ureg-8<<"\t<op_transfer> Opcode: "<<opcode<<" Funct variant: "<<I<<"\n";
    return 4;
}

template<uint32_t I>
uint64_t Arm7tdmi::op_transfer2(uint32_t opcode) {
    cout<<hex<<r[15].ureg-8<<"\t<op_transfer> Opcode: "<<opcode<<" Funct variant: "<<I<<"\n";
    return 4;
}

template<uint32_t I>
uint64_t Arm7tdmi::op_blktrans(uint32_t opcode) {
    cout<<hex<<r[15].ureg-8<<"\t<op_blktrans> Opcode: "<<opcode<<" Funct variant: "<<I<<"\n";
    return 4;
}

Arm7tdmi::regbank::regbank() : priv_mode(Arm7tdmi::SYS), isa_state(Arm7tdmi::ARM) {
    for(int i=0;i<7;i++) {
        cur_reg[i] = &r[i+8];
    }
    cur_psr = &cpsr;
}

Arm7tdmi::reg& Arm7tdmi::regbank::operator[](uint32_t index) {
    assert(index >= 0 && index <= 15);
    if(index < 8 || index == 15) {
        return r[index];
    }
    else if(index < 15) {
        return *(cur_reg[index - 8]);
    }

    throw std::string("Dude, where's my car??");
}

//TODO: ALL of these need to have the right register-banking behavior implemented
Arm7tdmi::state Arm7tdmi::regbank::get_isa_state() {return isa_state;}
Arm7tdmi::mode Arm7tdmi::regbank::get_priv_mode() {return priv_mode;}
bool Arm7tdmi::regbank::get_fiq_disable() {return cpsr.fiq_disable;}
bool Arm7tdmi::regbank::get_irq_disable() {return cpsr.irq_disable;}
bool Arm7tdmi::regbank::get_overflow() {return cpsr.overflow;}
bool Arm7tdmi::regbank::get_carry() {return cpsr.carry;}
bool Arm7tdmi::regbank::get_zero() {return cpsr.zero;}
bool Arm7tdmi::regbank::get_sign() {return cpsr.sign;}

void Arm7tdmi::regbank::set_isa_state(state s) {isa_state = s;}
void Arm7tdmi::regbank::set_priv_mode(mode m) {priv_mode = m;}
void Arm7tdmi::regbank::set_fiq_disable(bool f) {cpsr.fiq_disable = f;}
void Arm7tdmi::regbank::set_irq_disable(bool i) {cpsr.irq_disable = i;}
void Arm7tdmi::regbank::set_overflow(bool o) {cpsr.overflow = o;}
void Arm7tdmi::regbank::set_carry(bool c) {cpsr.carry = c;}
void Arm7tdmi::regbank::set_zero(bool z) {cpsr.zero = z;}
void Arm7tdmi::regbank::set_sign(bool s) {cpsr.sign = s;}

