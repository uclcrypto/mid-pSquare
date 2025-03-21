#include <stdint.h>

#define ROUNDS 40

#define SWAPMOVE(a, b, mask, n)                               \
{                                                             \
  tmp = (b ^ (a >> n)) & mask;                                \
  b ^= tmp;                                                   \
  a ^= (tmp << n);                                            \
}

#define QUADRUPLE_ROUND(state, tk)                            \
{                                                             \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  state[1] ^= (state[2] | state[3]);                          \
  SWAPMOVE(state[3], state[0], 0x55555555, 0);                \
  state[0] ^= (tk)[0];                                        \
  state[1] ^= (tk)[1];                                        \
  state[2] ^= (tk)[2];                                        \
  state[3] ^= (tk)[3];                                        \
  mcolumns_0(state);                                          \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  state[3] ^= (state[0] | state[1]);                          \
  SWAPMOVE(state[1], state[2], 0x55555555, 0);                \
  state[0] ^= (tk)[4];                                        \
  state[1] ^= (tk)[5];                                        \
  state[2] ^= (tk)[6];                                        \
  state[3] ^= (tk)[7];                                        \
  mcolumns_1(state);                                          \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  state[1] ^= (state[2] | state[3]);                          \
  SWAPMOVE(state[3], state[0], 0x55555555, 0);                \
  state[0] ^= (tk)[8];                                        \
  state[1] ^= (tk)[9];                                        \
  state[2] ^= (tk)[10];                                       \
  state[3] ^= (tk)[11];                                       \
  mcolumns_2(state);                                          \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  state[3] ^= (state[0] | state[1]);                          \
  SWAPMOVE(state[1], state[2], 0x55555555, 0);                \
  state[0] ^= (tk)[12];                                       \
  state[1] ^= (tk)[13];                                       \
  state[2] ^= (tk)[14];                                       \
  state[3] ^= (tk)[15];                                       \
  mcolumns_3(state);                                          \
}

#define INV_QUADRUPLE_ROUND(state, tk)                        \
{                                                             \
  inv_mcolumns_3(state);                                      \
  state[0] ^= (tk)[12];                                       \
  state[1] ^= (tk)[13];                                       \
  state[2] ^= (tk)[14];                                       \
  state[3] ^= (tk)[15];                                       \
  SWAPMOVE(state[1], state[2], 0x55555555, 0);                \
  state[3] ^= (state[0] | state[1]);                          \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  inv_mcolumns_2(state);                                      \
  state[0] ^= (tk)[8];                                        \
  state[1] ^= (tk)[9];                                        \
  state[2] ^= (tk)[10];                                       \
  state[3] ^= (tk)[11];                                       \
  SWAPMOVE(state[3], state[0], 0x55555555, 0);                \
  state[1] ^= (state[2] | state[3]);                          \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
  inv_mcolumns_1(state);                                      \
  state[0] ^= (tk)[4];                                        \
  state[1] ^= (tk)[5];                                        \
  state[2] ^= (tk)[6];                                        \
  state[3] ^= (tk)[7];                                        \
  SWAPMOVE(state[1], state[2], 0x55555555, 0);                \
  state[3] ^= (state[0] | state[1]);                          \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  inv_mcolumns_0(state);                                      \
  state[0] ^= (tk)[0];                                        \
  state[1] ^= (tk)[1];                                        \
  state[2] ^= (tk)[2];                                        \
  state[3] ^= (tk)[3];                                        \
  SWAPMOVE(state[3], state[0], 0x55555555, 0);                \
  state[1] ^= (state[2] | state[3]);                          \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
  SWAPMOVE(state[0], state[3], 0x55555555, 1);                \
  SWAPMOVE(state[1], state[0], 0x55555555, 1);                \
  state[1] ^= ~(state[2] | state[3]);                         \
  SWAPMOVE(state[3], state[2], 0x55555555, 1);                \
  SWAPMOVE(state[2], state[1], 0x55555555, 1);                \
  state[3] ^= ~(state[0] | state[1]);                         \
}

#define ROR(x, y) (((x) >> (y)) | ((x) << (32 - (y))))

#define LE_LOAD(x, y)                                         \
  *(x) = (((uint32_t)(y)[3] << 24) |                          \
          ((uint32_t)(y)[2] << 16) |                          \
          ((uint32_t)(y)[1] << 8) |                           \
          (y)[0]);

