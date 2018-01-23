#include <stdlib.h>
#include <stdio.h>
#include "cartridge.h"

static const uint8_t NINTENDO_LOGO[] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
};

int cartridge_open(struct cartridge *cartridge, const char *path) {
    cartridge->data = NULL;
    cartridge->size = 0;

    FILE *fp = fopen(path, "rb");
    if(fp == NULL) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    uint32_t size = (uint32_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *data = malloc(size);
    if(fread(data, size, 1, fp) != 1) {
        free(data);
        fclose(fp);
        return -1;
    }

    fclose(fp);

    /* Verify cartridge header (start jump and logo). */

    struct cartridge_header *header = (struct cartridge_header *)&data[0x100];
    if(header->const_nop != 0x00 || header->const_jp != 0xC3) {
        fprintf(stderr, "cartridge error: expected 00 C3 (NOP, JP) at 0x100, got: %02X %02X\n",
            header->const_nop, header->const_jp);
        free(data);
        return -1;
    }

    for(int i = 0; i < 0x30; i++) {
        if(header->logo[i] != NINTENDO_LOGO[i]) {
            fprintf(stderr, "cartridge error: Nintendo logo mismatch (at 0x%04X)\n", 0x0104 + i);
            free(data);
            return -1;
        }
    }

    /* Verify complement */
    uint8_t complement = 0;
    for(uint32_t i = 0x134; i < 0x14D; i++) {
        complement += data[i];
    }
    complement += 0x19;
    complement = 0x00 - complement;
    if(complement != header->complement) {
        fprintf(stderr, "cartridge error: complement mismatch\n");
        free(data);
        return -1;
    }

    /* Verify checksum? */
    uint16_t sum = 0;
    for(uint32_t i = 0; i < size; i++) {
        if(i == 0x014E || i == 0x014F) {
            continue;
        }
        sum += data[i];
    }

    printf("Cartridge header:\n");
    printf("start addr:     0x%04X\n", header->start_addr);
    printf("title:          %s\n", header->title);
    printf("game code:      0x%08X\n", header->game_code);
    printf("CGB:            0x%02X\n", header->support_code);
    printf("marker:         0x%04X\n", header->marker_code);
    printf("SGB:            0x%02X\n", header->sgb_support_code);
    printf("type:           %d\n", header->type);
    printf("ROM size:       %d\n", header->rom_size);
    printf("RAM size:       %d\n", header->ram_size);
    printf("dest:           %d\n", header->dest_code);
    printf("license code:   0x%02X\n", header->licence_code);
    printf("mask ROM:       0x%02X\n", header->mask_rom_version);
    printf("complement:     0x%02X\n", header->complement);
    printf("calc compl:     0x%02X\n", complement);
    printf("checksum:       0x%04X\n", (header->sum_hi << 8) | header->sum_lo);
    printf("calc sum:       0x%04X\n", sum);

    cartridge->data = data;
    cartridge->size = size;

    return 0;
}

void cartridge_cleanup(struct cartridge *cartridge) {
    free(cartridge->data);
}
