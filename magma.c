#include <stdint.h>
#include <string.h>
#include "magma.h"

/* Round keys. Private to this file. */
static uint8_t iter_key[32][4];

static void GOST_Magma_Add(const uint8_t *a, const uint8_t *b, uint8_t *c) {
    for (int i = 0; i < 4; i++) c[i] = a[i] ^ b[i];
}
static void GOST_Magma_Add_32(const uint8_t *a, const uint8_t *b, uint8_t *c) {
    unsigned int t = 0;
    for (int i = 3; i >= 0; i--) { t = a[i] + b[i] + (t >> 8); c[i] = t & 0xff; }
}
static unsigned char Pi[8][16] = {
    {1,7,14,13,0,5,8,3,4,15,10,6,9,12,11,2},
    {8,14,2,5,6,9,1,12,15,4,11,0,13,10,3,7},
    {5,13,15,6,9,2,12,10,11,7,8,1,4,3,14,0},
    {7,15,5,10,8,1,6,13,0,9,3,14,11,4,2,12},
    {12,8,2,1,13,4,15,6,7,0,10,5,3,14,9,11},
    {11,3,5,8,2,15,10,13,14,1,7,4,12,9,6,0},
    {6,8,2,3,9,10,5,12,1,14,4,7,11,13,0,15},
    {12,4,6,2,10,5,11,9,14,8,13,7,0,3,15,1}
};
static void GOST_Magma_T(const uint8_t *in, uint8_t *out) {
    uint8_t hi, lo;
    for (int i = 0; i < 4; i++) {
        hi = (in[i] & 0xf0) >> 4;  lo = in[i] & 0x0f;
        hi = Pi[i*2][hi];          lo = Pi[i*2+1][lo];
        out[i] = (hi << 4) | lo;
    }
}

/* FIX: the original filled round keys 0-7 three times and left 8-23
 * uninitialised, so rounds 9-24 used garbage. Correct schedule:
 * K1..K8 three times, then K8..K1. */
void GOST_Magma_Expand_Key(const uint8_t *key) {
    for (int i = 0; i < 8; i++) {
        memcpy(iter_key[i],      key + 4*i,       4);   /* rounds 1-8   */
        memcpy(iter_key[8 + i],  key + 4*i,       4);   /* rounds 9-16  */
        memcpy(iter_key[16 + i], key + 4*i,       4);   /* rounds 17-24 */
        memcpy(iter_key[24 + i], key + 4*(7 - i), 4);   /* rounds 25-32 */
    }
}

static void GOST_Magma_g(const uint8_t *k, const uint8_t *a, uint8_t *out) {
    uint8_t in[4]; uint32_t v;
    GOST_Magma_Add_32(a, k, in);
    GOST_Magma_T(in, in);
    v = in[0]; v = (v<<8)+in[1]; v = (v<<8)+in[2]; v = (v<<8)+in[3];
    v = (v << 11) | (v >> 21);
    out[3] = v; out[2] = v>>8; out[1] = v>>16; out[0] = v>>24;
}
static void GOST_Magma_G(const uint8_t *k, const uint8_t *a, uint8_t *out) {
    uint8_t a0[4], a1[4], G[4];
    for (int i=0;i<4;i++){ a0[i]=a[4+i]; a1[i]=a[i]; }
    GOST_Magma_g(k, a0, G);
    GOST_Magma_Add(a1, G, G);
    for (int i=0;i<4;i++){ a1[i]=a0[i]; a0[i]=G[i]; }
    for (int i=0;i<4;i++){ out[i]=a1[i]; out[4+i]=a0[i]; }
}
static void GOST_Magma_G_Fin(const uint8_t *k, const uint8_t *a, uint8_t *out) {
    uint8_t a0[4], a1[4], G[4];
    for (int i=0;i<4;i++){ a0[i]=a[4+i]; a1[i]=a[i]; }
    GOST_Magma_g(k, a0, G);
    GOST_Magma_Add(a1, G, G);
    for (int i=0;i<4;i++) a1[i]=G[i];
    for (int i=0;i<4;i++){ out[i]=a1[i]; out[4+i]=a0[i]; }
}
void GOST_Magma_Encript(const uint8_t *blk, uint8_t *out) {
    GOST_Magma_G(iter_key[0], blk, out);
    for (int i=1;i<31;i++) GOST_Magma_G(iter_key[i], out, out);
    GOST_Magma_G_Fin(iter_key[31], out, out);
}
void GOST_Magma_Decript(const uint8_t *blk, uint8_t *out) {
    GOST_Magma_G(iter_key[31], blk, out);
    for (int i=30;i>0;i--) GOST_Magma_G(iter_key[i], out, out);
    GOST_Magma_G_Fin(iter_key[0], out, out);
}
