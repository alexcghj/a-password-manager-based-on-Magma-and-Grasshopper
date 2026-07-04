# GOST Password Manager (Magma + Streebog)

A terminal password manager in C built on **from-scratch implementations of the Russian GOST cryptographic standards**. Account passwords are hashed with **Streebog** (GOST R 34.11-2012); stored credentials are encrypted with the **Magma** block cipher (GOST R 34.12-2015). The Magma implementation is verified against the official test vector from the standard.

> University information-security project by **Alexander Kuleshov** and **Alexey Siotanov**.

## What works
- Registration and login — account passwords are stored only as Streebog-512 hashes
- Per-user encrypted vault — each stored password is encrypted with Magma and shown decrypted only after login
- Clean `ncurses` terminal UI — masked password input, arrow-key menus
- Test suite — Magma checked against the GOST test vector, plus end-to-end vault round-trips

## Project layout
```
magma.c / magma.h                Magma block cipher (GOST R 34.12-2015), verified
crypto_store.c / .h              key derivation, password encrypt/decrypt, auth hashing
vault.c / vault.h                accounts + encrypted entries (file-backed)
Pass.c                           ncurses user interface
stribog.c / .h, stribog_const.h  Streebog hash (GOST R 34.11-2012) — your existing files
selftest.c                       Magma test vector + storage round-trips
vault_test.c                     register / login / add / view logic tests
Makefile
```

## Build & run
Requires a C compiler and ncurses (`sudo apt install libncurses-dev` on Debian/Ubuntu;
MinGW-w64 + ncurses on Windows).
```bash
make          # builds ./passman
./passman
make test     # builds and runs both test suites
```

## How encryption works
On login the master password is hashed with Streebog and the digest becomes the 256-bit Magma key, kept only in memory. Each stored password is padded to Magma's 8-byte block, encrypted, and saved as hex in `vault.txt`; viewing reverses this. Because the key comes from the master password, entries can only be read by the account that created them.

## Security scope
This is an educational project and is **not intended for storing real passwords**. Current simplifications: ECB mode (no chaining/IV), no per-user salt, no dedicated key-stretching KDF, and no secure wiping of secrets in memory. A production version would use CBC/CTR, a random salt, and a proper KDF.

## Roadmap
- Add **Kuznyechik** (GOST R 34.12-2015, 128-bit block) as a second cipher, verified the same way
- CBC/CTR mode, random salt, KDF; edit/delete entries

## Credits
Developed by **Alexander Kuleshov** and **Alexey Siotanov**.
