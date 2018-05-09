#pragma once

#include<cstdint>
#include<array>
#include<string>
#include "Gba_rom.h"
#include "gba_types.h"

/*  I/O register maps (very rough view):
      LCD I/O between 4000000 and 4000056
      Sound I/O between 4000060 and 40000A8
      DMA control between 40000B0 and 40000E0
      Timer between       40000100 and 4000110
      Serial(1) between 4000120 and 400012C
      Keypad between 4000130 and 4000134
      Serial(2) between 4000134 and 400015A
      Interrupts, waitstates, power-down: 4000200-4000804
*/

class Gba_memmap {

    bool bios_valid;
    std::array<word, 0x1000> bios;     //0x00000000 BIOS ROM           (16KB), 32-bit bus width
    std::array<word, 0x10000> wram;    //0x02000000 On-board work RAM  (256KB), 16-bit bus width
    std::array<word, 0x2000> cpu_wram; //0x03000000 On-CPU work RAM    (32KB), 32-bit bus width
                                       //0x04000000 IO Registers       (1023 bytes), 32-bit bus width
                                       //0x05000000 BG/OBJ Palette RAM (1KB), 32-bit bus width
                                       //0x06000000 VRAM               (96KB), 16-bit bus width
                                       //0x07000000 OAM/OBJ Attributes (1KB), 32-bit bus width
    Gba_rom rom;                       //0x08000000 Game Pak ROM/flash - Wait State 0 (up to 32MB), 16-bit bus width
                                       //0x0A000000 Game Pak ROM/flash - Wait State 1 (up to 32MB), 16-bit bus width
                                       //0x0C000000 Game Pak ROM/flash - Wait State 2 (up to 32MB), 16-bit bus width
                                       //0x0E000000 Game Pak SRAM               (up to 64KB), 8-bit bus width
public:
    Gba_memmap(std::string& bios_file, std::string& rom_file);
    bool loaded();
    read_response read32(uint32_t addr, uint64_t cycle);
    read_response read16(uint32_t addr, uint64_t cycle);
    read_response read8(uint32_t addr, uint64_t cycle);
    uint64_t      write32(uint32_t addr, uint32_t val, uint64_t cycle);
    uint64_t      write16(uint32_t addr, uint16_t val, uint64_t cycle);
    uint64_t      write8(uint32_t addr, uint8_t val, uint64_t cycle);
};
