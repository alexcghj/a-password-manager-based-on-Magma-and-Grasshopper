#ifndef MAGMA_H
#define MAGMA_H
#include <stdint.h>

/* GOST R 34.12-2015 "Magma": 64-bit block cipher, 256-bit key.
 * Verified against the official test vector (Appendix A.2). */

void GOST_Magma_Expand_Key(const uint8_t *key);              /* key: 32 bytes  */
void GOST_Magma_Encript(const uint8_t *blk, uint8_t *out);   /* blk/out: 8 bytes */
void GOST_Magma_Decript(const uint8_t *blk, uint8_t *out);   /* blk/out: 8 bytes */

#endif
