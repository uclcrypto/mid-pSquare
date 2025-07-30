#include <stdint.h>

#define n 7
#define p ((1 << n) - 1)
#define RedModMersenne(a) ((a & p) + (a >> n))
#define AddModMersenne(a, b) RedModMersenne((a + b))
#define SubModMersenne(a, b) RedModMersenne((a + (b ^ p)))
#define SquAddModMersenne(a, b) RedModMersenne(RedModMersenne((a * a + b)))
#define Psi0(a) ((((a >> 0) & 0x1) << 5) | (((a >> 1) & 0x1) << 3) | (((a >> 2) & 0x1) << 0) | (((a >> 3) & 0x1) << 4) | (((a >> 4) & 0x1) << 1) | (((a >> 5) & 0x1) << 6) | (((a >> 6) & 0x1) << 2))
#define Psi1(a) ((((a >> 6) & 0x1) << 5) | (((a >> 0) & 0x1) << 3) | (((a >> 1) & 0x1) << 0) | (((a >> 2) & 0x1) << 4) | (((a >> 3) & 0x1) << 1) | (((a >> 4) & 0x1) << 6) | (((a >> 5) & 0x1) << 2))
#define Psi2(a) ((((a >> 5) & 0x1) << 5) | (((a >> 6) & 0x1) << 3) | (((a >> 0) & 0x1) << 0) | (((a >> 1) & 0x1) << 4) | (((a >> 2) & 0x1) << 1) | (((a >> 3) & 0x1) << 6) | (((a >> 4) & 0x1) << 2))
#define Psi3(a) ((((a >> 4) & 0x1) << 5) | (((a >> 5) & 0x1) << 3) | (((a >> 6) & 0x1) << 0) | (((a >> 0) & 0x1) << 4) | (((a >> 1) & 0x1) << 1) | (((a >> 2) & 0x1) << 6) | (((a >> 3) & 0x1) << 2))
#define Psi4(a) ((((a >> 3) & 0x1) << 5) | (((a >> 4) & 0x1) << 3) | (((a >> 5) & 0x1) << 0) | (((a >> 6) & 0x1) << 4) | (((a >> 0) & 0x1) << 1) | (((a >> 1) & 0x1) << 6) | (((a >> 2) & 0x1) << 2))
#define Psi5(a) ((((a >> 2) & 0x1) << 5) | (((a >> 3) & 0x1) << 3) | (((a >> 4) & 0x1) << 0) | (((a >> 5) & 0x1) << 4) | (((a >> 6) & 0x1) << 1) | (((a >> 0) & 0x1) << 6) | (((a >> 1) & 0x1) << 2))
#define Psi6(a) ((((a >> 1) & 0x1) << 5) | (((a >> 2) & 0x1) << 3) | (((a >> 3) & 0x1) << 0) | (((a >> 4) & 0x1) << 4) | (((a >> 5) & 0x1) << 1) | (((a >> 6) & 0x1) << 6) | (((a >> 0) & 0x1) << 2))
#define rounds_per_step 4
#define steps 21

void __attribute__((noinline)) Encrypt(const uint8_t *plaintext, const uint8_t* key, const uint8_t* tweak0, const uint8_t* tweak1, uint8_t *ciphertext);
void __attribute__((noinline)) Decrypt(const uint8_t *ciphertext, const uint8_t* key, const uint8_t* tweak0, const uint8_t* tweak1, uint8_t *plaintext);

