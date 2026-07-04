#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vault.h"
#include "crypto_store.h"
#include "magma.h"

#define MAXLINE 1024

int user_exists(const char *username) {
    FILE *f = fopen(USERS_FILE, "r");
    if (!f) return 0;
    char line[MAXLINE];
    while (fgets(line, sizeof(line), f)) {
        char *u = strtok(line, ":");
        if (u && strcmp(u, username) == 0) { fclose(f); return 1; }
    }
    fclose(f);
    return 0;
}

int register_user(const char *username, const char *password) {
    if (user_exists(username)) return -1;
    char hex[129];
    hash_password_hex(password, hex);
    FILE *f = fopen(USERS_FILE, "a");
    if (!f) return -1;
    fprintf(f, "%s:%s\n", username, hex);
    fclose(f);
    return 0;
}

int verify_login(const char *username, const char *password) {
    FILE *f = fopen(USERS_FILE, "r");
    if (!f) return 0;
    char want[129];
    hash_password_hex(password, want);
    char line[MAXLINE];
    int ok = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0';
        char *u = strtok(line, ":");
        char *h = strtok(NULL, ":");
        if (u && h && strcmp(u, username) == 0 && strcmp(h, want) == 0) { ok = 1; break; }
    }
    fclose(f);
    return ok;
}

void start_session(const char *master_password) {
    uint8_t key[32];
    derive_key(master_password, key);
    GOST_Magma_Expand_Key(key);
}

void add_entry(const char *username, const char *service,
               const char *login, const char *password) {
    char enc[600];
    encrypt_password(password, enc);
    FILE *f = fopen(VAULT_FILE, "a");
    if (!f) return;
    fprintf(f, "%s:%s:%s:%s\n", username, service, login, enc);
    fclose(f);
}

void list_entries(const char *username,
                  void (*cb)(const char*, const char*, const char*, void*),
                  void *ctx) {
    FILE *f = fopen(VAULT_FILE, "r");
    if (!f) return;
    char line[MAXLINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0';
        char *u   = strtok(line, ":");
        char *svc = strtok(NULL, ":");
        char *log = strtok(NULL, ":");
        char *hex = strtok(NULL, ":");
        if (u && svc && log && hex && strcmp(u, username) == 0) {
            char pw[600];
            decrypt_password(hex, pw);
            cb(svc, log, pw, ctx);
        }
    }
    fclose(f);
}