#define LE_STORE(x, y)                                        \
  (x)[0] = (y) & 0xff;                                        \
  (x)[1] = ((y) >> 8) & 0xff;                                 \
  (x)[2] = ((y) >> 16) & 0xff;                                \
  (x)[3] = (y) >> 24;

void packing(uint32_t* out, const uint8_t* in);
void unpacking(uint8_t* out, uint32_t *in);
inline void permute_tk_2(uint32_t* tk);
inline void permute_tk_4(uint32_t* tk);
inline void permute_tk_6(uint32_t* tk);
inline void permute_tk_8(uint32_t* tk);
inline void permute_tk_10(uint32_t* tk);
inline void permute_tk_12(uint32_t* tk);
inline void permute_tk_14(uint32_t* tk);
void permute_tk(uint32_t* tk, const uint8_t* key);
void skinny128_tk(uint32_t* rtk, const uint8_t* tk);
void mcolumns_0(uint32_t* state);
void mcolumns_1(uint32_t* state);
void mcolumns_2(uint32_t* state);
void mcolumns_3(uint32_t* state);
void inv_mcolumns_0(uint32_t* state);
void inv_mcolumns_1(uint32_t* state);
void inv_mcolumns_2(uint32_t* state);
void inv_mcolumns_3(uint32_t* state);
void __attribute__((noinline)) skinny128_encrypt(uint8_t* ctext, const uint8_t* ptext, const uint8_t* tk);
void __attribute__((noinline)) skinny128_decrypt(uint8_t* ptext, const uint8_t* ctext, const uint8_t* tk);

static uint32_t rconst_32_bs[224] = {
  0x00000004, 0xffffffbf, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x10000100, 0xfffffeff, 0x44000000, 0xfbffffff, 0x00000000, 0x04000000,
  0x00100000, 0x00100000, 0x00100001, 0xffefffff, 0x00440000, 0xffafffff,
  0x00400000, 0x00400000, 0x01000000, 0x01000000, 0x01401000, 0xffbfffff,
  0x01004000, 0xfefffbff, 0x00000400, 0x00000400, 0x00000010, 0x00000000,
  0x00010410, 0xfffffbef, 0x00000054, 0xffffffaf, 0x00000000, 0x00000040,
  0x00000100, 0x00000100, 0x10000140, 0xfffffeff, 0x44000000, 0xfffffeff,
  0x04000000, 0x04000000, 0x00100000, 0x00100000, 0x04000001, 0xfbffffff,
  0x00140000, 0xffafffff, 0x00400000, 0x00000000, 0x00000000, 0x00000000,
  0x01401000, 0xfebfffff, 0x01004400, 0xfffffbff, 0x00000000, 0x00000400,
  0x00000010, 0x00000010, 0x00010010, 0xffffffff, 0x00000004, 0xffffffaf,
  0x00000040, 0x00000040, 0x00000100, 0x00000000, 0x10000140, 0xffffffbf,
  0x40000100, 0xfbfffeff, 0x00000000, 0x04000000, 0x00100000, 0x00000000,
  0x04100001, 0xffefffff, 0x00440000, 0xffefffff, 0x00000000, 0x00400000,
  0x01000000, 0x01000000, 0x00401000, 0xffffffff, 0x00004000, 0xfeffffff, 
  0x00000400, 0x00000000, 0x00000000, 0x00000000, 0x00010400, 0xfffffbff,
  0x00000014, 0xffffffbf, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x10000100, 0xffffffff, 0x40000000, 0xfbffffff, 0x00000000, 0x04000000,
  0x00100000, 0x00000000, 0x00100001, 0xffefffff, 0x00440000, 0xffafffff,
  0x00000000, 0x00400000, 0x01000000, 0x01000000, 0x01401000, 0xffffffff,
  0x00004000, 0xfeffffff, 0x00000400, 0x00000400, 0x00000010, 0x00000000,
  0x00010400, 0xfffffbff, 0x00000014, 0xffffffaf, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x10000140, 0xfffffeff, 0x44000000, 0xffffffff,
  0x00000000, 0x04000000, 0x00100000, 0x00100000, 0x00000001, 0xffefffff,
  0x00440000, 0xffafffff, 0x00400000, 0x00000000, 0x00000000, 0x01000000,
  0x01401000, 0xffbfffff, 0x01004000, 0xfffffbff, 0x00000400, 0x00000400,
  0x00000010, 0x00000000, 0x00010010, 0xfffffbff, 0x00000014, 0xffffffef,
  0x00000000, 0x00000040, 0x00000100, 0x00000000, 0x10000040, 0xfffffeff,
  0x44000000, 0xfffffeff, 0x00000000, 0x00000000, 0x00000000, 0x00100000,
  0x04000001, 0xffffffff, 0x00040000, 0xffffffff, 0x00400000, 0x00000000,
  0x00000000, 0x00000000, 0x00001000, 0xfebfffff, 0x01004400, 0xffffffff,
  0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00010000, 0xffffffff,
  0x00000004, 0xffffffbf, 0x00000040, 0x00000000, 0x00000000, 0x00000000,
  0x10000100, 0xfffffebf, 0x44000100, 0xffffffff, 0x00000000, 0x04000000,
  0x00100000, 0x00100000, 0x00000001, 0xffffffff, 0x00040000, 0xffafffff,
  0x00400000, 0x00000000, 0x00000000, 0x00000000, 0x01401000, 0xffbfffff,
  0x01004000, 0xfffffbff, 0x00000000, 0x00000400, 0x00000010, 0x00000000,
  0x00010010, 0xffffffff
};

