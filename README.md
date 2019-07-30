# khedgba - Aspiration to Emulatordom!

### 2019-07-29

Well, I've actually been doing some bits of coding over the past few days, about 7 months since I last made a code submission, and well over a year since the last time I wrote something here. The decoding I mentioned before would work, but would either require a few levels of decoding passes, or writing some 90 functions and *still* having a fair bit of branching. So I researched (i.e. what I was doing 7 months ago), and decided on...OK, I'm still trying to find a clear way to say it. It involves generating a few hundred function variations using templates and constexpr, and relying on the compiler to recognize branching based on the template parameter, optimizing out branches that won't be taken for that instruction. So, I'm going to write about 11 functions to implement the instruction set, and auto-generating close to 700 functions, to be placed in a 4K-item function pointer table.

I've got a fairly bare start on actually getting the instructions implemented, but I've had a few proof-of-concept kinds of experiments. This is all kind of overkill, but hey, I've built a couple of instruction decoders before, and it's kind of fun to stretch a little.

### 2018-04-24

Currently, I'm going back and forth through ARM documents to discover where the various opcodes fit into the 4-billion combinations afforded by a 32-bit number, and deciding on an efficient way to identify the codes. Since my previous experience is limited to the NES's 6502-like and the GB/GBC's z80/8080-like CPUs, moving to a semi-regular 32-bit ISA is a big jump. Also, it'll be my first CPU with multiple modes, memory protections, and such.

Exactly what I'm doing right now is identifying the order that I need to run my decoding tests in. For example, the MSR and MRS instructions fit into ALU opcodes 8-11 (which are value-testing operations), in the case that the "don't set status bits" flag is set. The multiplication opcodes are similarly stuck onto ALU opcodes 0-3 (variations on AND and XOR), in the case of one particular bit combination that would otherwise specify an invalid second operand.

My first read of the instruction set was to see if I could puzzle out some of the patterns intuitively. My second read through made me more familiar with some of the bit patterns, and operations that overlap with each other. This is my third time through, and I'm taking special care to document the combinations that aren't used by the expected sets of instructions.

