#include<iostream>
#include<string>
#include<memory>

using namespace std;

#include "Gba.h"

int main() {
    Gba system(string("gba_bios.bin"), string("rom.gba"));
    if(system.loaded()) {
        system.run();
    }
    return 0;
}
