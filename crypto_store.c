#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "magma.h"
#include "crypto_store.h"
#include "stribog.h"          /* your existing Streebog implementation */

void hash_password_hex(const char *password, char *hex_out) {
    StribogContext ctx;
    stribog_init(&ctx, 512);
    stribog_update(&ctx, (const uint8_t *)password, strlen(password));
    stribog_final(&ctx);
    for (int i = 0; i < 64; i++) sprintf(hex_out + 2*i, "%02x", ctx.hash[i]);
    hex_out[128] = '\0';
}

void derive_key(const char *master_password, uint8_t key[32]) {
    /* Domain tag "|enc" so this key differs from the auth hash you already
     * store for login. (For real use, add a random per-user salt + a KDF.) */
    char buf[300];
    snprintf(buf, sizeof(buf), "%s|enc", master_password);

    StribogContext ctx;
    stribog_init(&ctx, 512);
    stribog_update(&ctx, (const uint8_t *)buf, strlen(buf));
    stribog_final(&ctx);
    memcpy(key, ctx.hash, 32); /* first 256 bits of the 512-bit digest */
}

static void bytes_to_hex(const uint8_t *in, int len, char *out) {
    for (int i = 0; i < len; i++) sprintf(out + 2*i, "%02x", in[i]);
    out[2*len] = '\0';
}
static int hex_to_bytes(const char *hex, uint8_t *out) {
    int n = (int)strlen(hex) / 2;
    for (int i = 0; i < n; i++) { unsigned v; sscanf(hex + 2*i, "%2x", &v); out[i] = (uint8_t)v; }
    return n;
}

void encrypt_password(const char *password, char *hex_out) {
    uint8_t buf[256]; memset(buf, 0, sizeof(buf));
    size_t len = strlen(password);
    if (len > 248) len = 248;                 /* keep within buffer */
    int padded = (int)(((len + 7) / 8) * 8); if (padded == 0) padded = 8;
    memcpy(buf, password, len);

    uint8_t ct[256];
    for (int off = 0; off < padded; off += 8)
        GOST_Magma_Encript(buf + off, ct + off);
    bytes_to_hex(ct, padded, hex_out);
}

void decrypt_password(const char *hex, char *password_out) {
    uint8_t ct[256];
    int n = hex_to_bytes(hex, ct);
    uint8_t pt[256]; memset(pt, 0, sizeof(pt));
    for (int off = 0; off < n; off += 8)
        GOST_Magma_Decript(ct + off, pt + off);
    pt[n] = '\0';
    strcpy(password_out, (char *)pt);         /* trailing zero-pad is stripped */
}
