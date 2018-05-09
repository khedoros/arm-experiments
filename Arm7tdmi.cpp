#include "Arm7tdmi.h"

using namespace std;

Arm7tdmi::Arm7tdmi(shared_ptr<Gba_memmap>& b) : bus(b) {

}

int Arm7tdmi::run(uint64_t run_to) {
    return 0;
}

uint32_t Arm7tdmi::decode() {
    return 0;
}

void Arm7tdmi::execute() {

}
