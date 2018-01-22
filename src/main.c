#include <stdlib.h>
#include <stdio.h>
#include "gboy.h"

#include "mmu.h"
#include "cpu.h"
#include "alu.h"

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