void packing(uint32_t* out, const uint8_t* in)
{
  uint32_t tmp;
  LE_LOAD(out, in)
  LE_LOAD(out + 1, in + 8)
  LE_LOAD(out + 2, in + 4)
  LE_LOAD(out + 3, in + 12)
  SWAPMOVE(out[0], out[0], 0x0a0a0a0a, 3)
  SWAPMOVE(out[1], out[1], 0x0a0a0a0a, 3)
  SWAPMOVE(out[2], out[2], 0x0a0a0a0a, 3)
  SWAPMOVE(out[3], out[3], 0x0a0a0a0a, 3)
  SWAPMOVE(out[2], out[0], 0x30303030, 2)
  SWAPMOVE(out[1], out[0], 0x0c0c0c0c, 4)
  SWAPMOVE(out[3], out[0], 0x03030303, 6)
  SWAPMOVE(out[1], out[2], 0x0c0c0c0c, 2)
  SWAPMOVE(out[3], out[2], 0x03030303, 4)
  SWAPMOVE(out[3], out[1], 0x03030303, 2)
}

void unpacking(uint8_t* out, uint32_t *in)
{
  uint32_t tmp;
  SWAPMOVE(in[3], in[1], 0x03030303, 2)
  SWAPMOVE(in[3], in[2], 0x03030303, 4)
  SWAPMOVE(in[1], in[2], 0x0c0c0c0c, 2)
  SWAPMOVE(in[3], in[0], 0x03030303, 6)
  SWAPMOVE(in[1], in[0], 0x0c0c0c0c, 4)
  SWAPMOVE(in[2], in[0], 0x30303030, 2)
  SWAPMOVE(in[0], in[0], 0x0a0a0a0a, 3)
  SWAPMOVE(in[1], in[1], 0x0a0a0a0a, 3)
  SWAPMOVE(in[2], in[2], 0x0a0a0a0a, 3)
  SWAPMOVE(in[3], in[3], 0x0a0a0a0a, 3)
  LE_STORE(out, in[0])
  LE_STORE(out + 8, in[1])
  LE_STORE(out + 4, in[2])
  LE_STORE(out + 12, in[3])
}

inline void permute_tk_2(uint32_t* tk)
{
  uint32_t tmp;
  for(int i =0; i < 4; i++)
  {
    tmp = tk[i];
    tk[i]  = ROR(tmp, 14) & 0xcc00cc00;
    tk[i] |= (tmp & 0x000000ff) << 16;
    tk[i] |= (tmp & 0xcc000000) >>  2;
    tk[i] |= (tmp & 0x0033cc00) >>  8;
    tk[i] |= (tmp & 0x00cc0000) >> 18;
  }
}

