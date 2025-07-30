// In part based on the code published under CERN Open Hardware Licence Version 2 at https://github.com/uclcrypto/small-pSquare
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx.h"

#define d 32
#define n 7
#define p ((1 << n) - 1)
#define RedModMersenne(a) ((a & p) + (a >> n))
#define AddModMersenne(a, b) RedModMersenne((a + b))
#define SubModMersenne(a, b) RedModMersenne((a + (b^p)))
#define MulAddModMersenne(a, b, c) RedModMersenne(RedModMersenne((a * b + c)))
#define AddMulAddAddModMersenne(a, b, c, d, e) RedModMersenne(RedModMersenne((((a + b) * c + d) + e)))
#define Psi0(a) ((((a >> 0) & 0x1) << 5) | (((a >> 1) & 0x1) << 3) | (((a >> 2) & 0x1) << 0) | (((a >> 3) & 0x1) << 4) | (((a >> 4) & 0x1) << 1) | (((a >> 5) & 0x1) << 6) | (((a >> 6) & 0x1) << 2))
#define Psi1(a) ((((a >> 6) & 0x1) << 5) | (((a >> 0) & 0x1) << 3) | (((a >> 1) & 0x1) << 0) | (((a >> 2) & 0x1) << 4) | (((a >> 3) & 0x1) << 1) | (((a >> 4) & 0x1) << 6) | (((a >> 5) & 0x1) << 2))
#define Psi2(a) ((((a >> 5) & 0x1) << 5) | (((a >> 6) & 0x1) << 3) | (((a >> 0) & 0x1) << 0) | (((a >> 1) & 0x1) << 4) | (((a >> 2) & 0x1) << 1) | (((a >> 3) & 0x1) << 6) | (((a >> 4) & 0x1) << 2))
#define Psi3(a) ((((a >> 4) & 0x1) << 5) | (((a >> 5) & 0x1) << 3) | (((a >> 6) & 0x1) << 0) | (((a >> 0) & 0x1) << 4) | (((a >> 1) & 0x1) << 1) | (((a >> 2) & 0x1) << 6) | (((a >> 3) & 0x1) << 2))
#define Psi4(a) ((((a >> 3) & 0x1) << 5) | (((a >> 4) & 0x1) << 3) | (((a >> 5) & 0x1) << 0) | (((a >> 6) & 0x1) << 4) | (((a >> 0) & 0x1) << 1) | (((a >> 1) & 0x1) << 6) | (((a >> 2) & 0x1) << 2))
#define Psi5(a) ((((a >> 2) & 0x1) << 5) | (((a >> 3) & 0x1) << 3) | (((a >> 4) & 0x1) << 0) | (((a >> 5) & 0x1) << 4) | (((a >> 6) & 0x1) << 1) | (((a >> 0) & 0x1) << 6) | (((a >> 1) & 0x1) << 2))
#define Psi6(a) ((((a >> 1) & 0x1) << 5) | (((a >> 2) & 0x1) << 3) | (((a >> 3) & 0x1) << 0) | (((a >> 4) & 0x1) << 4) | (((a >> 5) & 0x1) << 1) | (((a >> 6) & 0x1) << 6) | (((a >> 0) & 0x1) << 2))
#define LeftRot(a) ((((a << 1) + (a >> (n - 1))) & p))
#define rounds_per_step 4
#define steps 21

void TRNG_Init(void);
inline uint8_t TRNG_GetRand(void);
void TRNG_DeInit(void);

void __attribute__((always_inline)) SQ_dshares(const uint8_t *a, uint8_t *b);
void __attribute__((noinline)) Encrypt(uint8_t **plaintext, uint8_t **key, const uint8_t* tweak0, const uint8_t* tweak1, uint8_t **ciphertext);
void __attribute__((noinline)) Decrypt(uint8_t **ciphertext, uint8_t **key, const uint8_t* tweak0, const uint8_t* tweak1, uint8_t **plaintext);

void TRNG_Init(void)
{
	// Activate PLL
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	// Activate RNG
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
	RNG->CR |= RNG_CR_RNGEN;
}

