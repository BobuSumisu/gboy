#include <stdio.h>

#include "cpu.h"
#include "mmu.h"

#ifdef TEST
#include "tests.h"
#endif

int main() {

#ifdef TEST
    test_main();
#else
    struct cpu cpu;
    struct mmu mmu;

    mmu_init(&mmu);
    cpu_init(&cpu, &mmu);

    cpu_run(&cpu);

    mmu_free(&mmu);
    cpu_free(&cpu);
#endif
    return 0;
}
