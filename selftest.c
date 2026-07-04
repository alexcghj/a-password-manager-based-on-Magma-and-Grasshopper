#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "magma.h"
#include "crypto_store.h"

int main(void) {
    int ok = 1;

    /* 1) Official GOST R 34.12-2015 Magma test vector (Appendix A.2) */
    uint8_t key[32] = {
        0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
        0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};
    uint8_t pt[8]  = {0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
    uint8_t exp[8] = {0x4e,0xe9,0x01,0xe5,0xc2,0xd8,0xca,0x3d};
    uint8_t ct[8];
    GOST_Magma_Expand_Key(key);
    GOST_Magma_Encript(pt, ct);
    int vec = memcmp(ct, exp, 8) == 0; ok &= vec;
    printf("Magma GOST test vector : %s\n", vec ? "PASS" : "FAIL");

    /* 2) Password encrypt/decrypt round-trips (reuses the key above) */
    const char *pws[] = {"hunter2","p@ssw0rd!","a-16-char-passwd","x"};
    for (int i = 0; i < 4; i++) {
        char hex[600], back[600];
        encrypt_password(pws[i], hex);
        decrypt_password(hex, back);
        int rt = strcmp(pws[i], back) == 0; ok &= rt;
        printf("round-trip %-16s: %s\n", pws[i], rt ? "PASS" : "FAIL");
    }
    printf("\n%s\n", ok ? "ALL TESTS PASS" : "SOME TESTS FAILED");
    return ok ? 0 : 1;
}
