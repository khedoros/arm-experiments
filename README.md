# arm-experiments

Work involved in deciding if I want to write a GBA emulator.

### 2018-04-24

Currently, I'm going back and forth through ARM documents to discover where the various opcodes fit into the 4-billion combinations afforded by a 32-bit number, and deciding on an efficient way to identify the codes. Since my previous experience is limited to the NES's 6502-like and the GB/GBC's z80/8080-like CPUs, moving to a semi-regular 32-bit ISA is a big jump. Also, it'll be my first CPU with multiple modes, memory protections, and such.

Exactly what I'm doing right now is identifying the order that I need to run my decoding tests in. For example, the MSR and MRS instructions fit into ALU opcodes 8-11 (which are value-testing operations), in the case that the "don't set status bits" flag is set. The multiplication opcodes are similarly stuck onto ALU opcodes 0-3 (variations on AND and XOR), in the case of one particular bit combination that would otherwise specify an invalid second operand.

My first read of the instruction set was to see if I could puzzle out some of the patterns intuitively. My second read through made me more familiar with some of the bit patterns, and operations that overlap with each other. This is my third time through, and I'm taking special care to document the combinations that aren't used by the expected sets of instructions.