inline void permute_tk_4(uint32_t* tk)
{
  uint32_t tmp;
  for(int i =0; i < 4; i++)
  {
    tmp = tk[i];
    tk[i]  = ROR(tmp, 22) & 0xcc0000cc;
    tk[i] |= ROR(tmp, 16) & 0x3300cc00;
    tk[i] |= ROR(tmp, 24) & 0x00cc3300;
    tk[i] |= (tmp & 0x00cc00cc) >> 2;
  }
}

inline void permute_tk_6(uint32_t* tk)
{
  uint32_t tmp;
  for(int i =0; i < 4; i++)
  {
    tmp = tk[i];
    tk[i]  = ROR(tmp,  6) & 0xcccc0000;
    tk[i] |= ROR(tmp, 24) & 0x330000cc;
    tk[i] |= ROR(tmp, 10) & 0x00003333;
    tk[i] |= (tmp & 0xcc) << 14;
    tk[i] |= (tmp & 0x3300) << 2;
  }
}

inline void permute_tk_8(uint32_t* tk)
{
  uint32_t tmp;
  for(int i =0; i < 4; i++)
  {
    tmp = tk[i];
    tk[i]  = ROR(tmp, 24) & 0xcc000033;
    tk[i] |= ROR(tmp,  8) & 0x33cc0000;
    tk[i] |= ROR(tmp, 26) & 0x00333300;
    tk[i] |= (tmp & 0x00333300) >> 6;
  }
}

inline void permute_tk_10(uint32_t* tk)
{
  uint32_t tmp;
  for(int i =0; i < 4; i++)
  {
    tmp = tk[i];
    tk[i]  = ROR(tmp,  8) & 0xcc330000;
    tk[i] |= ROR(tmp, 26) & 0x33000033;
    tk[i] |= ROR(tmp, 22) & 0x00cccc00;
    tk[i] |= (tmp & 0x00330000) >> 14;
    tk[i] |= (tmp & 0xcc00) >> 2;
  }
}

inline void permute_tk_12(uint32_t* tk)
{
  uint32_t tmp;
  for(int i =0; i < 4; i++)
  {
    tmp = tk[i];
    tk[i]  = ROR(tmp,  8) & 0x0000cc33;
    tk[i] |= ROR(tmp, 30) & 0x00cc00cc;
    tk[i] |= ROR(tmp, 10) & 0x33330000;
    tk[i] |= ROR(tmp, 16) & 0xcc003300;
  }
}

inline void permute_tk_14(uint32_t* tk)
{
  uint32_t tmp;
  for(int i =0; i < 4; i++)
  {
    tmp = tk[i];
    tk[i]  = ROR(tmp, 24) & 0x0033cc00;
    tk[i] |= ROR(tmp, 14) & 0x00cc0000;
    tk[i] |= ROR(tmp, 30) & 0xcc000000;
    tk[i] |= ROR(tmp, 16) & 0x000000ff;
    tk[i] |= ROR(tmp, 18) & 0x33003300;
  }
}

