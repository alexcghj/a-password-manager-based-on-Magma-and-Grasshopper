#ifndef CRYPTO_STORE_H
#define CRYPTO_STORE_H
#include <stdint.h>

/* Streebog-512 hash of the password as a 128-char hex string (for login/auth).
 * Use this in BOTH registration and login so the two sides match. */
void hash_password_hex(const char *password, char *hex_out);

/* Derive a 256-bit Magma key from the master password (via Streebog).
 * Call GOST_Magma_Expand_Key(key) once afterwards, before encrypt/decrypt. */
void derive_key(const char *master_password, uint8_t key[32]);

/* Encrypt a password string with Magma (ECB + zero padding) -> null-terminated hex. */
void encrypt_password(const char *password, char *hex_out);

/* Inverse of encrypt_password. */
void decrypt_password(const char *hex, char *password_out);

#endif
