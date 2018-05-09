#include "Gba_memmap.h"
#include "util.h"
#include <iostream>
#include <fstream>
#include<cassert>

using namespace std;

Gba_memmap::Gba_memmap(std::string& bios_file, std::string& rom_file) : rom(rom_file), bios_valid(false) {
    uint32_t expected_crc = 2174186293;
    uint32_t file_crc = util::crc32(bios_file);
    if(file_crc != expected_crc) {
        cout<<"Doesn't look like a valid gba_bios. Expected crc: "<<hex<<expected_crc<<", got "<<file_crc<<endl;
    }
    else {
        ifstream in(bios_file.c_str());
        in.seekg(0,ios::end);
        size_t insize = in.tellg();
        assert(sizeof(word) == 4);
        assert(sizeof(array<word,0x1000>) == 16384);
        assert(insize == 16384);
        in.seekg(0,ios::beg);
        in.read(reinterpret_cast<char *>(&bios[0]), 16384);
        in.close();
        bios_valid = true;
    }
}

bool Gba_memmap::loaded() {
    return bios_valid && rom.loaded();
}

read_response Gba_memmap::read32(uint32_t addr, uint64_t cycle) {
    addr &= 0xFFFFFFFC;
    switch(addr & 0xFF000000) {
        case 0x00000000:
            if(addr < 0x4000) {
                return read_response{bios[addr>>2], 1};
            }
            else {
                return read_response{word{.word = 0}, 1};
            }
            break;
        case 0x02000000:
            break;
        case 0x03000000:
            break;
        case 0x04000000:
            break;
        case 0x05000000:
            break;
        case 0x06000000:
            break;
        case 0x07000000:
            break;
        case 0x08000000:
            break;
        case 0x0A000000:
            break;
        case 0x0C000000:
            break;
        //case 0x0E000000: Gamepak SRAM is an 8-bit only bus
        //    break;
        default:
            break;
    }
    return read_response{0,0};
}

read_response Gba_memmap::read16(uint32_t addr, uint64_t cycle) {
    addr &= 0xFFFFFFFE;
    switch(addr & 0xFF000000) {
        case 0x00000000:
            break;
        case 0x02000000:
            break;
        case 0x03000000:
            break;
        case 0x04000000:
            break;
        case 0x05000000:
            break;
        case 0x06000000:
            break;
        case 0x07000000:
            break;
        case 0x08000000:
            break;
        case 0x0A000000:
            break;
        case 0x0C000000:
            break;
        //case 0x0E000000: Gamepak SRAM is an 8-bit only bus
        //    break;
        default:
            break;
    }
    return read_response{0,0};
}

read_response Gba_memmap::read8(uint32_t addr, uint64_t cycle) {
    switch(addr & 0xFF000000) {
        case 0x00000000:
            break;
        case 0x02000000:
            break;
        case 0x03000000:
            break;
        case 0x04000000:
            break;
        case 0x05000000:
            break;
        case 0x06000000:
            break;
        case 0x07000000:
            break;
        case 0x08000000:
            break;
        case 0x0A000000:
            break;
        case 0x0C000000:
            break;
        case 0x0E000000:
            break;
        default:
            break;
    }
    return read_response{0,0};
}

uint64_t      Gba_memmap::write32(uint32_t addr, uint32_t val, uint64_t cycle) {
    addr &= 0xFFFFFFFC;
    switch(addr & 0xFF000000) {
        //case 0x00000000: BIOS ROM can't be written
        //    break;
        case 0x02000000:
            break;
        case 0x03000000:
            break;
        case 0x04000000:
            break;
        case 0x05000000:
            break;
        case 0x06000000:
            break;
        case 0x07000000:
            break;
        //case 0x08000000: Gamepak ROM can't be written
        //    break;
        //case 0x0A000000: Gamepak ROM can't be written
        //    break;
        //case 0x0C000000: Gamepak ROM can't be written
        //    break;
        //case 0x0E000000: Gamepak SRAM is an 8-bit only bus
        //    break;
        default:
            break;
    }

}

uint64_t      Gba_memmap::write16(uint32_t addr, uint16_t val, uint64_t cycle) {
    addr &= 0xFFFFFFFE;
    switch(addr & 0xFF000000) {
        //case 0x00000000: BIOS ROM can't be written
        //    break;
        case 0x02000000:
            break;
        case 0x03000000:
            break;
        case 0x04000000:
            break;
        case 0x05000000:
            break;
        case 0x06000000:
            break;
        case 0x07000000:
            break;
        //case 0x08000000: Gamepak ROM can't be written
        //    break;
        //case 0x0A000000: Gamepak ROM can't be written
        //    break;
        //case 0x0C000000: Gamepak ROM can't be written
        //    break;
        //case 0x0E000000: Gamepak SRAM is an 8-bit only bus
        //    break;
        default:
            break;
    }

}

uint64_t      Gba_memmap::write8(uint32_t addr, uint8_t val, uint64_t cycle) {
    switch(addr & 0xFF000000) {
        //case 0x00000000: BIOS ROM can't be written
        //    break;
        case 0x02000000:
            break;
        case 0x03000000:
            break;
        case 0x04000000:
            break;
        //case 0x05000000: Palette RAM writes can't be 8-bit
        //    break;
        //case 0x06000000: VRAM writes can't be 8-bit
        //    break;
        //case 0x07000000: OAM writes can't be 8-bit
        //    break;
        //case 0x08000000: Gamepak ROM can't be written
        //    break;
        //case 0x0A000000: Gamepak ROM can't be written
        //    break;
        //case 0x0C000000: Gamepak ROM can't be written
        //    break;
        case 0x0E000000:
            break;
        default:
            break;
    }

}
