#ifndef stribog_h
#define stribog_h

#include <stdfix.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "stribog_const.h"

#define BLOCK_SIZE 64

typedef uint8_t vect[BLOCK_SIZE];

typedef struct TStribogContext
{
    vect buffer;
    vect hash;
    vect h;
    vect N;
    vect Sigma;
    vect v_0;
    vect v_512;
    size_t buf_size;
    int hash_size;
} StribogContext;

void stribog_init(StribogContext *Context, uint16_t hash_size);

void stribog_update(StribogContext *Context, const uint8_t *data, size_t len);

void stribog_final(StribogContext *Context);

void stribog_Stage(StribogContext *context, const uint8_t *data);
void stribog_hash_add512(const uint8_t *a, const uint8_t *b, uint8_t *c);
void stribog_hash_G(uint8_t *h, uint8_t *N, const uint8_t *m);
void stribog_hash_X(const uint8_t *a, const uint8_t *b, uint8_t *c);
void stribog_hash_P(uint8_t *state);
void stribog_padding(StribogContext *context);
void stribog_stage_2(StribogContext *context);
void stribog_hash_S(uint8_t *state);
void stribog_hash_L(uint8_t *state);
void stribog_hash_E(uint8_t *K, const uint8_t *m, uint8_t *state);
void stribog_hash_get_key(uint8_t *K, int i);

#endif // stribog_h
