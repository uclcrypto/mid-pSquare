#include <stdint.h>
#include <stdlib.h>

#define d 32
#define n 31
#define p ((1U << n) - 1)
#define RedModMersenneA(a) ((a & p) + (a >> n))
#define RedModMersenneS(a) ((a & p) - (a >> n))
#define AddModMersenne(a, b) RedModMersenneA((a + b))
#define SubModMersenne(a, b) RedModMersenneS((a - b))
#define MulAddModMersenne(a, b, c) RedModMersenneA(RedModMersenneA((a * b + c)))
#define AddMulAddAddModMersenne(a, b, c, d, e) RedModMersenneA(RedModMersenneA((((a + b) * c + d) + e)))
#define LeftRot(a) ((((a << 1) + (a >> (n - 1))) & p))
#define Psi(a) ((a ^ (a >> 1) ^ (p >> 1)))
#define rounds_per_step 4
#define steps 16

#define RAND_CONST 0x618dde64

void __attribute__((always_inline)) SQ_dshares(const uint32_t *a, uint32_t *b);
void __attribute__((noinline)) Encrypt(uint32_t **plaintext, uint32_t **key, const uint32_t* tweak, uint32_t **ciphertext);
void __attribute__((noinline)) Decrypt(uint32_t **ciphertext, uint32_t **key, const uint32_t* tweak, uint32_t **plaintext);

void __attribute__((always_inline)) SQ_dshares(const uint32_t *a, uint32_t *b)
{
	// Iteration i=0 pulled out of nested loop to save some cycles
	uint32_t gamma = a[0];
	b[0] = 0;
	for(int j=1; j<d; j++)
	{
		uint32_t r_r = RAND_CONST;
		uint32_t r_p = RAND_CONST;

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
			uint32_t r_r = RAND_CONST;
			uint32_t r_p = RAND_CONST;

			b[i] = AddMulAddAddModMersenne((uint64_t)LeftRot(a[j]), r_r, a[i], r_p, b[i]);
			b[j] = SubModMersenne(b[j], r_p);
			gamma = SubModMersenne(gamma, r_r);
		}
		b[i] = MulAddModMersenne((uint64_t)a[i], gamma, b[i]);
	}
}

void __attribute__((noinline)) Encrypt(uint32_t **plaintext, uint32_t **key, const uint32_t* tweak, uint32_t **ciphertext)
{
	uint64_t pi = 0xC90FDAA22168C234;

	// Compute round tweaks
	uint32_t round_tweaks[steps+1][4];
	for(int i=0; i<4; i++) round_tweaks[0][i] = tweak[i];
	for(int step=0; step<steps; step++)
	{
		uint32_t round_tweak_tmp[4];
		for(int i=0; i<4; i++) round_tweak_tmp[i] = round_tweaks[step][i];
		for(int i=0; i<4; i++)
		{
			uint32_t round_tweak_psi[4];
			for(int j=0; j<4; j++) round_tweak_psi[j] = Psi(round_tweak_tmp[j]);
			round_tweak_tmp[1] = AddModMersenne(round_tweak_psi[0], round_tweak_psi[3]);
			round_tweak_tmp[0] = AddModMersenne(round_tweak_psi[0], round_tweak_tmp[1]);
			round_tweak_tmp[3] = AddModMersenne(round_tweak_psi[2], round_tweak_psi[1]);
			round_tweak_tmp[2] = AddModMersenne(round_tweak_psi[2], round_tweak_tmp[3]);
		}
		for(int i=0; i<4; i++) round_tweaks[step+1][i] = round_tweak_tmp[i];
	}

	uint32_t state[4][d];
	for(int i=0; i<d; i++)
	{
		for(int j=0; j<4; j++)
		{
			state[j][i] = plaintext[j][i];
		}
	}

	// Steps
	for(int step=0; step<steps; step++)
	{
		// Add tweakey
		for(int i=0; i<4; i++) state[i][0] = AddModMersenne(state[i][0], AddModMersenne(round_tweaks[step][i], key[i][0]));
		for(int i=1; i<d; i++)
		{
			for(int j=0; j<4; j++)
			{
				state[j][i] = AddModMersenne(state[j][i], key[j][i]);
			}
		}

		// Multiple rounds per step
		for(int round=0; round<rounds_per_step; round++)
		{
			// Round constants
			uint32_t c0 = pi & p;
			uint32_t c1 = (pi >> 32) & p;
			pi = (pi << 1) + (pi >> 63);

			// Apply F function + Feistel
			uint32_t sq1_in, sq1_out[d], mds_in[d], f1_out[d], mds1_out[d], sq2_out[d], f2_out[d], swap0[d], swap1[d];
			sq1_in = state[1][0];
			state[1][0] = AddModMersenne(state[1][0], c0);
			SQ_dshares(state[1], sq1_out);
			for(int i=0; i<d; i++) mds_in[i] = AddModMersenne(sq1_out[i], state[0][i]);
			f1_out[0] = AddModMersenne(state[1][0], mds_in[0]);
			for(int i=1; i<d; i++) f1_out[i] = AddModMersenne(state[1][i], mds_in[i]);
			for(int i=0; i<d; i++) mds1_out[i] = AddModMersenne(f1_out[i], mds_in[i]);
			f1_out[0] = AddModMersenne(f1_out[0], c1);
			SQ_dshares(f1_out, sq2_out);
			for(int i=0; i<d; i++) f2_out[i] = AddModMersenne(sq2_out[i], mds1_out[i]);
			for(int i=0; i<d; i++) swap0[i] = AddModMersenne(state[2][i], f1_out[i]);
			for(int i=0; i<d; i++) swap1[i] = AddModMersenne(state[3][i], f2_out[i]);
			state[3][0] = sq1_in;
			for(int i=1; i<d; i++) state[3][i] = state[1][i];
			for(int i=0; i<d; i++) state[2][i] = state[0][i];
			for(int i=0; i<d; i++) state[1][i] = swap1[i];
			for(int i=0; i<d; i++) state[0][i] = swap0[i];
		}
	}

	// Final tweakey addition
	for(int i=0; i<4; i++) ciphertext[i][0] = AddModMersenne(state[i][0], AddModMersenne(round_tweaks[steps][i], key[i][0]));
	for(int i=1; i<d; i++)
	{
		for(int j=0; j<4; j++)
		{
			ciphertext[j][i] = AddModMersenne(state[j][i], key[j][i]);
		}
	}
}

