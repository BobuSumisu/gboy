#ifndef GBOY_BITUTIL_H
#define GBOY_BITUTIL_H

#define BIT_GET(b, n)       (((b) >> (n)) & 1)
#define BIT_TEST(b, n)      (((b) & (1 << (n))) != 0)
#define BIT_SET(b, n)       ((b) |= (1 << (n)))
#define BIT_RESET(b, n)     ((b) &= ~(1 << (n)))
#define BIT_TOGGLE(b, n)    ((b) ^= (1 << (n)))

#endif
