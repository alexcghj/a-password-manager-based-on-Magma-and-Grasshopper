#include <stdio.h>
#include <string.h>
#include "vault.h"

static int checks = 0, fails = 0;
#define CHECK(cond, name) do { checks++; \
    if (cond) printf("  PASS  %s\n", name); \
    else { fails++; printf("  FAIL  %s\n", name); } } while (0)

static char found_pw[600];
static int  found = 0;
static void collect(const char *svc, const char *login, const char *pw, void *ctx) {
    (void)login; (void)ctx;
    if (strcmp(svc, "github") == 0) { strcpy(found_pw, pw); found = 1; }
}

int main(void) {
    remove(USERS_FILE); remove(VAULT_FILE);

    printf("Auth:\n");
    CHECK(register_user("alice", "wonderland") == 0, "register new user");
    CHECK(register_user("alice", "again")     == -1, "duplicate username rejected");
    CHECK(verify_login("alice", "wonderland") == 1, "correct password accepted");
    CHECK(verify_login("alice", "wrong")      == 0, "wrong password rejected");
    CHECK(verify_login("bob",   "wonderland") == 0, "unknown user rejected");

    printf("Vault:\n");
    start_session("wonderland");
    add_entry("alice", "github", "alice@dev", "s3cr3t-p@ss!");
    found = 0;
    list_entries("alice", collect, NULL);
    CHECK(found == 1, "entry stored and listed");
    CHECK(found && strcmp(found_pw, "s3cr3t-p@ss!") == 0, "password decrypts to original");

    remove(USERS_FILE); remove(VAULT_FILE);
    printf("\n%s  (%d checks, %d failed)\n", fails ? "FAILURES" : "ALL PASS", checks, fails);
    return fails ? 1 : 0;
}
