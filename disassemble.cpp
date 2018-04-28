#include<iostream>
#include<fstream>
#include<cstdint>
#include<vector>
#include<cassert>
#include<string>

#include "arm-defs.h"

using namespace std;

int decode_a(uint32_t inst, uint32_t offset);
int decode_t(uint16_t inst, uint32_t offset);
string instr_str_a(uint32_t op, uint32_t offset, int format);
string instr_str_t(uint16_t op, uint32_t offset, int format);

int main(int argc, char *argv[]) {
    assert(sizeof(instr) == 4);
    assert(sizeof(status) == 4);

    if(argc != 2) {
        return 1;
    }
    
    ifstream rom(argv[1]);
    if(!rom.is_open()) {
        return 2;
    }

    size_t file_size = 0;
    rom.seekg(0,ios::end);
    file_size = rom.tellg();
    rom.seekg(0,ios::beg);
    vector<char> data(file_size,0);
    rom.read(&data[0], file_size);
    rom.close();
    for(size_t i = 0; i < file_size; i += 4) {
        cout.width(8);
        cout.fill('0');
        cout<<hex<<i<<": ";
        cout.width(8);
        cout.fill('0');
        cout<<(*(reinterpret_cast<uint32_t *>(&data[i])));
        int result = decode_a(*(reinterpret_cast<uint32_t *>(&data[i])), i);
        if(result < 0) {
            cout<<"\tUNKNOWN"<<endl;
        }
    }
    return 0;
}

int decode_a(uint32_t inst, uint32_t offset) {
    instr i;
    i.val = inst;
    cout<<"\t";
    int ret = -1;
    for(int j=0;j<22;j++) {
        if((i.val & instr_mask[j]) == instr_match[j]) {
            cout<<dec<<j<<": "<<instr_str_a(inst, offset, j)<<endl;
            ret = j;
            break;
        }
    }
    return ret;
}

int decode_t(uint16_t inst) {
    int ret = 0;
    return ret;
}

uint32_t rotl(uint32_t val, uint32_t rot) {
    rot &= 0x1f;
    return (val<<rot) | (val>>((-rot)&0x1f));
}

uint32_t rotr(uint32_t val, uint32_t rot) {
    rot &= 0x1f;
    return (val>>rot) | (val<<((-rot)&0x1f));
}

