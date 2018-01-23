#ifndef GBOY_CARTRIDGE_H
#define GBOY_CARTRIDGE_H

#include <inttypes.h>

struct cartridge_header {
    uint8_t     const_nop;
    uint8_t     const_jp;
    uint16_t    start_addr;
    uint8_t     logo[0x30];
    char        title[0x0B];
    uint32_t    game_code;
    uint8_t     support_code;
    uint16_t    marker_code;
    uint8_t     sgb_support_code;
    uint8_t     type;
    uint8_t     rom_size;
    uint8_t     ram_size;
    uint8_t     dest_code;
    uint8_t     licence_code;
    uint8_t     mask_rom_version;
    uint8_t     complement;
    uint8_t     sum_hi;
    uint8_t     sum_lo;
} __attribute__ ((packed));

struct cartridge {
    char        *data;
    uint32_t    size;
};

int     cartridge_open(struct cartridge *cartridge, const char *path);
void    cartridge_cleanup(struct cartridge *cartridge);

#endif