inline uint8_t TRNG_GetRand(void)
{
	while(!(RNG->SR & RNG_SR_DRDY));
	return (uint8_t)((RNG->DR % p) + 1);
}

void TRNG_DeInit(void)
{
	// Deactivate RNG
	RNG->CR &= ~RNG_CR_RNGEN;
	RCC->AHB2ENR &= ~RCC_AHB2ENR_RNGEN;

	// Deactivate PLL
	RCC->CR &= ~RCC_CR_PLLON;
}

void __attribute__((always_inline)) SQ_dshares(const uint8_t *a, uint8_t *b)
{
	// Iteration i=0 pulled out of nested loop to save some cycles
	uint8_t gamma = a[0];
	b[0] = 0;
	for(int j=1; j<d; j++)
	{
		uint8_t r_r = TRNG_GetRand();
		uint8_t r_p = TRNG_GetRand();

		b[0] = AddMulAddAddModMersenne((uint64_t)LeftRot(a[j]), r_r, a[0], r_p, b[0]);
		b[j] = p - r_p;
		gamma = SubModMersenne(gamma, r_r);
	}
	b[0] = MulAddModMersenne((uint64_t)a[0], gamma, b[0]);

	// Nested loop
	for(int i=1; i<d; i++)
	{
		gamma = a[i];
		for(int j=i+1; j<d; j++)
		{
			uint8_t r_r = TRNG_GetRand();
			uint8_t r_p = TRNG_GetRand();

			b[i] = AddMulAddAddModMersenne((uint64_t)LeftRot(a[j]), r_r, a[i], r_p, b[i]);
			b[j] = SubModMersenne(b[j], r_p);
			gamma = SubModMersenne(gamma, r_r);
		}
		b[i] = MulAddModMersenne((uint64_t)a[i], gamma, b[i]);
	}
}

