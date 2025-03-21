#include <stdint.h>

#define n 31
#define p ((1U << n) - 1)
#define RedModMersenneA(a) ((a & p) + (a >> n))
#define RedModMersenneS(a) ((a & p) - (a >> n))
#define AddModMersenne(a, b) RedModMersenneA((a + b))
#define SubModMersenne(a, b) RedModMersenneS((a - b))
#define SquAddModMersenne(a, b) RedModMersenneA(RedModMersenneA((a * a + b)))
#define SquAddAddModMersenne(a, b, c) RedModMersenneA(RedModMersenneA((a * a + b + c)))
#define rounds_per_step 4
#define steps 14

void __attribute__ ((noinline)) Encrypt(const uint32_t *plaintext, const uint32_t* key, uint32_t *ciphertext);
void __attribute__ ((noinline)) Decrypt(const uint32_t *ciphertext, const uint32_t* key, uint32_t *plaintext);

void __attribute__ ((noinline)) Encrypt(const uint32_t *plaintext, const uint32_t* key, uint32_t *ciphertext)
{
  uint64_t pi = 0xC90FDAA22168C234;

  uint32_t state[4];
  for(int i=0; i<4; i++) state[i] = plaintext[i];

  // Steps
  for(int step=0; step<steps; step++)
  {
    // Add tweakey
    for(int i=0; i<4; i++) state[i] = AddModMersenne(state[i], key[i]);

    // Multiple rounds per step
    for(int round=0; round<rounds_per_step; round++)
    {
      // Round constants
      uint32_t c0 = pi & p;
      uint32_t c1 = (pi >> 32) & p;
      pi = (pi << 1) + (pi >> 63);

      // Apply F function + Feistel
      uint32_t sq1_in = AddModMersenne(state[1], c0);
      uint32_t mds_in = SquAddModMersenne((uint64_t)sq1_in, state[0]);
      uint32_t mds_out = AddModMersenne(sq1_in, mds_in);
      uint32_t f_out0 = AddModMersenne(mds_out, c1);
      uint32_t f_out1 = SquAddAddModMersenne((uint64_t)f_out0, mds_out, mds_in);
      uint32_t swap0 = AddModMersenne(state[2], f_out0);
      uint32_t swap1 = AddModMersenne(state[3], f_out1);
      state[2] = state[0];
      state[3] = state[1];
      state[0] = swap0;
      state[1] = swap1;
    }
  }

  // Final tweakey addition
  for(int i=0; i<4; i++) ciphertext[i] = AddModMersenne(state[i], key[i]);
}

void __attribute__ ((noinline)) Decrypt(const uint32_t *ciphertext, const uint32_t* key, uint32_t *plaintext)
{
  uint64_t pi = 0x34C90FDAA22168C2;

  uint32_t state[4];
  for(int i=0; i<4; i++) state[i] = ciphertext[i];

  // Steps
  for(int step=0; step<steps; step++)
  {
    // Add tweakey
    for(int i=0; i<4; i++) state[i] = SubModMersenne(state[i], key[i]);

    // Multiple rounds per step
    for(int round=0; round<rounds_per_step; round++)
    {
      // Round constants
      pi = (pi << 63) + (pi >> 1);
      uint32_t c0 = pi & p;
      uint32_t c1 = (pi >> 32) & p;

      // Apply F function + Feistel
      uint32_t sq1_in = AddModMersenne(state[3], c0);
      uint32_t mds_in = SquAddModMersenne((uint64_t)sq1_in, state[2]);
      uint32_t mds_out = AddModMersenne(sq1_in, mds_in);
      uint32_t f_out0 = AddModMersenne(mds_out, c1);
      uint32_t f_out1 = SquAddAddModMersenne((uint64_t)f_out0, mds_out, mds_in);
      uint32_t swap0 = SubModMersenne(state[0], f_out0);
      uint32_t swap1 = SubModMersenne(state[1], f_out1);
      state[0] = state[2];
      state[1] = state[3];
      state[2] = swap0;
      state[3] = swap1;
    }
  }

  // Final tweakey addition
  for(int i=0; i<4; i++) plaintext[i] = SubModMersenne(state[i], key[i]);
}

int main(void)
{
  // Test Vector 1
  const uint32_t plaintext0[4]  = {0x78066d6b, 0x68a24eb4, 0x2d12aacd, 0x42bb7df4};
  const uint32_t key0[4]        = {0x3a85ecf4, 0x010084b5, 0x28e3f4fb, 0x41514a49};
  const uint32_t ciphertext0[4] = {0x0e904f42, 0x0981bfd9, 0x3309b9ac, 0x19f408ff};

  // Test Vector 2
  const uint32_t plaintext1[4]  = {0x1f3202d0, 0x2ebbcc8c, 0x261b659f, 0x22171a32};
  const uint32_t key1[4]        = {0x6abbc641, 0x44f243a3, 0x7660994b, 0x5c03e803};
  const uint32_t ciphertext1[4] = {0x1f762b3c, 0x4fbd7adc, 0x2469c337, 0x45bfc587};

  uint32_t ciphertext[4];
  Encrypt(plaintext0, key0, ciphertext);
  for(int i=0; i<4; i++)
  {
    if(ciphertext[i] != ciphertext0[i]) return -1;
  }

  uint32_t plaintext[4];
  Decrypt(ciphertext1, key1, plaintext);
  for(int i=0; i<4; i++)
  {
    if(plaintext[i] != plaintext1[i]) return -1;
  }

  return 0;
}
