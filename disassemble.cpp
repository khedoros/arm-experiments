#include<iostream>
#include<fstream>
#include<cstdint>
#include<vector>
#include<cassert>

#include "arm.h"

using namespace std;

int decode(uint32_t inst);

int main(int argc, char *argv[]) {
    cout<<"instr: "<<sizeof(instr)<<" status: "<<sizeof(status)<<endl;
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
        int result = decode(*(reinterpret_cast<uint32_t *>(&data[i])));
        if(!result) {
            cout<<"\tUNKNOWN"<<endl;
        }
    }
    return 0;
}

int decode(uint32_t inst) {
    instr i;
    i.val = inst;
    cout<<"\t";
    int ret = 0;
    switch(i.id) {
        case 0:
            cout<<"ALU stuff ";
            break;
        case 1:
            cout<<"Data FSR ";
            break;
        case 2:
            cout<<"<Invalid> ";
            break;
        case 3:
            cout<<"Single data transfer ";
            break;
        case 4:
            cout<<"Block data transfer ";
            break;
        case 5:
            cout<<"B/BL "<<cond_str[i.cond]<<endl;
            ret = 1;
            break;
        case 6:
            cout<<"CoProcessor data transfer ";
            break;
        case 7:
            cout<<"CoProcessor data transfer/SWI ";
            break;
    }
    return ret;
}
