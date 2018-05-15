#include<algorithm>
#include<iostream>
#include "Arm7tdmi.h"
#include "gba_types.h"

using namespace std;

Arm7tdmi::Arm7tdmi(shared_ptr<Gba_memmap>& b) : bus(b) {
    for(int inst_ptr = 0; inst_ptr < 4096; inst_ptr++) {
        for(int candidate = 0; candidate < op_count; candidate++) {
            if((inst_ptr & inst_mask[candidate]) == inst_mask_match[candidate]) {
                op_map[inst_ptr] = inst_mask_ops[candidate];
                break;
            }
        }
    }
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

uint64_t Arm7tdmi::op_and_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_eor_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_sub_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rsb_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_add_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_adc_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_sbc_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rsc_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_orr_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_mov_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_bic_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_mvn_reg_immed(uint32_t opcode) {}

uint64_t Arm7tdmi::op_ands_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_eors_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_subs_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rsbs_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_adds_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_adcs_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_sbcs_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rscs_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_tst_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_teq_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_cmp_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_cmn_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_orrs_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_movs_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_bics_reg_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_mvns_reg_immed(uint32_t opcode) {}

uint64_t Arm7tdmi::op_and_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_eor_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_sub_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rsb_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_add_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_adc_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_sbc_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rsc_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_orr_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_mov_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_bic_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_mvn_reg_reg(uint32_t opcode) {}

uint64_t Arm7tdmi::op_ands_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_eors_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_subs_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rsbs_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_adds_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_adcs_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_sbcs_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rscs_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_tst_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_teq_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_cmp_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_cmn_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_orrs_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_movs_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_bics_reg_reg(uint32_t opcode) {}
uint64_t Arm7tdmi::op_mvns_reg_reg(uint32_t opcode) {}

uint64_t Arm7tdmi::op_and_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_eor_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_sub_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rsb_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_add_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_adc_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_sbc_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rsc_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_orr_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_mov_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_bic_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_mvn_immed(uint32_t opcode) {}

uint64_t Arm7tdmi::op_ands_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_eors_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_subs_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rsbs_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_adds_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_adcs_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_sbcs_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_rscs_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_tst_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_teq_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_cmp_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_cmn_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_orrs_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_movs_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_bics_immed(uint32_t opcode) {}
uint64_t Arm7tdmi::op_mvns_immed(uint32_t opcode) {}













const uint32_t Arm7tdmi::inst_mask[] = {
    0xfff, //BX
    0xf00, //B
    0xf00, //BL
    0xfbf, //MRS
    0xfbf, //MSR reg
    0xfbf, //MSR imm

    0xff1, //AND reg-immed
    0xff1, //SUB reg-immed
    0xff1, //RSB reg-immed
    0xff1, //ADD reg-immed
    0xff1, //ADC reg-immed
    0xff1, //SBC reg-immed
    0xff1, //RSC reg-immed
    0xff1, //ORR reg-immed
    0xff1, //MOV reg-immed
    0xff1, //BIC reg-immed
    0xff1, //MVN reg-immed

    0xff1, //ANDS reg-immed
    0xff1, //SUBS reg-immed
    0xff1, //RSBS reg-immed
    0xff1, //ADDS reg-immed
    0xff1, //ADCS reg-immed
    0xff1, //SBCS reg-immed
    0xff1, //RSCS reg-immed
    0xff1, //TST  reg-immed
    0xff1, //TEQ  reg-immed
    0xff1, //CMP  reg-immed
    0xff1, //CMN  reg-immed
    0xff1, //ORRS reg-immed
    0xff1, //MOVS reg-immed
    0xff1, //BICS reg-immed
    0xff1, //MVNS reg-immed

    0xff9, //AND reg-reg
    0xff9, //SUB reg-reg
    0xff9, //RSB reg-reg
    0xff9, //ADD reg-reg
    0xff9, //ADC reg-reg
    0xff9, //SBC reg-reg
    0xff9, //RSC reg-reg
    0xff9, //ORR reg-reg
    0xff9, //MOV reg-reg
    0xff9, //BIC reg-reg
    0xff9, //MVN reg-reg
                            
    0xff9, //ANDS reg-reg
    0xff9, //SUBS reg-reg
    0xff9, //RSBS reg-reg
    0xff9, //ADDS reg-reg
    0xff9, //ADCS reg-reg
    0xff9, //SBCS reg-reg
    0xff9, //RSCS reg-reg
    0xff9, //TST  reg-reg
    0xff9, //TEQ  reg-reg
    0xff9, //CMP  reg-reg
    0xff9, //CMN  reg-reg
    0xff9, //ORRS reg-reg
    0xff9, //MOVS reg-reg
    0xff9, //BICS reg-reg
    0xff9, //MVNS reg-reg

    0xff0, //AND immed
    0xff0, //SUB immed
    0xff0, //RSB immed
    0xff0, //ADD immed
    0xff0, //ADC immed
    0xff0, //SBC immed
    0xff0, //RSC immed
    0xff0, //ORR immed
    0xff0, //MOV immed
    0xff0, //BIC immed
    0xff0, //MVN immed
                            
    0xff0, //ANDS immed
    0xff0, //SUBS immed
    0xff0, //RSBS immed
    0xff0, //ADDS immed
    0xff0, //ADCS immed
    0xff0, //SBCS immed
    0xff0, //RSCS immed
    0xff0, //TST  immed
    0xff0, //TEQ  immed
    0xff0, //CMP  immed
    0xff0, //CMN  immed
    0xff0, //ORRS immed
    0xff0, //MOVS immed
    0xff0, //BICS immed
    0xff0, //MVNS immed

    0x000  //Unrecognized/Nop
};

const uint32_t Arm7tdmi::inst_mask_match[] = {
    0x121, //BX
    0xa00, //B
    0xb00, //BL
    0x100, //MRS
    0x120, //MSR reg
    0x320, //MSR imm

    0x000, //AND reg-imm
    0x040, //SUB reg-imm
    0x060, //RSB reg-imm
    0x080, //ADD reg-imm
    0x0a0, //ADC reg-imm
    0x0c0, //SBC reg-imm
    0x0e0, //RSC reg-imm
    0x180, //ORR reg-imm
    0x1a0, //MOV reg-imm
    0x1c0, //BIC reg-imm
    0x1e0, //MVN reg-imm
                        
    0x010, //ANDS reg-imm
    0x050, //SUBS reg-imm
    0x070, //RSBS reg-imm
    0x090, //ADDS reg-imm
    0x0b0, //ADCS reg-imm
    0x0d0, //SBCS reg-imm
    0x0f0, //RSCS reg-imm
    0x110, //TST  reg-imm
    0x130, //TEQ  reg-imm
    0x150, //CMP  reg-imm
    0x170, //CMN  reg-imm
    0x190, //ORRS reg-imm
    0x1b0, //MOVS reg-imm
    0x1d0, //BICS reg-imm
    0x1f0, //MVNS reg-imm
                        
    0x001, //AND reg-reg
    0x041, //SUB reg-reg
    0x061, //RSB reg-reg
    0x081, //ADD reg-reg
    0x0a1, //ADC reg-reg
    0x0c1, //SBC reg-reg
    0x0e1, //RSC reg-reg
    0x181, //ORR reg-reg
    0x1a1, //MOV reg-reg
    0x1c1, //BIC reg-reg
    0x1e1, //MVN reg-reg
                        
    0x011, //ANDS reg-reg
    0x051, //SUBS reg-reg
    0x071, //RSBS reg-reg
    0x091, //ADDS reg-reg
    0x0b1, //ADCS reg-reg
    0x0d1, //SBCS reg-reg
    0x0f1, //RSCS reg-reg
    0x111, //TST  reg-reg
    0x131, //TEQ  reg-reg
    0x151, //CMP  reg-reg
    0x171, //CMN  reg-reg
    0x191, //ORRS reg-reg
    0x1b1, //MOVS reg-reg
    0x1d1, //BICS reg-reg
    0x1f1, //MVNS reg-reg
                        
    0x200, //AND immed
    0x240, //SUB immed
    0x260, //RSB immed
    0x280, //ADD immed
    0x2a0, //ADC immed
    0x2c0, //SBC immed
    0x2e0, //RSC immed
    0x380, //ORR immed
    0x3a0, //MOV immed
    0x3c0, //BIC immed
    0x3e0, //MVN immed
                        
    0x210, //ANDS immed
    0x250, //SUBS immed
    0x270, //RSBS immed
    0x290, //ADDS immed
    0x2b0, //ADCS immed
    0x2d0, //SBCS immed
    0x2f0, //RSCS immed
    0x310, //TST  immed
    0x330, //TEQ  immed
    0x350, //CMP  immed
    0x370, //CMN  immed
    0x390, //ORRS immed
    0x3b0, //MOVS immed
    0x3d0, //BICS immed
    0x3f0, //MVNS immed

    0x000  //Unrecognized/Nop
};

const Arm7OpPtr Arm7tdmi::inst_mask_ops[] = {
    &Arm7tdmi::op_bx,
    &Arm7tdmi::op_b,
    &Arm7tdmi::op_bl,
    &Arm7tdmi::op_mrs,
    &Arm7tdmi::op_msr_reg,
    &Arm7tdmi::op_msr_imm,
 
    &Arm7tdmi::op_and_reg_immed,
    &Arm7tdmi::op_eor_reg_immed,
    &Arm7tdmi::op_sub_reg_immed,
    &Arm7tdmi::op_rsb_reg_immed,
    &Arm7tdmi::op_add_reg_immed,
    &Arm7tdmi::op_adc_reg_immed,
    &Arm7tdmi::op_sbc_reg_immed,
    &Arm7tdmi::op_rsc_reg_immed,
    &Arm7tdmi::op_orr_reg_immed,
    &Arm7tdmi::op_mov_reg_immed,
    &Arm7tdmi::op_bic_reg_immed,
    &Arm7tdmi::op_mvn_reg_immed,

    &Arm7tdmi::op_ands_reg_immed,
    &Arm7tdmi::op_eors_reg_immed,
    &Arm7tdmi::op_subs_reg_immed,
    &Arm7tdmi::op_rsbs_reg_immed,
    &Arm7tdmi::op_adds_reg_immed,
    &Arm7tdmi::op_adcs_reg_immed,
    &Arm7tdmi::op_sbcs_reg_immed,
    &Arm7tdmi::op_rscs_reg_immed,
    &Arm7tdmi::op_tst_reg_immed,
    &Arm7tdmi::op_teq_reg_immed,
    &Arm7tdmi::op_cmp_reg_immed,
    &Arm7tdmi::op_cmn_reg_immed,
    &Arm7tdmi::op_orrs_reg_immed,
    &Arm7tdmi::op_movs_reg_immed,
    &Arm7tdmi::op_bics_reg_immed,
    &Arm7tdmi::op_mvns_reg_immed,

    &Arm7tdmi::op_and_reg_reg,
    &Arm7tdmi::op_eor_reg_reg,
    &Arm7tdmi::op_sub_reg_reg,
    &Arm7tdmi::op_rsb_reg_reg,
    &Arm7tdmi::op_add_reg_reg,
    &Arm7tdmi::op_adc_reg_reg,
    &Arm7tdmi::op_sbc_reg_reg,
    &Arm7tdmi::op_rsc_reg_reg,
    &Arm7tdmi::op_orr_reg_reg,
    &Arm7tdmi::op_mov_reg_reg,
    &Arm7tdmi::op_bic_reg_reg,
    &Arm7tdmi::op_mvn_reg_reg,

    &Arm7tdmi::op_ands_reg_reg,
    &Arm7tdmi::op_eors_reg_reg,
    &Arm7tdmi::op_subs_reg_reg,
    &Arm7tdmi::op_rsbs_reg_reg,
    &Arm7tdmi::op_adds_reg_reg,
    &Arm7tdmi::op_adcs_reg_reg,
    &Arm7tdmi::op_sbcs_reg_reg,
    &Arm7tdmi::op_rscs_reg_reg,
    &Arm7tdmi::op_tst_reg_reg,
    &Arm7tdmi::op_teq_reg_reg,
    &Arm7tdmi::op_cmp_reg_reg,
    &Arm7tdmi::op_cmn_reg_reg,
    &Arm7tdmi::op_orrs_reg_reg,
    &Arm7tdmi::op_movs_reg_reg,
    &Arm7tdmi::op_bics_reg_reg,
    &Arm7tdmi::op_mvns_reg_reg,

    &Arm7tdmi::op_and_immed,
    &Arm7tdmi::op_eor_immed,
    &Arm7tdmi::op_sub_immed,
    &Arm7tdmi::op_rsb_immed,
    &Arm7tdmi::op_add_immed,
    &Arm7tdmi::op_adc_immed,
    &Arm7tdmi::op_sbc_immed,
    &Arm7tdmi::op_rsc_immed,
    &Arm7tdmi::op_orr_immed,
    &Arm7tdmi::op_mov_immed,
    &Arm7tdmi::op_bic_immed,
    &Arm7tdmi::op_mvn_immed,

    &Arm7tdmi::op_ands_immed,
    &Arm7tdmi::op_eors_immed,
    &Arm7tdmi::op_subs_immed,
    &Arm7tdmi::op_rsbs_immed,
    &Arm7tdmi::op_adds_immed,
    &Arm7tdmi::op_adcs_immed,
    &Arm7tdmi::op_sbcs_immed,
    &Arm7tdmi::op_rscs_immed,
    &Arm7tdmi::op_tst_immed,
    &Arm7tdmi::op_teq_immed,
    &Arm7tdmi::op_cmp_immed,
    &Arm7tdmi::op_cmn_immed,
    &Arm7tdmi::op_orrs_immed,
    &Arm7tdmi::op_movs_immed,
    &Arm7tdmi::op_bics_immed,
    &Arm7tdmi::op_mvns_immed,

    &Arm7tdmi::op_noop
};

const int Arm7tdmi::op_count = 91;
