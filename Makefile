main: byte-writer disassemble

byte-writer:
	g++ -std=c++11 -o byte-writer byte-writer.cpp


disassemble:
	g++ -std=c++11 -o disassemble disassemble.cpp
