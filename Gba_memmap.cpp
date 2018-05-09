#include "Gba_memmap.h"

Gba_memmap::Gba_memmap(std::string& bios_file, std::string& rom_file) : rom(rom_file) {

}

bool Gba_memmap::loaded() {
    return rom.loaded();
}

read_response Gba_memmap::read32(uint32_t addr, uint64_t cycle) {
    addr &= 0xFFFFFFFC;
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
        case 0x0E000000:
            break;
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
}

uint64_t      Gba_memmap::write16(uint32_t addr, uint16_t val, uint64_t cycle) {
    addr &= 0xFFFFFFFE;
}

uint64_t      Gba_memmap::write8(uint32_t addr, uint8_t val, uint64_t cycle) {

}
