#pragma once
#include<memory>
#include<cstdint>
#include<utility>

#include "Arm7tdmi.h"
#include "Gba_memmap.h"
//#include "Gba_ppu.h"

class Gba {
public:
    Gba(std::string, std::string);
    void run();
    bool loaded();

private:
    std::shared_ptr<Arm7tdmi> cpu;
    std::shared_ptr<Gba_memmap> bus;
    //std::shared_ptr<Gba_ppu> lcd;

    bool valid;
};
