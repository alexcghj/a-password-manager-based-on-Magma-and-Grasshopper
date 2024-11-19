#include </home/mnt/hgfs/stribog.h>
#include <string.h>
#include "stribog_const.h"

void stribog_init(StribogContext *context, uint16_t hash_size)
{
    memset(context, 0x00, sizeof(StribogContext));
    if (hash_size == 256)
        memset(context->h, 0x01, BLOCK_SIZE);
    else
        memset(context->h, 0x00, BLOCK_SIZE);
    context->hash_size = hash_size;
    context->v_512[1] = 0x02;
}
void stribog_update(StribogContext *context, const uint8_t *data, size_t len)
{
    size_t chk_size;

    while((len > 63) && (context->buf_size) == 0)
    {
        stribog_Stage(context, data);
        data += 64;
        len -= 64;
    }
    while (len)
    {
        chk_size = 64 - context->buf_size;
        if (chk_size > len)
            chk_size = len;
        memcpy(&context->buffer[context->buf_size], data, chk_size);
        context->buf_size += chk_size;
        len -= chk_size;
        data += chk_size;
        if (context->buf_size == 64)
        {
            stribog_Stage(context, context->buffer);
            context->buf_size = 0;
        }
    }
}
void stribog_final(StribogContext *context)
{
    stribog_stage_2(context);
    context->buf_size = 0;
}
void stribog_stage_2(StribogContext *context)
{
    vect internal;
    memset(internal, 0x00, BLOCK_SIZE);
    internal[1] = ((context->buf_size * 8) >> 8) & 0xff;
    internal[0] = (context->buf_size * 8) & 0xff;

    stribog_padding(context);

    stribog_hash_G(context->h, context->N, (uint8_t*)&(context->buffer));

    stribog_hash_add512(context->N, internal, context->N);
    stribog_hash_add512(context->Sigma, context->buffer, context->Sigma);

    stribog_hash_G(context->h, context->v_0, (uint8_t*)&(context->N));
    stribog_hash_G(context->h, context->v_0, (uint8_t*)&(context->Sigma));

    memcpy(context->hash, context->h, BLOCK_SIZE);
}
void stribog_padding(StribogContext *context)
{
    vect internal;

    if (context->buf_size < BLOCK_SIZE)
    {
        memset(internal, 0x00, BLOCK_SIZE);
        memcpy(internal, context->buffer, context->buf_size);
        internal[context->buf_size] = 0x01;
        memcpy(context->buffer, internal, BLOCK_SIZE);
    }
}
void stribog_Stage(StribogContext *context, const uint8_t *data)
{
    stribog_hash_G(context->h, context->N, data);
    stribog_hash_add512(context->N, context->v_512, context->N);
    stribog_hash_add512(context->Sigma, data, context->Sigma);
}
void stribog_hash_add512(const uint8_t *a, const uint8_t *b, uint8_t *c)
{
    int i;
    int internal = 0;
    for (i = 0; i < 64; i++)
    {
        internal = a[i] + b[i] + (internal >> 8);
        c[i] = internal & 0xff;
    }
}
void stribog_hash_G( uint8_t *h, uint8_t *N, const uint8_t *m)
{
    vect K, internal;
    stribog_hash_X(N, h, K);

    stribog_hash_S(K);
    stribog_hash_P(K);
    stribog_hash_L(K);

    stribog_hash_E(K, m, internal);

    stribog_hash_X(internal, h, internal);
    stribog_hash_X(internal, m, h);
}
void stribog_hash_X(const uint8_t *a, const uint8_t *b, uint8_t *c)
{
    int i;
    for (i = 0; i < 64; i++)
        c[i] = a[i]^b[i];
}
void stribog_hash_P(uint8_t *state)
{
    int i;
    vect internal;
    for (i = 63; i >= 0; i--)
        internal[i] = state[Tau[i]];
    memcpy(state, internal, BLOCK_SIZE);
}

void stribog_hash_S(uint8_t *state)
{
    int i;
    vect internal;
    for (i = 63; i >= 0; i--)
        internal[i] = Pi[state[i]];
    memcpy(state, internal, BLOCK_SIZE);
}

void stribog_hash_L(uint8_t *state)
{
    uint64_t* internal_in = (uint64_t*)state;
    uint64_t internal_out[8];
    memset(internal_out, 0x00, BLOCK_SIZE);
    int i, j;
    for (i = 7; i >= 0; i--)
    {
        for (j = 63; j >= 0; j--)
            if ((internal_in[i] >> j) & 1)
                internal_out[i] ^= A[63 - j];
    }
    memcpy(state, internal_out, 64);
}
void stribog_hash_E(uint8_t *K, const uint8_t *m, uint8_t *state)
{
    int i;
    memcpy(K, m, BLOCK_SIZE);
    stribog_hash_X(m, K, state);
    for(i = 0; i < 12; i++)
    {
        stribog_hash_S(state);
        stribog_hash_P(state);
        stribog_hash_L(state);
        stribog_hash_get_key(K, i);
        stribog_hash_X(state, K, state);
    }
}
void stribog_hash_get_key(uint8_t *K, int i)
{
    stribog_hash_X(K, C[i], K);
    stribog_hash_S(K);
    stribog_hash_P(K);
    stribog_hash_L(K);
}
