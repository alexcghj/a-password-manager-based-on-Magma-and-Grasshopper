CC     = gcc
CFLAGS = -O2 -Wall
LIBS   = -lncurses

# Requires your existing Streebog files: stribog.c, stribog.h, stribog_const.h
CORE = magma.c crypto_store.c vault.c stribog.c

passman: Pass.c $(CORE)
	$(CC) $(CFLAGS) -o $@ Pass.c $(CORE) $(LIBS)

selftest: selftest.c magma.c crypto_store.c stribog.c
	$(CC) $(CFLAGS) -o $@ $^

vaulttest: vault_test.c vault.c magma.c crypto_store.c stribog.c
	$(CC) $(CFLAGS) -o $@ $^

test: selftest vaulttest
	./selftest && ./vaulttest

clean:
	rm -f passman selftest vaulttest *.o users.txt vault.txt

.PHONY: test clean
