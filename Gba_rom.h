#pragma once

#include<memory>
#include<array>
#include<string>

class Gba_rom {
public:
    Gba_rom(std::string filename);
    bool loaded();

private:
    bool valid;
};
