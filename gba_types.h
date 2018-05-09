#pragma once

union word {
    uint32_t word;
    uint16_t hw[2];
    uint8_t byte[4];
};

struct read_response {
    word data; //data that was read
    uint64_t time; //cycles it took to read
};