void __attribute__((noinline)) Encrypt(uint8_t **plaintext, uint8_t **key, const uint8_t* tweak0, const uint8_t* tweak1, uint8_t **ciphertext)
{
	uint64_t pi = 0xC90FDAA22168C234;

	// Compute round tweaks
	uint32_t round_tweaks[steps+1][16];
	for(int i=0; i<16; i++)
	{
		round_tweaks[0][i] = tweak0[i];
		round_tweaks[1][i] = tweak1[i];
	}
	for(int step=0; step<steps-1; step++)
	{
		round_tweaks[step+2][0] = Psi0(round_tweaks[step][9]);
		round_tweaks[step+2][1] = Psi1(round_tweaks[step][5]);
		round_tweaks[step+2][2] = Psi2(round_tweaks[step][13]);
		round_tweaks[step+2][3] = Psi3(round_tweaks[step][15]);
		round_tweaks[step+2][4] = Psi4(round_tweaks[step][12]);
		round_tweaks[step+2][5] = Psi5(round_tweaks[step][7]);
		round_tweaks[step+2][6] = Psi6(round_tweaks[step][14]);
		round_tweaks[step+2][7] = Psi0(round_tweaks[step][2]);
		round_tweaks[step+2][8] = Psi1(round_tweaks[step][4]);
		round_tweaks[step+2][9] = Psi2(round_tweaks[step][6]);
		round_tweaks[step+2][10] = Psi3(round_tweaks[step][8]);
		round_tweaks[step+2][11] = Psi4(round_tweaks[step][3]);
		round_tweaks[step+2][12] = Psi5(round_tweaks[step][10]);
		round_tweaks[step+2][13] = Psi6(round_tweaks[step][1]);
		round_tweaks[step+2][14] = Psi0(round_tweaks[step][11]);
		round_tweaks[step+2][15] = Psi1(round_tweaks[step][0]);
	}

	uint8_t state[16][d];
	for(int i=0; i<d; i++)
	{
		for(int j=0; j<16; j++)
		{
			state[j][i] = plaintext[j][i];
		}
	}

	// Steps
	for(int step=0; step<steps; step++)
	{
		// Add tweakey
		for(int j=0; j<16; j++) state[j][0] = AddModMersenne(state[j][0], AddModMersenne(round_tweaks[step][j], key[j][0]));
		for(int i=1; i<d; i++)
		{
			for(int j=0; j<16; j++)
			{
				state[j][i] = AddModMersenne(state[j][i], key[j][i]);
			}
		}

		// Multiple rounds per step
		for(int round=0; round<rounds_per_step; round++)
		{
			// Round constants
			uint8_t c0 = (pi & p);
			uint8_t c1 = ((pi >> 48) & p);
			uint8_t c2 = ((pi >> 32) & p);
			uint8_t c3 = ((pi >> 16) & p);
			pi = (pi << 1) + (pi >> 63);

			uint8_t f_out1[4][d];
			uint8_t f_out2[4][d];

			// Apply F function 1
			uint8_t sq1_in[d], sq1_out[d], sq2_out[d], sq3_out[d], mds2_in[d], mds3_in[d], mds4_in[d], add1_12[d], add1_34[d], add2_234[d], add2_124[d], add1_12_2[d], add1_34_2[d], mds4_out[d], mds2_out[d], add2_124_4[d], add2_234_4[d], mds1_out[d], mds3_out[d], sq4_out[d], sq5_out[d], sq6_out[d];

			sq1_in[0] = AddModMersenne(state[3][0], c0);
			for(int i=1; i<d; i++) sq1_in[i] = state[3][i];
			SQ_dshares(sq1_in, sq1_out);
			SQ_dshares(state[2], sq2_out);
			SQ_dshares(state[1], sq3_out);
			for(int i=0; i<d; i++) mds2_in[i] = AddModMersenne(sq1_out[i], state[2][i]);
			for(int i=0; i<d; i++) mds3_in[i] = AddModMersenne(sq2_out[i], state[1][i]);
			for(int i=0; i<d; i++) mds4_in[i] = AddModMersenne(sq3_out[i], state[0][i]);

			for(int i=0; i<d; i++) add1_12[i] = AddModMersenne(sq1_in[i], mds2_in[i]);
			for(int i=0; i<d; i++) add1_34[i] = AddModMersenne(mds3_in[i], mds4_in[i]);
			for(int i=0; i<d; i++) add2_234[i] = AddModMersenne(mds2_in[i], add1_34[i]);
			for(int i=0; i<d; i++) add2_124[i] = AddModMersenne(mds4_in[i], add1_12[i]);
			for(int i=0; i<d; i++) add1_12_2[i] = ((add1_12[i] << 1) & p) + (add1_12[i] >> (n-1));
			for(int i=0; i<d; i++) add1_34_2[i] = ((add1_34[i] << 1) & p) + (add1_34[i] >> (n-1));
			for(int i=0; i<d; i++) mds4_out[i] = AddModMersenne(add1_34_2[i], add2_124[i]);
			for(int i=0; i<d; i++) mds2_out[i] = AddModMersenne(add1_12_2[i], add2_234[i]);
			for(int i=0; i<d; i++) add2_124_4[i] = ((add2_124[i] << 2) & p) + (add2_124[i] >> (n-2));
			for(int i=0; i<d; i++) add2_234_4[i] = ((add2_234[i] << 2) & p) + (add2_234[i] >> (n-2));
			for(int i=0; i<d; i++) mds1_out[i] = AddModMersenne(add2_124_4[i], mds2_out[i]);
			for(int i=0; i<d; i++) mds3_out[i] = AddModMersenne(add2_234_4[i], mds4_out[i]);

			f_out1[3][0] = AddModMersenne(mds1_out[0], c1);
			for(int i=1; i<d; i++) f_out1[3][i] = mds1_out[i];
			SQ_dshares(f_out1[3], sq4_out);
			SQ_dshares(mds2_out, sq5_out);
			SQ_dshares(mds3_out, sq6_out);
			for(int i=0; i<d; i++) f_out1[0][i] = AddModMersenne(sq4_out[i], mds2_out[i]);
			for(int i=0; i<d; i++) f_out1[1][i] = AddModMersenne(sq5_out[i], mds3_out[i]);
			for(int i=0; i<d; i++) f_out1[2][i] = AddModMersenne(sq6_out[i], mds4_out[i]);

			// Apply F function 2
			sq1_in[0] = AddModMersenne(state[11][0], c2);
			for(int i=1; i<d; i++) sq1_in[i] = state[11][i];
			SQ_dshares(sq1_in, sq1_out);
			SQ_dshares(state[10], sq2_out);
			SQ_dshares(state[9], sq3_out);
			for(int i=0; i<d; i++) mds2_in[i] = AddModMersenne(sq1_out[i], state[10][i]);
			for(int i=0; i<d; i++) mds3_in[i] = AddModMersenne(sq2_out[i], state[9][i]);
			for(int i=0; i<d; i++) mds4_in[i] = AddModMersenne(sq3_out[i], state[8][i]);

			for(int i=0; i<d; i++) add1_12[i] = AddModMersenne(sq1_in[i], mds2_in[i]);
			for(int i=0; i<d; i++) add1_34[i] = AddModMersenne(mds3_in[i], mds4_in[i]);
			for(int i=0; i<d; i++) add2_234[i] = AddModMersenne(mds2_in[i], add1_34[i]);
			for(int i=0; i<d; i++) add2_124[i] = AddModMersenne(mds4_in[i], add1_12[i]);
			for(int i=0; i<d; i++) add1_12_2[i] = ((add1_12[i] << 1) & p) + (add1_12[i] >> (n-1));
			for(int i=0; i<d; i++) add1_34_2[i] = ((add1_34[i] << 1) & p) + (add1_34[i] >> (n-1));
			for(int i=0; i<d; i++) mds4_out[i] = AddModMersenne(add1_34_2[i], add2_124[i]);
			for(int i=0; i<d; i++) mds2_out[i] = AddModMersenne(add1_12_2[i], add2_234[i]);
			for(int i=0; i<d; i++) add2_124_4[i] = ((add2_124[i] << 2) & p) + (add2_124[i] >> (n-2));
			for(int i=0; i<d; i++) add2_234_4[i] = ((add2_234[i] << 2) & p) + (add2_234[i] >> (n-2));
			for(int i=0; i<d; i++) mds1_out[i] = AddModMersenne(add2_124_4[i], mds2_out[i]);
			for(int i=0; i<d; i++) mds3_out[i] = AddModMersenne(add2_234_4[i], mds4_out[i]);

			f_out2[3][0] = AddModMersenne(mds1_out[0], c3);
			for(int i=1; i<d; i++) f_out2[3][i] = mds1_out[i];
			SQ_dshares(f_out2[3], sq4_out);
			SQ_dshares(mds2_out, sq5_out);
			SQ_dshares(mds3_out, sq6_out);
			for(int i=0; i<d; i++) f_out2[0][i] = AddModMersenne(sq4_out[i], mds2_out[i]);
			for(int i=0; i<d; i++) f_out2[1][i] = AddModMersenne(sq5_out[i], mds3_out[i]);
			for(int i=0; i<d; i++) f_out2[2][i] = AddModMersenne(sq6_out[i], mds4_out[i]);

			// Apply Feistel
			uint8_t tmp[16][d];
			for(int i=0; i<d; i++)
			{
				for(int j=0; j<4; j++)
				{
					tmp[j][i] = AddModMersenne(state[j+4][i], f_out1[3-j][i]);
					tmp[j+4][i] = state[j+8][i];
					tmp[j+8][i] = AddModMersenne(state[j+12][i], f_out2[3-j][i]);
					tmp[j+12][i] = state[j][i];
				}
			}
			for(int i=0; i<d; i++)
			{
				for(int j=0; j<16; j++)
				{
					state[j][i] = tmp[j][i];
				}
			}
		}
	}

	// Final tweakey addition
	for(int j=0; j<16; j++) ciphertext[j][0] = AddModMersenne(state[j][0], AddModMersenne(round_tweaks[steps][j], key[j][0]));
	for(int i=1; i<d; i++)
	{
		for(int j=0; j<16; j++)
		{
			ciphertext[j][i] = AddModMersenne(state[j][i], key[j][i]);
		}
	}
}

