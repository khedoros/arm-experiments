#include<iostream>
#include<fstream>
#include<cstdint>
#include<vector>
#include<cassert>

#include "arm.h"

using namespace std;

int decode_a(uint32_t inst);
int decode_t(uint16_t inst);

int main(int argc, char *argv[]) {
    cout<<"instr: "<<sizeof(instr)<<" status: "<<sizeof(status)<<endl;
    //assert(sizeof(instr) == 4);
    //assert(sizeof(status) == 4);

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
        int result = decode_a(*(reinterpret_cast<uint32_t *>(&data[i])));
        if(result < 0) {
            cout<<"\tUNKNOWN"<<endl;
        }
    }
    return 0;
}

int decode_a(uint32_t inst) {
    instr i;
    i.val = inst;
    cout<<"\t";
    int ret = -1;
    for(int j=0;j<22;j++) {
        if((i.val & instr_mask[j]) == instr_match[j]) {
            cout<<dec<<j<<": "<<instr_names[j]<<endl;
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
