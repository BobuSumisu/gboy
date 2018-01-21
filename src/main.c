#include <stdlib.h>
#include <stdio.h>
#include "gboy.h"

#include "mmu.h"
#include "cpu.h"
#include "alu.h"

void testing() {
    struct cpu cpu;
    struct mmu mmu;
    cpu_init(&cpu, &mmu);

    cpu.pc = 0x4000;
    cpu.c = 2;
    mmu_wb(&mmu, 0x4000, 0x0D); /* DEC C */
    mmu_wb(&mmu, 0x4001, 0x20);
    mmu_wb(&mmu, 0x4002, 0x10); /* JR NZ,0x4002+0x10 */
    cpu_step(&cpu);
    cpu_step(&cpu);
    cpu_debug(&cpu);
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s ROM-FILE\n", argv[0]);
        exit(1);
    }

    struct gboy gb;
    gboy_init(&gb);
    gboy_run(&gb, argv[1]);
    gboy_cleanup(&gb);
    return 0;
}