string to_hex_string(uint32_t in) {
    char digits[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    string out("");
    int first=0;
    for(int i=7;i>=0;i--) {
        int digit = ((in & (0xf<<(i*4)))>>(i*4));
        if(digit && i > first) first = i;
        if(first >= i) out += digits[digit];
    }
    return out;
}

int32_t sign_extend(uint32_t val, uint32_t bits) {
    bits &= 0x1f;
    if(!bits) return val;
    if(val & (1<<(bits - 1))) { //high bit is set; return negative number
        return (-int32_t(val))-1;
    }
    return val;
}

string imm_reg_op2(uint32_t op) {
   //When below Bit 4 R=0 - Shift by Immediate
   //          11-7   Is - Shift amount   (1-31, 0=Special/See below)

    instr i;
    i.val = op;
    uint8_t shift_amount = ((i.reg11 * 16 + i.field7)>>3);
    uint8_t stype = ((i.field7>>1) & 3);
    uint8_t reg = i.reg3;
    return string("R")+to_string(reg)+", "+shift_type[stype]+" #"+to_string(shift_amount);
}

string reg_reg_op2(uint32_t op) {
     //   When below Bit 4 R=1 - Shift by Register
       //           11-8   Rs - Shift register (R0-R14) - only lower 8bit 0-255 used
         //               7      Reserved, must be zero  (otherwise multiply or undefined opcode)
           //                 6-5    Shift Type (0=LSL, 1=LSR, 2=ASR, 3=ROR)
             //                   4      R - Shift by Register Flag (0=Immediate, 1=Register)
               //                     3-0    Rm - 2nd Operand Register (R0..R15) (including PC=R15)
    instr i;
    i.val = op;
    uint8_t shift_reg = i.reg11;
    uint8_t stype = ((i.field7>>1) & 3);
    uint8_t shifted_reg = i.reg3;
    return string("R")+to_string(shifted_reg)+", "+shift_type[stype]+" R"+to_string(shift_reg);
}

string imm_imm_op2(uint32_t op) {
//        11-8   Is - ROR-Shift applied to nn (0-30, in steps of 2)
  //              7-0    nn - 2nd Operand Unsigned 8bit Immediate
    uint32_t immed = op & 0xff;
    immed = rotr(immed, 2 * ((op & 0xf00)>>8));
    return string("#"+to_string(immed));
}

string instr_str_a(uint32_t op, uint32_t offset, int format) {
    instr in;
    in.val = op;
    switch(format) {
        case 0: //Branch and exchange
            return string("BX") + cond_str[in.cond] + " R" + to_string(in.reg3);
            break;
        case 1: //MRS (copy status to register)
            return string("MRS") + cond_str[in.cond] + " R" + to_string(in.reg15) + ", " + (((in.op & 4) == 4)?"SPSR":"CPSR");
            break;
        case 2: //MSR reg->reg (copy register to status)
            return string("MSR") + cond_str[in.cond] + (((in.op & 4) == 4)?" SPSR":" CPSR") + ", R" + to_string(in.reg3);
            break;
        case 3: //MSR reg->reg_flg (copy register to flags)
            return string("MSR") + cond_str[in.cond] + (((in.op & 4) == 4)?" SPSR":" CPSR") + "_flg, R" + to_string(in.reg3);
            break;
        case 4: //MSR immediate->reg_flg (copy shifted immediate to flags)
            {
                uint32_t immed = rotr(in.reg3 + (in.field7 * 16), in.reg11 * 2);
                return string("MSR") + cond_str[in.cond] + (((in.op & 4) == 4)?"SPSR":"CPSR") + "_flg, R" + to_string(immed);
            }
            break;
        case 5: //ALU ops (immediate-shifted reg operand)
            {
                int alu_op = ((in.op & 0x1f)>>1);
                string ret(string(alu_ops[(in.op & 0x1f)>>1]) + (((in.op&1) == 1)?"S":"") + cond_str[in.cond] + " ");
                string extra("");
                if(alu_op < 8 || alu_op > 0xb) { //print rd
                    ret+="R"+to_string(in.reg15)+", ";
                }
                else if(in.reg15 != 0 && in.reg15 != 15) extra += "\tRd looks wrong for this opcode";
                if(alu_op != 0xd && alu_op != 0xf) { //print rn
                    ret+="R"+to_string(in.reg19)+", ";
                }
                else if(in.reg19 != 0) extra += "\tRn looks wrong for this opcode";
                ret+=imm_reg_op2(op)+extra;
                return ret;
            }
            break;
        case 6: //ALU ops (reg-shifted reg operand)
            {
                int alu_op = ((in.op & 0x1f)>>1);
                string ret(string(alu_ops[(in.op & 0x1f)>>1]) + (((in.op&1) == 1)?"S":"") + cond_str[in.cond] + " ");
                string extra("");
                if(alu_op < 8 || alu_op > 0xb) { //print rd
                    ret+="R"+to_string(in.reg15)+", ";
                }
                else if(in.reg15 != 0 && in.reg15 != 15) extra += "\tRd looks wrong for this opcode";
                if(alu_op != 0xd && alu_op != 0xf) { //print rn
                    ret+="R"+to_string(in.reg19)+", ";
                }
                else if(in.reg19 != 0) extra += "\tRn looks wrong for this opcode";
                ret+=reg_reg_op2(op)+extra;
                return ret;
            }
            break;
        case 7: //ALU ops (immediate-shifted immediate operand)
            {
                int alu_op = ((in.op & 0x1f)>>1);
                string ret(string(alu_ops[(in.op & 0x1f)>>1]) + (((in.op&1) == 1)?"S":"") + cond_str[in.cond] + " ");
                string extra("");
                if(alu_op < 8 || alu_op > 0xb) { //print rd
                    ret+="R"+to_string(in.reg15)+", ";
                }
                else if(in.reg15 != 0 && in.reg15 != 15) extra += "\tRd looks wrong for this opcode";
                if(alu_op != 0xd && alu_op != 0xf) { //print rn
                    ret+="R"+to_string(in.reg19)+", ";
                }
                else if(in.reg19 != 0) extra += "\tRn looks wrong for this opcode";
                ret+=imm_imm_op2(op)+extra;
                return ret;
            }
            break;
        case 8: //Multiply
            {
                string s = (((in.op & 1) == 1)?"S":"");
                string c = string(cond_str[in.cond]) + " ";
                string reg_list = "R" + to_string(in.reg19) + ", R" + to_string(in.reg3) + ", R" + to_string(in.reg11);
                if((in.op & 2) == 2) return string("MLA") + s + c + reg_list + ", R" + to_string(in.reg15);
                else                 return string("MUL") + s + c + reg_list;
            }
            break;
        case 9: //Multiply Long
            {
                string sign = (((in.op & 4) == 4)?"S":"U");
                string s = (((in.op & 1) == 1)?"S":"");
                string c = string(cond_str[in.cond]) + " ";
                string reg_list = "R" + to_string(in.reg15) + ", R" + to_string(in.reg19) + ", R" + to_string(in.reg3) + ", R" + to_string(in.reg11);
                if((in.op & 2) == 2) return sign + string("MLAL") + s + c + reg_list;
                else                 return sign + string("MULL") + s + c + reg_list;
            }
            break;
        case 10: //Load/Store immediate
            break;
        case 11: //Load/Store immediate-shifted register
            break;
        case 12: //SWP operation
            break;
        case 13: //Load/Store half-word/signed-byte (register operand)
            break;
        case 14: //Load/Store half-word/signed-byte (immediate offset)
            break;
        case 15: //Load/Store multiple
            break;
        case 16: //Branch/Branch+Link
            return string("B") + (((in.op & 16)==16)?"L":"") + cond_str[in.cond] + " 0x" + to_hex_string(sign_extend((in.val & 0x00ffffff)*4 + 8 + offset, 26));
            break;
        case 17: //Software Interrupt
            return string("SWI 0x") + to_hex_string(in.val & 0x00ffffff);
            break;
        case 18: //CDP (coprocessor inst)
            return string("CDP (unexpected instruction)");
            break;
        case 19: //LDC/STC (coprocessor load/store)
            return string("LDC/STC (unexpected instruction)");
            break;
        case 20: //MRC/MCR (coprocessor register transfer?)
            return string("MRC/MCR (unexpected instruction)");
            break;
        case 21: //Undefined
            return string("<undefined>");
            break;
        default:
            break;
    }
    return string("");
}

string instr_str_t(uint16_t opcode, uint32_t offset, int format) {}
