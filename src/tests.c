#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "tests.h"
#include "cpu.h"
#include "alu.h"

#define ASSERT_FLAGS(z, n, h, c) \
    do { \
        assert(cpu_flag(&cpu, FLAG_Z) == (z)); \
        assert(cpu_flag(&cpu, FLAG_N) == (n)); \
        assert(cpu_flag(&cpu, FLAG_H) == (h)); \
        assert(cpu_flag(&cpu, FLAG_C) == (c)); \
    } while(0)

void alu_test() {
    struct cpu cpu;

    puts("[ALU tests]");

    puts("- alu_add_u16_i8");
    assert(alu_add_u16_i8(&cpu, 0x0FFF, 0x01) == 0x1000);
    ASSERT_FLAGS(0, 0, 1, 0);
    assert(alu_add_u16_i8(&cpu, 0xFFFF, 0x01) == 0x0000);
    ASSERT_FLAGS(0, 0, 1, 1);
    assert(alu_add_u16_i8(&cpu, 0x01, -0x02) == 0xFFFF);
    ASSERT_FLAGS(0, 0, 0, 1);   /* not sure */

    puts("- alu_add");
    assert(alu_add(&cpu, 0x0F, 0x01) == 0x10);
    ASSERT_FLAGS(0, 0, 1, 0);
    assert(alu_add(&cpu, 0x3A, 0xC6) == 0x00);
    ASSERT_FLAGS(1, 0, 1, 1);
    assert(alu_add(&cpu, 0x3C, 0xFF) == 0x3B);
    ASSERT_FLAGS(0, 0, 1, 1);
    assert(alu_add(&cpu, 0x3C, 0x12) == 0x4E);
    ASSERT_FLAGS(0, 0, 0, 0);

    puts("- alu_adc");
    cpu.f = FLAG_C;
    assert(alu_adc(&cpu, 0xE1, 0x0F) == 0xF1);
    ASSERT_FLAGS(0, 0, 1, 0);
    assert(alu_adc(&cpu, 0xE1, 0x3B) == 0x1C);
    ASSERT_FLAGS(0, 0, 0, 1);

    puts("- alu_sub");
    assert(alu_sub(&cpu, 0x3E, 0x3E) == 0x00);
    ASSERT_FLAGS(1, 1, 0, 0);

    puts("- alu_sbc");
    cpu.f = FLAG_C;
    assert(alu_sbc(&cpu, 0x3B, 0x2A) == 0x10);
    ASSERT_FLAGS(0, 1, 0, 0);

    puts("- alu_and");
    assert(alu_and(&cpu, 0x5A, 0x3F) == 0x1A);
    ASSERT_FLAGS(0, 0, 1, 0);

    puts("- alu_xor");
    assert(alu_xor(&cpu, 0xFF, 0x0F) == 0xF0);
    ASSERT_FLAGS(0, 0, 0, 0);

    puts("- alu_or");
    assert(alu_or(&cpu, 0x5A, 0x0F) == 0x5F);
    ASSERT_FLAGS(0, 0, 0, 0);

    puts("- alu_cp");
    alu_cp(&cpu, 0x3C, 0x2F);
    ASSERT_FLAGS(0, 1, 1, 0);
}

void test_main() {
    puts("*** Running tests ***");
    alu_test();
}