void __attribute__((noinline)) Decrypt(uint8_t **ciphertext, uint8_t **key, const uint8_t* tweak0, const uint8_t* tweak1, uint8_t **plaintext)
{
	uint64_t pi = 0xAA22168C234C90FD;

	// Compute round tweaks
	uint32_t round_tweaks[steps+1][16];
	for(int i=0; i<16; i++)
	{
		round_tweaks[0][i] = tweak0[i];
		round_tweaks[1][i] = tweak1[i];
	}
	for(int step=0; step<steps-1; step++)
	{
		round_tweaks[step+2][0] = Psi0(round_tweaks[step][9]);
		round_tweaks[step+2][1] = Psi1(round_tweaks[step][5]);
		round_tweaks[step+2][2] = Psi2(round_tweaks[step][13]);
		round_tweaks[step+2][3] = Psi3(round_tweaks[step][15]);
		round_tweaks[step+2][4] = Psi4(round_tweaks[step][12]);
		round_tweaks[step+2][5] = Psi5(round_tweaks[step][7]);
		round_tweaks[step+2][6] = Psi6(round_tweaks[step][14]);
		round_tweaks[step+2][7] = Psi0(round_tweaks[step][2]);
		round_tweaks[step+2][8] = Psi1(round_tweaks[step][4]);
		round_tweaks[step+2][9] = Psi2(round_tweaks[step][6]);
		round_tweaks[step+2][10] = Psi3(round_tweaks[step][8]);
		round_tweaks[step+2][11] = Psi4(round_tweaks[step][3]);
		round_tweaks[step+2][12] = Psi5(round_tweaks[step][10]);
		round_tweaks[step+2][13] = Psi6(round_tweaks[step][1]);
		round_tweaks[step+2][14] = Psi0(round_tweaks[step][11]);
		round_tweaks[step+2][15] = Psi1(round_tweaks[step][0]);
	}

	uint8_t state[16][d];
	for(int i=0; i<d; i++)
	{
		for(int j=0; j<16; j++)
		{
			state[j][i] = ciphertext[j][i];
		}
	}

	// Steps
	for(int step=0; step<steps; step++)
	{
		// Add tweakey
		for(int j=0; j<16; j++) state[j][0] = SubModMersenne(state[j][0], AddModMersenne(round_tweaks[steps-step][j], key[j][0]));
		for(int i=1; i<d; i++)
		{
			for(int j=0; j<16; j++)
			{
				state[j][i] = SubModMersenne(state[j][i], key[j][i]);
			}
		}

		// Multiple rounds per step
		for(int round=0; round<rounds_per_step; round++)
		{
			// Round constants
			pi = (pi << 63) + (pi >> 1);
			uint8_t c0 = (pi & p);
			uint8_t c1 = ((pi >> 48) & p);
			uint8_t c2 = ((pi >> 32) & p);
			uint8_t c3 = ((pi >> 16) & p);

			uint8_t f_out1[4][d];
			uint8_t f_out2[4][d];

			// Apply F function 1
			uint8_t sq1_in[d], sq1_out[d], sq2_out[d], sq3_out[d], mds2_in[d], mds3_in[d], mds4_in[d], add1_12[d], add1_34[d], add2_234[d], add2_124[d], add1_12_2[d], add1_34_2[d], mds4_out[d], mds2_out[d], add2_124_4[d], add2_234_4[d], mds1_out[d], mds3_out[d], sq4_out[d], sq5_out[d], sq6_out[d];

			sq1_in[0] = AddModMersenne(state[15][0], c0);
			for(int i=1; i<d; i++) sq1_in[i] = state[15][i];
			SQ_dshares(sq1_in, sq1_out);
			SQ_dshares(state[14], sq2_out);
			SQ_dshares(state[13], sq3_out);
			for(int i=0; i<d; i++) mds2_in[i] = AddModMersenne(sq1_out[i], state[14][i]);
			for(int i=0; i<d; i++) mds3_in[i] = AddModMersenne(sq2_out[i], state[13][i]);
			for(int i=0; i<d; i++) mds4_in[i] = AddModMersenne(sq3_out[i], state[12][i]);

			for(int i=0; i<d; i++) add1_12[i] = AddModMersenne(sq1_in[i], mds2_in[i]);
			for(int i=0; i<d; i++) add1_34[i] = AddModMersenne(mds3_in[i], mds4_in[i]);
			for(int i=0; i<d; i++) add2_234[i] = AddModMersenne(mds2_in[i], add1_34[i]);
			for(int i=0; i<d; i++) add2_124[i] = AddModMersenne(mds4_in[i], add1_12[i]);
			for(int i=0; i<d; i++) add1_12_2[i] = ((add1_12[i] << 1) & p) + (add1_12[i] >> (n-1));
			for(int i=0; i<d; i++) add1_34_2[i] = ((add1_34[i] << 1) & p) + (add1_34[i] >> (n-1));
			for(int i=0; i<d; i++) mds4_out[i] = AddModMersenne(add1_34_2[i], add2_124[i]);
			for(int i=0; i<d; i++) mds2_out[i] = AddModMersenne(add1_12_2[i], add2_234[i]);
			for(int i=0; i<d; i++) add2_124_4[i] = ((add2_124[i] << 2) & p) + (add2_124[i] >> (n-2));
			for(int i=0; i<d; i++) add2_234_4[i] = ((add2_234[i] << 2) & p) + (add2_234[i] >> (n-2));
			for(int i=0; i<d; i++) mds1_out[i] = AddModMersenne(add2_124_4[i], mds2_out[i]);
			for(int i=0; i<d; i++) mds3_out[i] = AddModMersenne(add2_234_4[i], mds4_out[i]);

			f_out1[3][0] = AddModMersenne(mds1_out[0], c1);
			for(int i=1; i<d; i++) f_out1[3][i] = mds1_out[i];
			SQ_dshares(f_out1[3], sq4_out);
			SQ_dshares(mds2_out, sq5_out);
			SQ_dshares(mds3_out, sq6_out);
			for(int i=0; i<d; i++) f_out1[0][i] = AddModMersenne(sq4_out[i], mds2_out[i]);
			for(int i=0; i<d; i++) f_out1[1][i] = AddModMersenne(sq5_out[i], mds3_out[i]);
			for(int i=0; i<d; i++) f_out1[2][i] = AddModMersenne(sq6_out[i], mds4_out[i]);

			// Apply F function 2
			sq1_in[0] = AddModMersenne(state[7][0], c2);
			for(int i=1; i<d; i++) sq1_in[i] = state[7][i];
			SQ_dshares(sq1_in, sq1_out);
			SQ_dshares(state[6], sq2_out);
			SQ_dshares(state[5], sq3_out);
			for(int i=0; i<d; i++) mds2_in[i] = AddModMersenne(sq1_out[i], state[6][i]);
			for(int i=0; i<d; i++) mds3_in[i] = AddModMersenne(sq2_out[i], state[5][i]);
			for(int i=0; i<d; i++) mds4_in[i] = AddModMersenne(sq3_out[i], state[4][i]);

			for(int i=0; i<d; i++) add1_12[i] = AddModMersenne(sq1_in[i], mds2_in[i]);
			for(int i=0; i<d; i++) add1_34[i] = AddModMersenne(mds3_in[i], mds4_in[i]);
			for(int i=0; i<d; i++) add2_234[i] = AddModMersenne(mds2_in[i], add1_34[i]);
			for(int i=0; i<d; i++) add2_124[i] = AddModMersenne(mds4_in[i], add1_12[i]);
			for(int i=0; i<d; i++) add1_12_2[i] = ((add1_12[i] << 1) & p) + (add1_12[i] >> (n-1));
			for(int i=0; i<d; i++) add1_34_2[i] = ((add1_34[i] << 1) & p) + (add1_34[i] >> (n-1));
			for(int i=0; i<d; i++) mds4_out[i] = AddModMersenne(add1_34_2[i], add2_124[i]);
			for(int i=0; i<d; i++) mds2_out[i] = AddModMersenne(add1_12_2[i], add2_234[i]);
			for(int i=0; i<d; i++) add2_124_4[i] = ((add2_124[i] << 2) & p) + (add2_124[i] >> (n-2));
			for(int i=0; i<d; i++) add2_234_4[i] = ((add2_234[i] << 2) & p) + (add2_234[i] >> (n-2));
			for(int i=0; i<d; i++) mds1_out[i] = AddModMersenne(add2_124_4[i], mds2_out[i]);
			for(int i=0; i<d; i++) mds3_out[i] = AddModMersenne(add2_234_4[i], mds4_out[i]);

			f_out2[3][0] = AddModMersenne(mds1_out[0], c3);
			for(int i=1; i<d; i++) f_out2[3][i] = mds1_out[i];
			SQ_dshares(f_out2[3], sq4_out);
			SQ_dshares(mds2_out, sq5_out);
			SQ_dshares(mds3_out, sq6_out);
			for(int i=0; i<d; i++) f_out2[0][i] = AddModMersenne(sq4_out[i], mds2_out[i]);
			for(int i=0; i<d; i++) f_out2[1][i] = AddModMersenne(sq5_out[i], mds3_out[i]);
			for(int i=0; i<d; i++) f_out2[2][i] = AddModMersenne(sq6_out[i], mds4_out[i]);

			// Apply Feistel
			uint8_t tmp[16][d];
			for(int i=0; i<d; i++)
			{
				for(int j=0; j<4; j++)
				{
					tmp[j][i] = state[j+12][i];
					tmp[j+4][i] = SubModMersenne(state[j][i], f_out1[3-j][i]);
					tmp[j+8][i] = state[j+4][i];
					tmp[j+12][i] = SubModMersenne(state[j+8][i], f_out2[3-j][i]);
				}
			}
			for(int i=0; i<d; i++)
			{
				for(int j=0; j<16; j++)
				{
					state[j][i] = tmp[j][i];
				}
			}
		}
	}

	// Final tweakey addition
	for(int j=0; j<16; j++) plaintext[j][0] = SubModMersenne(state[j][0], AddModMersenne(round_tweaks[0][j], key[j][0]));
	for(int i=1; i<d; i++)
	{
		for(int j=0; j<16; j++)
		{
			plaintext[j][i] = SubModMersenne(state[j][i], key[j][i]);
		}
	}
}