void __attribute__((noinline)) Decrypt(uint32_t **ciphertext, uint32_t **key, const uint32_t* tweak, uint32_t **plaintext)
{
	uint64_t pi = 0xC90FDAA22168C234;

	// Compute round tweaks
	uint32_t round_tweaks[steps+1][4];
	for(int i=0; i<4; i++) round_tweaks[0][i] = tweak[i];
	for(int step=0; step<steps; step++)
	{
		uint32_t round_tweak_tmp[4];
		for(int i=0; i<4; i++) round_tweak_tmp[i] = round_tweaks[step][i];
		for(int i=0; i<4; i++)
		{
			uint32_t round_tweak_psi[4];
			for(int j=0; j<4; j++) round_tweak_psi[j] = Psi(round_tweak_tmp[j]);
			round_tweak_tmp[1] = AddModMersenne(round_tweak_psi[0], round_tweak_psi[3]);
			round_tweak_tmp[0] = AddModMersenne(round_tweak_psi[0], round_tweak_tmp[1]);
			round_tweak_tmp[3] = AddModMersenne(round_tweak_psi[2], round_tweak_psi[1]);
			round_tweak_tmp[2] = AddModMersenne(round_tweak_psi[2], round_tweak_tmp[3]);
		}
		for(int i=0; i<4; i++) round_tweaks[step+1][i] = round_tweak_tmp[i];
	}

	uint32_t state[4][d];
	for(int i=0; i<d; i++)
	{
		for(int j=0; j<4; j++)
		{
			state[j][i] = ciphertext[j][i];
		}
	}

	// Steps
	for(int step=0; step<steps; step++)
	{
		// Add tweakey
		for(int i=0; i<4; i++) state[i][0] = SubModMersenne(state[i][0], AddModMersenne(round_tweaks[steps-step][i], key[i][0]));
		for(int i=1; i<d; i++)
		{
			for(int j=0; j<4; j++)
			{
				state[j][i] = SubModMersenne(state[j][i], key[j][i]);
			}
		}

		// Multiple rounds per step
		for(int round=0; round<rounds_per_step; round++)
		{
			// Round constants
			pi = (pi << 63) + (pi >> 1);
			uint32_t c0 = pi & p;
			uint32_t c1 = (pi >> 32) & p;

			// Apply F function + Feistel
			uint32_t sq1_in, sq1_out[d], mds_in[d], f1_out[d], mds1_out[d], sq2_out[d], f2_out[d], swap0[d], swap1[d];
			sq1_in = state[3][0];
			state[3][0] = AddModMersenne(state[3][0], c0);
			SQ_dshares(state[3], sq1_out);
			for(int i=0; i<d; i++) mds_in[i] = AddModMersenne(sq1_out[i], state[2][i]);
			f1_out[0] = AddModMersenne(state[3][0], mds_in[0]);
			for(int i=1; i<d; i++) f1_out[i] = AddModMersenne(state[3][i], mds_in[i]);
			for(int i=0; i<d; i++) mds1_out[i] = AddModMersenne(f1_out[i], mds_in[i]);
			f1_out[0] = AddModMersenne(f1_out[0], c1);
			SQ_dshares(f1_out, sq2_out);
			for(int i=0; i<d; i++) f2_out[i] = AddModMersenne(sq2_out[i], mds1_out[i]);
			for(int i=0; i<d; i++) swap0[i] = SubModMersenne(state[0][i], f1_out[i]);
			for(int i=0; i<d; i++) swap1[i] = SubModMersenne(state[1][i], f2_out[i]);
			state[1][0] = sq1_in;
			for(int i=1; i<d; i++) state[1][i] = state[3][i];
			for(int i=0; i<d; i++) state[0][i] = state[2][i];
			for(int i=0; i<d; i++) state[3][i] = swap1[i];
			for(int i=0; i<d; i++) state[2][i] = swap0[i];
		}
	}

	// Final tweakey addition
	for(int i=0; i<4; i++) plaintext[i][0] = SubModMersenne(state[i][0], AddModMersenne(round_tweaks[0][i], key[i][0]));
	for(int i=1; i<d; i++)
	{
		for(int j=0; j<4; j++)
		{
			plaintext[j][i] = SubModMersenne(state[j][i], key[j][i]);
		}
	}
}


