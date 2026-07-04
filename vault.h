#ifndef VAULT_H
#define VAULT_H
/* File-backed accounts and encrypted credential entries.
 * Storage:
 *   users.txt : username:streebog512hex(password)
 *   vault.txt : username:service:login:magma_hex(password)
 * Depends on crypto_store (hashing, key derivation, encrypt/decrypt). */

#define USERS_FILE "users.txt"
#define VAULT_FILE "vault.txt"

int  user_exists(const char *username);
int  register_user(const char *username, const char *password); /* 0 ok, -1 exists */
int  verify_login(const char *username, const char *password);  /* 1 ok, 0 fail   */

void start_session(const char *master_password);               /* derive+expand key */
void add_entry(const char *username, const char *service,
               const char *login, const char *password);
void list_entries(const char *username,
                  void (*cb)(const char *service, const char *login,
                             const char *password, void *ctx),
                  void *ctx);
#endif
