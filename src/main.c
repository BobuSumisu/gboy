#include "cpu.h"

int main() {
    struct cpu *cpu = cpu_new();
    cpu_run(cpu);
    cpu_free(cpu);
    return 0;
}