void __attribute__((noinline)) Encrypt(const uint8_t *plaintext, const uint8_t* key, const uint8_t* tweak0, const uint8_t* tweak1, uint8_t *ciphertext)
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

	uint32_t state[16];
	for(int i=0; i<16; i++) state[i] = plaintext[i];

	// Steps
	for(int step=0; step<steps; step++)
	{
		// Add tweakey
		for(int i=0; i<16; i++) state[i] = AddModMersenne(state[i], AddModMersenne(round_tweaks[step][i], key[i]));

		// Multiple rounds per step
		for(int round=0; round<rounds_per_step; round++)
		{
			// Round constants
			uint8_t c0 = (pi & p);
			uint8_t c1 = ((pi >> 48) & p);
			uint8_t c2 = ((pi >> 32) & p);
			uint8_t c3 = ((pi >> 16) & p);
			pi = (pi << 1) + (pi >> 63);

			uint8_t f_out1[4];
			uint8_t f_out2[4];

			// Apply F function 1
			uint8_t sq1_in = AddModMersenne(state[3], c0);
			uint8_t mds2_in = SquAddModMersenne(sq1_in, state[2]);
			uint8_t mds3_in = SquAddModMersenne(state[2], state[1]);
			uint8_t mds4_in = SquAddModMersenne(state[1], state[0]);

			uint8_t add1_12 = AddModMersenne(sq1_in, mds2_in);
			uint8_t add1_34 = AddModMersenne(mds3_in, mds4_in);
			uint8_t add2_234 = AddModMersenne(mds2_in, add1_34);
			uint8_t add2_124 = AddModMersenne(mds4_in, add1_12);
			uint8_t add1_12_2 = ((add1_12 << 1) & p) + (add1_12 >> (n-1));
			uint8_t add1_34_2 = ((add1_34 << 1) & p) + (add1_34 >> (n-1));
			uint8_t mds4_out = AddModMersenne(add1_34_2, add2_124);
			uint8_t mds2_out = AddModMersenne(add1_12_2, add2_234);
			uint8_t add2_124_4 = ((add2_124 << 2) & p) + (add2_124 >> (n-2));
			uint8_t add2_234_4 = ((add2_234 << 2) & p) + (add2_234 >> (n-2));
			uint8_t mds1_out = AddModMersenne(add2_124_4, mds2_out);
			uint8_t mds3_out = AddModMersenne(add2_234_4, mds4_out);

			f_out1[3] = AddModMersenne(mds1_out, c1);
			f_out1[0] = SquAddModMersenne(f_out1[3], mds2_out);
			f_out1[1] = SquAddModMersenne(mds2_out, mds3_out);
			f_out1[2] = SquAddModMersenne(mds3_out, mds4_out);

			// Apply F function 2
			sq1_in = AddModMersenne(state[11], c2);
			mds2_in = SquAddModMersenne(sq1_in, state[10]);
			mds3_in = SquAddModMersenne(state[10], state[9]);
			mds4_in = SquAddModMersenne(state[9], state[8]);

			add1_12 = AddModMersenne(sq1_in, mds2_in);
			add1_34 = AddModMersenne(mds3_in, mds4_in);
			add2_234 = AddModMersenne(mds2_in, add1_34);
			add2_124 = AddModMersenne(mds4_in, add1_12);
			add1_12_2 = ((add1_12 << 1) & p) + (add1_12 >> (n-1));
			add1_34_2 = ((add1_34 << 1) & p) + (add1_34 >> (n-1));
			mds4_out = AddModMersenne(add1_34_2, add2_124);
			mds2_out = AddModMersenne(add1_12_2, add2_234);
			add2_124_4 = ((add2_124 << 2) & p) + (add2_124 >> (n-2));
			add2_234_4 = ((add2_234 << 2) & p) + (add2_234 >> (n-2));
			mds1_out = AddModMersenne(add2_124_4, mds2_out);
			mds3_out = AddModMersenne(add2_234_4, mds4_out);

			f_out2[3] = AddModMersenne(mds1_out, c3);
			f_out2[0] = SquAddModMersenne(f_out2[3], mds2_out);
			f_out2[1] = SquAddModMersenne(mds2_out, mds3_out);
			f_out2[2] = SquAddModMersenne(mds3_out, mds4_out);

			// Apply Feistel
			uint8_t tmp[16];
			for(int i=0; i<4; i++)
			{
				tmp[i] = AddModMersenne(state[i+4], f_out1[3-i]);
				tmp[i+4] = state[i+8];
				tmp[i+8] = AddModMersenne(state[i+12], f_out2[3-i]);
				tmp[i+12] = state[i];
			}
			for(int i=0; i<16; i++) state[i] = tmp[i];
		}
	}

	// Final tweakey addition
	for(int i=0; i<16; i++) ciphertext[i] = AddModMersenne(state[i], AddModMersenne(round_tweaks[steps][i], key[i]));
}