void permute_tk(uint32_t* tk, const uint8_t* key)
{
  uint32_t test;
  uint32_t tk1[4], tmp[4];
  packing(tk1, key);
  tmp[0] = tk[0] ^ tk1[0];
  tmp[1] = tk[1] ^ tk1[1];
  tmp[2] = tk[2] ^ tk1[2];
  tmp[3] = tk[3] ^ tk1[3];
  for(int i = 0 ; i < ROUNDS; i += 8)
  {
    test = (i % 16 < 8) ? 1 : 0;
    tk[i*4]   = tmp[2] & 0xf0f0f0f0;
    tk[i*4+1] = tmp[3] & 0xf0f0f0f0;
    tk[i*4+2] = tmp[0] & 0xf0f0f0f0;
    tk[i*4+3] = tmp[1] & 0xf0f0f0f0;
    tmp[0] = tk[i*4+4] ^ tk1[0];
    tmp[1] = tk[i*4+5] ^ tk1[1];
    tmp[2] = tk[i*4+6] ^ tk1[2];
    tmp[3] = tk[i*4+7] ^ tk1[3];
    if (test)
      permute_tk_2(tmp);
    else
      permute_tk_10(tmp);
    tk[i*4+ 4]  = ROR(tmp[0], 26) & 0xc3c3c3c3;
    tk[i*4+ 5]  = ROR(tmp[1], 26) & 0xc3c3c3c3;
    tk[i*4+ 6]  = ROR(tmp[2], 26) & 0xc3c3c3c3;
    tk[i*4+ 7]  = ROR(tmp[3], 26) & 0xc3c3c3c3;
    tk[i*4+ 8]  = ROR(tmp[2], 28) & 0x03030303;
    tk[i*4+ 8] |= ROR(tmp[2], 12) & 0x0c0c0c0c;
    tk[i*4+ 9]  = ROR(tmp[3], 28) & 0x03030303;
    tk[i*4+ 9] |= ROR(tmp[3], 12) & 0x0c0c0c0c;
    tk[i*4+10]  = ROR(tmp[0], 28) & 0x03030303;
    tk[i*4+10] |= ROR(tmp[0], 12) & 0x0c0c0c0c;
    tk[i*4+11]  = ROR(tmp[1], 28) & 0x03030303;
    tk[i*4+11] |= ROR(tmp[1], 12) & 0x0c0c0c0c;
    tmp[0] = tk[i*4+12] ^ tk1[0];
    tmp[1] = tk[i*4+13] ^ tk1[1];
    tmp[2] = tk[i*4+14] ^ tk1[2];
    tmp[3] = tk[i*4+15] ^ tk1[3];
    if (test)
    {
      permute_tk_4(tmp);
    }else{
      permute_tk_12(tmp);
    }
    for(int j = 0; j < 4; j++)
    {
      tk[i*4+12+j]  = ROR(tmp[j], 14) & 0x30303030;
      tk[i*4+12+j] |= ROR(tmp[j], 6) & 0x0c0c0c0c;
    }
    tk[i*4+16] = ROR(tmp[2], 16) & 0xf0f0f0f0;
    tk[i*4+17] = ROR(tmp[3], 16) & 0xf0f0f0f0;
    tk[i*4+18] = ROR(tmp[0], 16) & 0xf0f0f0f0;
    tk[i*4+19] = ROR(tmp[1], 16) & 0xf0f0f0f0;
    tmp[0] = tk[i*4+20] ^ tk1[0];
    tmp[1] = tk[i*4+21] ^ tk1[1];
    tmp[2] = tk[i*4+22] ^ tk1[2];
    tmp[3] = tk[i*4+23] ^ tk1[3];
    if (test)
    {
      permute_tk_6(tmp);
    }else{
      permute_tk_14(tmp);
    }
    tk[i*4+20]  = ROR(tmp[0], 10) & 0xc3c3c3c3;
    tk[i*4+21]  = ROR(tmp[1], 10) & 0xc3c3c3c3;
    tk[i*4+22]  = ROR(tmp[2], 10) & 0xc3c3c3c3;
    tk[i*4+23]  = ROR(tmp[3], 10) & 0xc3c3c3c3;
    tk[i*4+24]  = ROR(tmp[2], 12) & 0x03030303;
    tk[i*4+24] |= ROR(tmp[2], 28) & 0x0c0c0c0c;
    tk[i*4+25]  = ROR(tmp[3], 12) & 0x03030303;
    tk[i*4+25] |= ROR(tmp[3], 28) & 0x0c0c0c0c;
    tk[i*4+26]  = ROR(tmp[0], 12) & 0x03030303;
    tk[i*4+26] |= ROR(tmp[0], 28) & 0x0c0c0c0c;
    tk[i*4+27]  = ROR(tmp[1], 12) & 0x03030303;
    tk[i*4+27] |= ROR(tmp[1], 28) & 0x0c0c0c0c;
    tmp[0] = tk[i*4+28] ^ tk1[0];
    tmp[1] = tk[i*4+29] ^ tk1[1];
    tmp[2] = tk[i*4+30] ^ tk1[2];
    tmp[3] = tk[i*4+31] ^ tk1[3];
    if (test)
    {
      permute_tk_8(tmp);
    }
    for(int j = 0; j < 4; j++)
    {
      tk[i*4+28+j]  = ROR(tmp[j], 30) & 0x30303030;
      tk[i*4+28+j] |= ROR(tmp[j], 22) & 0x0c0c0c0c;
    }
    if (test && (i+8 < ROUNDS))
    {
      tk[i*4+32] = tmp[2] & 0xf0f0f0f0;
      tk[i*4+33] = tmp[3] & 0xf0f0f0f0;
      tk[i*4+34] = tmp[0] & 0xf0f0f0f0;
      tk[i*4+35] = tmp[1] & 0xf0f0f0f0;
    }
  }
}

