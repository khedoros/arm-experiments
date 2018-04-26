#include<bitset>
#include<iostream>
#include<fstream>
#include<cstdint>
#include<string>
using namespace std;
//1110 000X XXXX 0000 0000 0000 1XX1 0000 : Halfword/Doubleword data transfer
//1110 0000 00XX 0000 0000 0000 1001 0000 Multiply
//1110 0000 1XXX 0000 0000 0000 1001 0000 Long Multiply
int main() {
    {
        ofstream out("data_transfer_bits_out.bin");
        bitset<32> number(string("11100000000000000000000010010000"));
        for(int i=0;i<32;i++) {
            for(int j=0;j<4;j++) {
                uint32_t base = number.to_ulong();
                base |= (i<<20);
                base |= (j<<5);
                out.write(reinterpret_cast<char *>(&base), 4);
            }
        }
        out.close();
    }
    {
        ofstream out("mult_bits_out.bin");
        bitset<32> number(string("11100000000000000000000010010000"));
        for(int i=0;i<4;i++) {
            uint32_t base = number.to_ulong();
            base |= (i<<20);
            out.write(reinterpret_cast<char *>(&base), 4);
        }
        out.close();
    }
    {
        ofstream out("long_mult_bits_out.bin");
        bitset<32> number(string("11100000100000000000000010010000"));
        for(int i=0;i<8;i++) {
            uint32_t base = number.to_ulong();
            base |= (i<<20);
            out.write(reinterpret_cast<char *>(&base), 4);
        }
        out.close();
    }
    {
        ofstream out("thumb_instructions.bin");
        for(uint32_t i=0;i<65536;i++) {
            out.write(reinterpret_cast<char *>(&i), 2);
        }
        out.close();
    }
    return 0;
}