int main(void)
{
	// Test Vector 1
	const uint32_t plaintext0[4]  = {0x78066d6b, 0x68a24eb4, 0x2d12aacd, 0x42bb7df4};
	const uint32_t key0[4]        = {0x3a85ecf4, 0x010084b5, 0x28e3f4fb, 0x41514a49};
	const uint32_t tweak0[4]      = {0x7fe52871, 0x510c6891, 0x1ce7b0bf, 0x03640680};
	const uint32_t ciphertext0[4] = {0x47bb5eaa, 0x24082deb, 0x68573cac, 0x0b8c3ecf};

	// Test Vector 2
	const uint32_t plaintext1[4]  = {0x1f3202d0, 0x2ebbcc8c, 0x261b659f, 0x22171a32};
	const uint32_t key1[4]        = {0x6abbc641, 0x44f243a3, 0x7660994b, 0x5c03e803};
	const uint32_t tweak1[4]      = {0x45ce7585, 0x5d27ee4f, 0x101c05fb, 0x1d873632};
	const uint32_t ciphertext1[4] = {0x4619342f, 0x210583de, 0x367d7946, 0x2a1c82e8};

	// Allocate memory for sharings
	uint32_t *shared_plaintext[4], *shared_key[4], *shared_ciphertext[4];
	for(int i=0; i<4; i++)
	{
		shared_plaintext[i] = (uint32_t*)malloc(d * sizeof(uint32_t));
		if(shared_plaintext[i] == NULL) return -1;
		shared_key[i] = (uint32_t*)malloc(d * sizeof(uint32_t));
		if(shared_key[i] == NULL) return -1;
		shared_ciphertext[i] = (uint32_t*)malloc(d * sizeof(uint32_t));
		if(shared_ciphertext[i] == NULL) return -1;
	}

	// Generate sharing for test vector 1
	for(int i=0; i<4; i++)
	{
		shared_plaintext[i][d-1] = plaintext0[i];
		shared_key[i][d-1] = key0[i];
		for(int j=0; j<d-1; j++)
		{
			shared_plaintext[i][j] = RAND_CONST;
			shared_plaintext[i][d-1] = ((shared_plaintext[i][d-1] + p) - shared_plaintext[i][j]) % p;
			shared_key[i][j] = RAND_CONST;
			shared_key[i][d-1] = ((shared_key[i][d-1] + p) - shared_key[i][j]) % p;
		}
	}

	// Encrypt
	Encrypt(shared_plaintext, shared_key, tweak0, shared_ciphertext);

	// Verify test vector 1
	for(int i=0; i<4; i++)
	{
		uint32_t ciphertext_tmp = shared_ciphertext[i][0];
		for(int j=1; j<d; j++)
		{
			ciphertext_tmp = (ciphertext_tmp + shared_ciphertext[i][j]) % p;
		}
		if(ciphertext_tmp != ciphertext0[i]) return -1;
	}


	// Generate sharing for test vector 2
	for(int i=0; i<4; i++)
	{
		shared_ciphertext[i][d-1] = ciphertext1[i];
		for(int j=0; j<d-1; j++)
		{
			shared_ciphertext[i][j] = RAND_CONST;
			shared_ciphertext[i][d-1] = ((shared_ciphertext[i][d-1] + p) - shared_ciphertext[i][j]) % p;
		}
		shared_key[i][d-1] = key1[i];
		for(int j=0; j<d-1; j++)
		{
			shared_key[i][j] = RAND_CONST;
			shared_key[i][d-1] = ((shared_key[i][d-1] + p) - shared_key[i][j]) % p;
		}
	}

	// Decrypt
	Decrypt(shared_ciphertext, shared_key, tweak1, shared_plaintext);

	// Verify test vector 2
	for(int i=0; i<4; i++)
	{
		uint32_t plaintext_tmp = shared_plaintext[i][0];
		for(int j=1; j<d; j++)
		{
			plaintext_tmp = (plaintext_tmp + shared_plaintext[i][j]) % p;
		}
		if(plaintext_tmp != plaintext1[i]) return -1;
	}

	// Free memory
	for(int i=0; i<4; i++)
	{
		free(shared_plaintext[i]);
		free(shared_key[i]);
		free(shared_ciphertext[i]);
	}

	return 0;
}