void skinny128_tk(uint32_t* rtk, const uint8_t* tk)
{
  for(int i = 0; i < 4*ROUNDS; i++) rtk[i] = 0;

  permute_tk(rtk, tk);
  for(int i = 0; i < ROUNDS; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      rtk[i*4+j] ^= rconst_32_bs[i*4+j];
    }
  }
}

void mcolumns_0(uint32_t* state)
{
  uint32_t tmp;
  for(int i = 0; i < 4; i++)
  {
    tmp = ROR(state[i], 24) & 0x0c0c0c0c;
    state[i] ^= ROR(tmp, 30);
    tmp = ROR(state[i], 16) & 0xc0c0c0c0;
    state[i] ^= ROR(tmp, 4);
    tmp = ROR(state[i], 8) & 0x0c0c0c0c;
    state[i] ^= ROR(tmp, 2);
  }
}

void mcolumns_1(uint32_t* state)
{
  uint32_t tmp;
  for(int i = 0; i < 4; i++)
  {
    tmp = ROR(state[i], 16) & 0x30303030;
    state[i] ^= ROR(tmp, 30);
    tmp = state[i] & 0x03030303;
    state[i] ^= ROR(tmp, 28);
    tmp = ROR(state[i], 16) & 0x30303030;
    state[i] ^= ROR(tmp, 2);
  }
}

void mcolumns_2(uint32_t* state)
{
  uint32_t tmp;
  for(int i = 0; i < 4; i++)
  {
    tmp = ROR(state[i], 8) & 0xc0c0c0c0;
    state[i] ^= ROR(tmp, 6);
    tmp = ROR(state[i], 16) & 0x0c0c0c0c;
    state[i] ^= ROR(tmp, 28);
    tmp = ROR(state[i], 24) & 0xc0c0c0c0;
    state[i] ^= ROR(tmp, 2);
  }
}

void mcolumns_3(uint32_t* state)
{
  uint32_t tmp;
  for(int i = 0; i < 4; i++)
  {
    tmp = state[i] & 0x03030303;
    state[i] ^= ROR(tmp, 30);
    tmp = state[i] & 0x30303030;
    state[i] ^= ROR(tmp, 4);
    tmp = state[i] & 0x03030303;
    state[i] ^= ROR(tmp, 26);
  }
}

void inv_mcolumns_0(uint32_t* state)
{
  uint32_t tmp;
  for(int i = 0; i < 4; i++)
  {
  tmp = ROR(state[i], 8) & 0x0c0c0c0c;
  state[i] ^= ROR(tmp, 2);
  tmp = ROR(state[i], 16) & 0xc0c0c0c0;
  state[i] ^= ROR(tmp, 4);
  tmp = ROR(state[i], 24) & 0x0c0c0c0c;
  state[i] ^= ROR(tmp, 30);
  }
}

void inv_mcolumns_1(uint32_t* state)
{
  uint32_t tmp;
  for(int i = 0; i < 4; i++)
  {
    tmp = ROR(state[i], 16) & 0x30303030;
    state[i] ^= ROR(tmp, 2);
    tmp = state[i] & 0x03030303;
    state[i] ^= ROR(tmp, 28);
    tmp = ROR(state[i], 16) & 0x30303030;
    state[i] ^= ROR(tmp, 30);
  }
}

void inv_mcolumns_2(uint32_t* state)
{
  uint32_t tmp;
  for(int i = 0; i < 4; i++)
  {
    tmp = ROR(state[i], 24) & 0xc0c0c0c0;
    state[i] ^= ROR(tmp, 2);
    tmp = ROR(state[i], 16) & 0x0c0c0c0c;
    state[i] ^= ROR(tmp, 28);
    tmp = ROR(state[i], 8) & 0xc0c0c0c0;
    state[i] ^= ROR(tmp, 6);
  }
}

void inv_mcolumns_3(uint32_t* state)
{
  uint32_t tmp;
  for(int i = 0; i < 4; i++)
  {
    tmp = state[i] & 0x03030303;
    state[i] ^= ROR(tmp, 26);
    tmp = state[i] & 0x30303030;
    state[i] ^= ROR(tmp, 4);
    tmp = state[i] & 0x03030303;
    state[i] ^= ROR(tmp, 30);
  }
}

