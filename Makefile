.PHONY: main
main: byte-writer disassemble

byte-writer:
	g++ -std=c++11 -o byte-writer byte-writer.cpp


disassemble:
	g++ -std=c++11 -o disassemble disassemble.cpp arm-defs.cpp

khedgba: main.cpp Gba.cpp Gba_memmap.cpp Arm7tdmi.cpp Arm7tdmi.h Gba_rom.cpp util.cpp
	g++ -std=c++17 -g -o khedgba main.cpp Gba.cpp Gba_memmap.cpp Arm7tdmi.cpp Gba_rom.cpp util.cpp
