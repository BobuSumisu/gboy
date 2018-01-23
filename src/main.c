#include <stdlib.h>
#include <stdio.h>
#include "gboy.h"

#include "mmu.h"
#include "cpu.h"
#include "alu.h"

void test() {
    struct cpu cpu;
    cpu.a = 0x45;
    cpu.b = 0x38;
    cpu.a = alu_add(&cpu, cpu.a, cpu.b);
    printf("A <- 0x%02X, N <- %d\n", cpu.a, cpu.f & FLAG_N);
    cpu.a = alu_daa(&cpu, cpu.a);
    printf("A <- 0x%02X, C <- %d\n", cpu.a, cpu.f & FLAG_C);
    cpu.a = alu_sub(&cpu, cpu.a, cpu.b);
    printf("A <- 0x%02X, N <- %d\n", cpu.a, (cpu.f & FLAG_N) != 0);
    cpu.a = alu_daa(&cpu, cpu.a);
    printf("A <- 0x%02X\n", cpu.a);
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