void __attribute__((noinline)) skinny128_encrypt(uint8_t* ctext, const uint8_t* ptext, const uint8_t* tk)
{
  uint32_t tmp;
  uint32_t state[4];
  uint32_t rtk[4*ROUNDS];

  skinny128_tk(rtk, tk);

  packing(state, ptext);
  QUADRUPLE_ROUND(state, rtk)
  QUADRUPLE_ROUND(state, rtk+16)
  QUADRUPLE_ROUND(state, rtk+32)
  QUADRUPLE_ROUND(state, rtk+48)
  QUADRUPLE_ROUND(state, rtk+64)
  QUADRUPLE_ROUND(state, rtk+80)
  QUADRUPLE_ROUND(state, rtk+96)
  QUADRUPLE_ROUND(state, rtk+112)
  QUADRUPLE_ROUND(state, rtk+128)
  QUADRUPLE_ROUND(state, rtk+144)
  unpacking(ctext, state);
}

void __attribute__((noinline)) skinny128_decrypt(uint8_t* ptext, const uint8_t* ctext, const uint8_t* tk)
{
  uint32_t tmp;
  uint32_t state[4];
  uint32_t rtk[4*ROUNDS];

  skinny128_tk(rtk, tk);

  packing(state, ctext);
  INV_QUADRUPLE_ROUND(state, rtk+144)
  INV_QUADRUPLE_ROUND(state, rtk+128)
  INV_QUADRUPLE_ROUND(state, rtk+112)
  INV_QUADRUPLE_ROUND(state, rtk+96)
  INV_QUADRUPLE_ROUND(state, rtk+80)
  INV_QUADRUPLE_ROUND(state, rtk+64)
  INV_QUADRUPLE_ROUND(state, rtk+48)
  INV_QUADRUPLE_ROUND(state, rtk+32)
  INV_QUADRUPLE_ROUND(state, rtk+16)
  INV_QUADRUPLE_ROUND(state, rtk)
  unpacking(ptext, state);
}

int main(void)
{
  // Test Vector 1
  const uint8_t plaintext0[16]  = {0xf2, 0x0a, 0xdb, 0x0e, 0xb0, 0x8b, 0x64, 0x8a, 0x3b, 0x2e, 0xee, 0xd1, 0xf0, 0xad, 0xda, 0x14};
  const uint8_t key0[16]        = {0x4f, 0x55, 0xcf, 0xb0, 0x52, 0x0c, 0xac, 0x52, 0xfd, 0x92, 0xc1, 0x5f, 0x37, 0x07, 0x3e, 0x93};
  const uint8_t ciphertext0[16] = {0x22, 0xff, 0x30, 0xd4, 0x98, 0xea, 0x62, 0xd7, 0xe4, 0x5b, 0x47, 0x6e, 0x33, 0x67, 0x5b, 0x74};

  // Test Vector 2
  const uint8_t plaintext1[16]  = {0x43, 0xeb, 0x90, 0x19, 0x5b, 0x9f, 0xb1, 0x58, 0x49, 0xca, 0xb1, 0xac, 0x73, 0x32, 0x63, 0xc1};
  const uint8_t key1[16]        = {0xee, 0x1e, 0x87, 0x64, 0x6a, 0xb8, 0x25, 0x47, 0xd1, 0x2f, 0x45, 0xcb, 0xc2, 0xc8, 0xa1, 0x01};
  const uint8_t ciphertext1[16] = {0xd2, 0x7d, 0x95, 0x30, 0x77, 0x17, 0x5f, 0x06, 0xd0, 0xd7, 0x70, 0x5c, 0x3a, 0x0b, 0x6f, 0xea};

  uint8_t ciphertext[16];
  skinny128_encrypt(ciphertext, plaintext0, key0);
  for(int i=0; i<16; i++)
  {
    if(ciphertext[i] != ciphertext0[i]) return -1;
  }

  uint8_t plaintext[16];
  skinny128_decrypt(plaintext, ciphertext1, key1);
  for(int i=0; i<16; i++)
  {
    if(plaintext[i] != plaintext1[i]) return -1;
  }

  return 0;
}