void __attribute__((noinline)) Decrypt(const uint8_t *ciphertext, const uint8_t* key, const uint8_t* tweak0, const uint8_t* tweak1, uint8_t *plaintext)
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

	uint32_t state[16];
	for(int i=0; i<16; i++) state[i] = ciphertext[i];

	// Steps
	for(int step=0; step<steps; step++)
	{
		// Add tweakey
		for(int i=0; i<16; i++) state[i] = SubModMersenne(state[i], AddModMersenne(round_tweaks[steps-step][i], key[i]));

		// Multiple rounds per step
		for(int round=0; round<rounds_per_step; round++)
		{
			// Round constants
			pi = (pi << 63) + (pi >> 1);
			uint8_t c0 = (pi & p);
			uint8_t c1 = ((pi >> 48) & p);
			uint8_t c2 = ((pi >> 32) & p);
			uint8_t c3 = ((pi >> 16) & p);

			uint8_t f_out1[4];
			uint8_t f_out2[4];

			// Apply F function 1
			uint8_t sq1_in = AddModMersenne(state[15], c0);
			uint8_t mds2_in = SquAddModMersenne(sq1_in, state[14]);
			uint8_t mds3_in = SquAddModMersenne(state[14], state[13]);
			uint8_t mds4_in = SquAddModMersenne(state[13], state[12]);

			uint8_t add1_12 = AddModMersenne(sq1_in, mds2_in);
			uint8_t add1_34 = AddModMersenne(mds3_in, mds4_in);
			uint8_t add2_234 = AddModMersenne(mds2_in, add1_34);
			uint8_t add2_124 = AddModMersenne(mds4_in, add1_12);
			uint8_t add1_12_2 = ((add1_12 << 1) & p) + (add1_12 >> (n-1));
			uint8_t add1_34_2 = ((add1_34 << 1) & p) + (add1_34 >> (n-1));
			uint8_t mds4_out = AddModMersenne(add1_34_2, add2_124);
			uint8_t mds2_out = AddModMersenne(add1_12_2, add2_234);
			uint8_t add2_124_4 = ((add2_124 << 2) & p) + (add2_124 >> (n-2));
			uint8_t add2_234_4 = ((add2_234 << 2) & p) + (add2_234 >> (n-2));
			uint8_t mds1_out = AddModMersenne(add2_124_4, mds2_out);
			uint8_t mds3_out = AddModMersenne(add2_234_4, mds4_out);

			f_out1[3] = AddModMersenne(mds1_out, c1);
			f_out1[0] = SquAddModMersenne(f_out1[3], mds2_out);
			f_out1[1] = SquAddModMersenne(mds2_out, mds3_out);
			f_out1[2] = SquAddModMersenne(mds3_out, mds4_out);

			// Apply F function 2
			sq1_in = AddModMersenne(state[7], c2);
			mds2_in = SquAddModMersenne(sq1_in, state[6]);
			mds3_in = SquAddModMersenne(state[6], state[5]);
			mds4_in = SquAddModMersenne(state[5], state[4]);

			add1_12 = AddModMersenne(sq1_in, mds2_in);
			add1_34 = AddModMersenne(mds3_in, mds4_in);
			add2_234 = AddModMersenne(mds2_in, add1_34);
			add2_124 = AddModMersenne(mds4_in, add1_12);
			add1_12_2 = ((add1_12 << 1) & p) + (add1_12 >> (n-1));
			add1_34_2 = ((add1_34 << 1) & p) + (add1_34 >> (n-1));
			mds4_out = AddModMersenne(add1_34_2, add2_124);
			mds2_out = AddModMersenne(add1_12_2, add2_234);
			add2_124_4 = ((add2_124 << 2) & p) + (add2_124 >> (n-2));
			add2_234_4 = ((add2_234 << 2) & p) + (add2_234 >> (n-2));
			mds1_out = AddModMersenne(add2_124_4, mds2_out);
			mds3_out = AddModMersenne(add2_234_4, mds4_out);

			f_out2[3] = AddModMersenne(mds1_out, c3);
			f_out2[0] = SquAddModMersenne(f_out2[3], mds2_out);
			f_out2[1] = SquAddModMersenne(mds2_out, mds3_out);
			f_out2[2] = SquAddModMersenne(mds3_out, mds4_out);

			// Apply Feistel
			uint8_t tmp[16];
			for(int i=0; i<4; i++)
			{
				tmp[i] = state[i+12];
				tmp[i+4] = SubModMersenne(state[i], f_out1[3-i]);
				tmp[i+8] = state[i+4];
				tmp[i+12] = SubModMersenne(state[i+8], f_out2[3-i]);
			}
			for(int i=0; i<16; i++) state[i] = tmp[i];
		}
	}

	// Final tweakey addition
	for(int i=0; i<16; i++) plaintext[i] = SubModMersenne(state[i], AddModMersenne(round_tweaks[0][i], key[i]));
}


int main(void)
{
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

	uint8_t ciphertext[16];
	Encrypt(plaintext0, key0, tweak0_0, tweak0_1, ciphertext);
	for(int i=0; i<16; i++)
	{
		if(ciphertext[i] != ciphertext0[i]) return -1;
	}

	uint8_t plaintext[16];
	Decrypt(ciphertext1, key1, tweak1_0, tweak1_1, plaintext);
	for(int i=0; i<16; i++)
	{
		if(plaintext[i] != plaintext1[i]) return -1;
	}

	return 0;
}
