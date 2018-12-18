#include<iostream>
#include<vector>
#include<functional>
#include<cstdint>

class cpu;
typedef cpu_func_ptr;

class cpu {
    public:
    enum alu_op {add_op, or_op, xor_op, op_end};
    enum set_status {set, no_set, set_end};
    typedef uint32_t cpu_op;
    typedef uint32_t reg;
    cpu(): regs{0} {};
    static const std::vector<std::function<cpu_func_ptr>> functs{ops<add_op,set>,ops<add_op,no_set>};

    template <alu_op op, set_status s>
        void ops(cpu_op i) {
            //extract operands from cpu_op argument
            switch(op) {
            add_op:
                break;
            or_op:
                break;
            xor_op:
                break;
            default:
                break;
            }

            if(s==set) {
            }
        }
    private:
    reg regs[16];
};

typedef void(cpu::*cpu_func_ptr)(cpu::cpu_op);
std::vector<std::function<cpu_func_ptr>> cpu::functs;

/*
constexpr void add() {
    cpu * a = nullptr;
    for(cpu::alu_op i=cpu::alu_op::add_op;i!=cpu::alu_op::op_end;i = cpu::alu_op(i+1)) {
        for(cpu::set_status j=cpu::set_status::set;j!=cpu::set_status::set_end;j = cpu::set_status(j+1)) {
            cpu::functs.push_back(a->ops<i,j>);
        }
    }
}
*/

int main() {
    //add();

    cpu a;
    cpu::cpu_op i=0;
    for(auto &f:a.functs) {
        f(i);
        i++;
    }
    return 0;
}