int main(void)
{
	TRNG_Init();

	// Test Vector 1
	const uint8_t plaintext0[16]  = {0x52, 0x30, 0x34, 0x67, 0x37, 0x0d, 0x0e, 0x27, 0x24, 0x57, 0x48, 0x62, 0x7b, 0x6f, 0x7b, 0x19};
	const uint8_t key0[16]        = {0x2d, 0x60, 0x05, 0x6d, 0x3b, 0x2e, 0x0c, 0x1e, 0x15, 0x2a, 0x6b, 0x55, 0x07, 0x11, 0x10, 0x26};
	const uint8_t tweak0_0[16]    = {0x32, 0x39, 0x49, 0x5a, 0x5d, 0x2f, 0x0d, 0x20, 0x70, 0x77, 0x41, 0x5f, 0x5f, 0x5c, 0x16, 0x61};
	const uint8_t tweak0_1[16]    = {0x5c, 0x7d, 0x24, 0x53, 0x22, 0x79, 0x36, 0x2d, 0x0c, 0x46, 0x4d, 0x7e, 0x5a, 0x2a, 0x4f, 0x26};
	const uint8_t ciphertext0[16] = {0x4c, 0x54, 0x6d, 0x5b, 0x5c, 0x01, 0x7c, 0x56, 0x5a, 0x10, 0x26, 0x02, 0x1e, 0x51, 0x24, 0x79};

	// Test Vector 2
	const uint8_t plaintext1[16]  = {0x11, 0x7a, 0x6d, 0x10, 0x4e, 0x25, 0x10, 0x67, 0x02, 0x26, 0x0b, 0x2d, 0x3a, 0x68, 0x78, 0x25};
	const uint8_t key1[16]        = {0x33, 0x54, 0x46, 0x2d, 0x4d, 0x72, 0x47, 0x6e, 0x1d, 0x05, 0x3a, 0x6f, 0x08, 0x5d, 0x3e, 0x69};
	const uint8_t tweak1_0[16]    = {0x2b, 0x38, 0x6b, 0x77, 0x4d, 0x3b, 0x60, 0x49, 0x4a, 0x27, 0x7a, 0x36, 0x2f, 0x2a, 0x3a, 0x52};
	const uint8_t tweak1_1[16]    = {0x7d, 0x45, 0x7f, 0x7a, 0x42, 0x03, 0x15, 0x3a, 0x4f, 0x31, 0x1a, 0x29, 0x11, 0x2b, 0x63, 0x59};
	const uint8_t ciphertext1[16] = {0x27, 0x6a, 0x32, 0x2e, 0x63, 0x7a, 0x2c, 0x20, 0x1f, 0x4e, 0x75, 0x3c, 0x02, 0x1d, 0x05, 0x1d};

	// Allocate memory for sharings
	uint8_t *shared_plaintext[16], *shared_key[16], *shared_ciphertext[16];
	for(int i=0; i<16; i++)
	{
		shared_plaintext[i] = (uint8_t*)malloc(d * sizeof(uint8_t));
		if(shared_plaintext[i] == NULL) return -1;
		shared_key[i] = (uint8_t*)malloc(d * sizeof(uint8_t));
		if(shared_key[i] == NULL) return -1;
		shared_ciphertext[i] = (uint8_t*)malloc(d * sizeof(uint8_t));
		if(shared_ciphertext[i] == NULL) return -1;
	}

	// Generate sharing for test vector 1
	for(int i=0; i<16; i++)
	{
		shared_plaintext[i][d-1] = plaintext0[i];
		shared_key[i][d-1] = key0[i];
		for(int j=0; j<d-1; j++)
		{
			shared_plaintext[i][j] = TRNG_GetRand();
			shared_plaintext[i][d-1] = SubModMersenne(shared_plaintext[i][d-1], shared_plaintext[i][j]);
			shared_key[i][j] = TRNG_GetRand();
			shared_key[i][d-1] = SubModMersenne(shared_key[i][d-1], shared_key[i][j]);
		}
	}

	// Encrypt
	Encrypt(shared_plaintext, shared_key, tweak0_0, tweak0_1, shared_ciphertext);

	// Verify test vector 1
	for(int i=0; i<16; i++)
	{
		uint8_t ciphertext_tmp = shared_ciphertext[i][0];
		for(int j=1; j<d; j++)
		{
			ciphertext_tmp = AddModMersenne(ciphertext_tmp, shared_ciphertext[i][j]);
		}
		if(ciphertext_tmp != ciphertext0[i]) return -1;
	}


	// Generate sharing for test vector 2
	for(int i=0; i<16; i++)
	{
		shared_ciphertext[i][d-1] = ciphertext1[i];
		for(int j=0; j<d-1; j++)
		{
			shared_ciphertext[i][j] = TRNG_GetRand();
			shared_ciphertext[i][d-1] = SubModMersenne(shared_ciphertext[i][d-1], shared_ciphertext[i][j]);
		}
		shared_key[i][d-1] = key1[i];
		for(int j=0; j<d-1; j++)
		{
			shared_key[i][j] = TRNG_GetRand();
			shared_key[i][d-1] = SubModMersenne(shared_key[i][d-1], shared_key[i][j]);
		}
	}

	// Decrypt
	Decrypt(shared_ciphertext, shared_key, tweak1_0, tweak1_1, shared_plaintext);

	// Verify test vector 2
	for(int i=0; i<16; i++)
	{
		uint8_t plaintext_tmp = shared_plaintext[i][0];
		for(int j=1; j<d; j++)
		{
			plaintext_tmp = AddModMersenne(plaintext_tmp, shared_plaintext[i][j]);
		}
		if(plaintext_tmp != plaintext1[i]) return -1;
	}


	// Free memory
	for(int i=0; i<16; i++)
	{
		free(shared_plaintext[i]);
		free(shared_key[i]);
		free(shared_ciphertext[i]);
	}

	TRNG_DeInit();

	return 0;
}
