#include <stdint.h>

#define n 31
#define p ((1U << n) - 1)
#define RedModMersenneA(a) ((a & p) + (a >> n))
#define RedModMersenneS(a) ((a & p) - (a >> n))
#define AddModMersenne(a, b) RedModMersenneA((a + b))
#define SubModMersenne(a, b) RedModMersenneS((a - b))
#define SquAddModMersenne(a, b) RedModMersenneA(RedModMersenneA((a * a + b)))
#define SquAddAddModMersenne(a, b, c) RedModMersenneA(RedModMersenneA((a * a + b + c)))
#define Psi(a) ((a ^ (a >> 1) ^ (p >> 1)))
#define rounds_per_step 4
#define steps 18

void __attribute__ ((noinline)) Encrypt(const uint32_t *plaintext, const uint32_t* key, const uint32_t* tweak, uint32_t *ciphertext);
void __attribute__ ((noinline)) Decrypt(const uint32_t *ciphertext, const uint32_t* key, const uint32_t* tweak, uint32_t *plaintext);

void __attribute__ ((noinline)) Encrypt(const uint32_t *plaintext, const uint32_t* key, const uint32_t* tweak, uint32_t *ciphertext)
{
  uint64_t pi = 0xC90FDAA22168C234;
	
  // Compute round tweaks
  uint32_t round_tweaks[steps+1][8];
  for(int i=0; i<8; i++) round_tweaks[0][i] = tweak[i];
  for(int step=0; step<steps; step++)
  {
		uint32_t round_tweak_tmp[8];
		for(int i=0; i<8; i++) round_tweak_tmp[i] = round_tweaks[step][i];
		for(int i=0; i<4; i++)
		{
			uint32_t round_tweak_psi[8];
			for(int j=0; j<8; j++) round_tweak_psi[j] = Psi(round_tweak_tmp[j]);
			round_tweak_tmp[1] = AddModMersenne(round_tweak_psi[0], round_tweak_psi[3]);
			round_tweak_tmp[0] = AddModMersenne(round_tweak_psi[0], round_tweak_tmp[1]);
			round_tweak_tmp[3] = AddModMersenne(round_tweak_psi[2], round_tweak_psi[5]);
			round_tweak_tmp[2] = AddModMersenne(round_tweak_psi[2], round_tweak_tmp[3]);
			round_tweak_tmp[5] = AddModMersenne(round_tweak_psi[4], round_tweak_psi[7]);
			round_tweak_tmp[4] = AddModMersenne(round_tweak_psi[4], round_tweak_tmp[5]);
			round_tweak_tmp[7] = AddModMersenne(round_tweak_psi[6], round_tweak_psi[1]);
			round_tweak_tmp[6] = AddModMersenne(round_tweak_psi[6], round_tweak_tmp[7]);
		}
		for(int i=0; i<8; i++) round_tweaks[step+1][i] = round_tweak_tmp[i];
  }

  uint32_t state[4];
  for(int i=0; i<4; i++) state[i] = plaintext[i];

  // Steps
  for(int step=0; step<steps; step+=2)
  {
    // Add tweakey
    for(int i=0; i<4; i++) state[i] = AddModMersenne(state[i], AddModMersenne(round_tweaks[step][i], key[i]));

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

    // Add tweakey
    for(int i=0; i<4; i++) state[i] = AddModMersenne(state[i], AddModMersenne(round_tweaks[step+1][i+4], key[i]));

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
  for(int i=0; i<4; i++) ciphertext[i] = AddModMersenne(state[i], AddModMersenne(round_tweaks[steps][i], key[i]));
}

void __attribute__ ((noinline)) Decrypt(const uint32_t *ciphertext, const uint32_t* key, const uint32_t* tweak, uint32_t *plaintext)
{
  uint64_t pi = 0x0FDAA22168C234C9;
	
  // Compute round tweaks
  uint32_t round_tweaks[steps+1][8];
  for(int i=0; i<8; i++) round_tweaks[0][i] = tweak[i];
  for(int step=0; step<steps; step++)
  {
		uint32_t round_tweak_tmp[8];
		for(int i=0; i<8; i++) round_tweak_tmp[i] = round_tweaks[step][i];
		for(int i=0; i<4; i++)
		{
			uint32_t round_tweak_psi[8];
			for(int j=0; j<8; j++) round_tweak_psi[j] = Psi(round_tweak_tmp[j]);
			round_tweak_tmp[1] = AddModMersenne(round_tweak_psi[0], round_tweak_psi[3]);
			round_tweak_tmp[0] = AddModMersenne(round_tweak_psi[0], round_tweak_tmp[1]);
			round_tweak_tmp[3] = AddModMersenne(round_tweak_psi[2], round_tweak_psi[5]);
			round_tweak_tmp[2] = AddModMersenne(round_tweak_psi[2], round_tweak_tmp[3]);
			round_tweak_tmp[5] = AddModMersenne(round_tweak_psi[4], round_tweak_psi[7]);
			round_tweak_tmp[4] = AddModMersenne(round_tweak_psi[4], round_tweak_tmp[5]);
			round_tweak_tmp[7] = AddModMersenne(round_tweak_psi[6], round_tweak_psi[1]);
			round_tweak_tmp[6] = AddModMersenne(round_tweak_psi[6], round_tweak_tmp[7]);
		}
		for(int i=0; i<8; i++) round_tweaks[step+1][i] = round_tweak_tmp[i];
  }
	
  uint32_t state[4];
  for(int i=0; i<4; i++) state[i] = ciphertext[i];

  // Steps
  for(int step=0; step<steps; step+=2)
  {
    // Add tweakey
    for(int i=0; i<4; i++) state[i] = SubModMersenne(state[i], AddModMersenne(round_tweaks[steps-step][i], key[i]));

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

    // Add tweakey
    for(int i=0; i<4; i++) state[i] = SubModMersenne(state[i], AddModMersenne(round_tweaks[steps-(step+1)][i+4], key[i]));

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
  for(int i=0; i<4; i++) plaintext[i] = SubModMersenne(state[i], AddModMersenne(round_tweaks[0][i], key[i]));
}

int main(void)
{
  // Test Vector 1
  const uint32_t plaintext0[4]  = {0x78066d6b, 0x68a24eb4, 0x2d12aacd, 0x42bb7df4};
  const uint32_t key0[4]        = {0x3a85ecf4, 0x010084b5, 0x28e3f4fb, 0x41514a49};
  const uint32_t tweak0[8]      = {0x7fe52871, 0x510c6891, 0x1ce7b0bf, 0x03640680, 0x00a93de5, 0x4e1b115f, 0x7bf77fc4, 0x2585c8e6};
  const uint32_t ciphertext0[4] = {0x4580fa81, 0x16a29749, 0x635f4399, 0x296a5077};

  // Test Vector 2
  const uint32_t plaintext1[4]  = {0x1f3202d0, 0x2ebbcc8c, 0x261b659f, 0x22171a32};
  const uint32_t key1[4]        = {0x6abbc641, 0x44f243a3, 0x7660994b, 0x5c03e803};
  const uint32_t tweak1[8]      = {0x45ce7585, 0x5d27ee4f, 0x101c05fb, 0x1d873632, 0x39a0ed3a, 0x2da8cef2, 0x22306798, 0x7919e018};
  const uint32_t ciphertext1[4] = {0x7ad1db84, 0x36d57132, 0x368fa2b1, 0x4930d0c5};

  uint32_t ciphertext[4];
  Encrypt(plaintext0, key0, tweak0, ciphertext);
  for(int i=0; i<4; i++)
  {
    if(ciphertext[i] != ciphertext0[i]) return -1;
  }

  uint32_t plaintext[4];
  Decrypt(ciphertext1, key1, tweak1, plaintext);
  for(int i=0; i<4; i++)
  {
    if(plaintext[i] != plaintext1[i]) return -1;
  }

  return 0;
}
