#include "Gba_rom.h"

using namespace std;

Gba_rom::Gba_rom(string filename) {
    valid = true;
}

bool Gba_rom::loaded() {
    return valid;
}
