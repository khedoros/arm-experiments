#include <array>
#include <utility>

struct cpu {
    using cpu_func_ptr = void (cpu::*)();

    template<int I>
    void ops() { }
};

template<unsigned long... I>
constexpr inline auto Init(std::index_sequence<I...>) {
    return std::array{ &cpu::ops<I>... };
}

constexpr std::array funcs = Init(std::make_index_sequence<16>());

const std::array<cpu::cpu_func_ptr, 16>& get() { return funcs; }
