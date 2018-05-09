#include "Gba.h"
using namespace std;

Gba::Gba(string bios_file, string rom_file) : valid(false) {
    bus = make_shared<Gba_memmap>(bios_file, rom_file);
    cpu = make_shared<Arm7tdmi>(bus);

    if(bus->loaded()) {
        valid = true;
    }
}

bool Gba::loaded() {
    return valid;
}

void Gba::run() {

}
