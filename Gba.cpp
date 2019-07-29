#include<iostream>
#include "Gba.h"
using namespace std;

Gba::Gba(string bios_file, string rom_file) : valid(false) {
    bus = make_shared<Gba_memmap>(bios_file, rom_file);
    cpu = make_shared<Arm7tdmi>(bus);

    if(bus->loaded()) {
        valid = true;
    }

    uint64_t cycle = 0;
    uint64_t cycle_chunk = 16384;
    while(1==1) {
        //cout<<"Run cycle "<<cycle<<"->"<<cycle+cycle_chunk<<"\n";
        int ret = cpu->run(cycle + cycle_chunk);
        //TODO: Run GPU
        //TODO: Run APU
        cycle += cycle_chunk;
    }
}

bool Gba::loaded() {
    return valid;
}

void Gba::run() {

}
