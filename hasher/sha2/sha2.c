/**
 * @file sha2.c
 * An implementation of the SHA2 collection of algorithms (SHA256, SHA224,
 *  SHA512, SHA384), described in FIPS 180-2 (and after)
 * @author	FergoFrog <fergofrog@fergofrog.com>
 * @version 0.3
 *
 * @section LICENSE
 * Copyright (C) 2011 FergoFrog
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>

#include "../global.h"
#include "sha2.h"

#define SHA2_CH(X, Y, Z) (((X) & (Y)) ^ (~(X) & (Z)))
#define SHA2_MAJ(X, Y, Z) (((X) & (Y)) ^ ((X) & (Z)) ^ ((Y) & (Z)))

#define SHA2_I_SIG_0(X) (i_r_rot((X), 2) ^ i_r_rot((X), 13) ^ i_r_rot((X), 22))
#define SHA2_I_SIG_1(X) (i_r_rot((X), 6) ^ i_r_rot((X), 11) ^ i_r_rot((X), 25))
#define SHA2_I_LSIG_0(X) (i_r_rot((X), 7) ^ i_r_rot((X), 18) ^ ((X) >> 3))
#define SHA2_I_LSIG_1(X) (i_r_rot((X), 17) ^ i_r_rot((X), 19) ^ ((X) >> 10))

#define SHA2_LL_SIG_0(X) (ll_r_rot((X), 28) ^ ll_r_rot((X), 34) ^ ll_r_rot((X), 39))
#define SHA2_LL_SIG_1(X) (ll_r_rot((X), 14) ^ ll_r_rot((X), 18) ^ ll_r_rot((X), 41))
#define SHA2_LL_LSIG_0(X) (ll_r_rot((X), 1) ^ ll_r_rot((X), 8) ^ ((X) >> 7ULL))
#define SHA2_LL_LSIG_1(X) (ll_r_rot((X), 19) ^ ll_r_rot((X), 61) ^ ((X) >> 6ULL))

const unsigned int sha2_i_operation_constants[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const unsigned long long sha2_ll_operation_constants[80] = {
		0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
		0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
		0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
		0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
		0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
		0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
		0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
		0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
		0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
		0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
		0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
		0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
		0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
		0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
		0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
		0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
		0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
		0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
		0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
		0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

extern unsigned int i_hash[8];
extern unsigned long long ll_hash[8];
extern unsigned long long hash_length, hash_length2;
extern char in_hash;
extern unsigned char cur_chunk[80][8], cur_chunk_pos;

enum sha2_t sha2_type;

void sha2_add_chunk();

char sha2_init(enum sha2_t type)
{
	if (!in_hash) {
		switch (type) {
		case SHA256:
			i_hash[0] = 0x6A09E667;
			i_hash[1] = 0xBB67AE85;
			i_hash[2] = 0x3C6EF372;
			i_hash[3] = 0xA54FF53A;
			i_hash[4] = 0x510E527F;
			i_hash[5] = 0x9B05688C;
			i_hash[6] = 0x1F83D9AB;
			i_hash[7] = 0x5BE0CD19;
			break;
		case SHA224:
			i_hash[0] = 0xC1059ED8;
			i_hash[1] = 0x367CD507;
			i_hash[2] = 0x3070DD17;
			i_hash[3] = 0xF70E5939;
			i_hash[4] = 0xFFC00B31;
			i_hash[5] = 0x68581511;
			i_hash[6] = 0x64F98FA7;
			i_hash[7] = 0xBEFA4FA4;
			break;
		case SHA512:
			ll_hash[0] = 0x6a09e667f3bcc908;
			ll_hash[1] = 0xbb67ae8584caa73b;
			ll_hash[2] = 0x3c6ef372fe94f82b;
			ll_hash[3] = 0xa54ff53a5f1d36f1;
			ll_hash[4] = 0x510e527fade682d1;
			ll_hash[5] = 0x9b05688c2b3e6c1f;
			ll_hash[6] = 0x1f83d9abfb41bd6b;
			ll_hash[7] = 0x5be0cd19137e2179;
			break;
		case SHA384:
			ll_hash[0] = 0xcbbb9d5dc1059ed8;
			ll_hash[1] = 0x629a292a367cd507;
			ll_hash[2] = 0x9159015a3070dd17;
			ll_hash[3] = 0x152fecd8f70e5939;
			ll_hash[4] = 0x67332667ffc00b31;
			ll_hash[5] = 0x8eb44a8768581511;
			ll_hash[6] = 0xdb0c2e0d64f98fa7;
			ll_hash[7] = 0x47b5481dbefa4fa4;
			break;
		}

		sha2_type = type;

		hash_length = 0;
		hash_length2 = 0;
		cur_chunk_pos = 0;

		in_hash = 1;

		return 1;
	} else {
		return 0;
	}
}

char sha2_add_string(char *str)
{
	if (in_hash) {
		int i, bytes_per_word;

		if (sha2_type == SHA256 || sha2_type == SHA224) {
			bytes_per_word = 4;
		} else {
			bytes_per_word = 8;
		}

		for (i = 0; str[i] != '\0'; i++) {
			cur_chunk[cur_chunk_pos / bytes_per_word][cur_chunk_pos % bytes_per_word] = str[i];
			cur_chunk_pos++;
			hash_length += 8;

			if (hash_length == 0) {
				/* Overflowed */
				hash_length2 += 1;
			}

			if (cur_chunk_pos >= 16 * bytes_per_word) {
				sha2_add_chunk();
				cur_chunk_pos = 0;
			}
		}

		return 1;
	} else {
		return 0;
	}
}

char sha2_add_file(FILE *fp)
{
	if (in_hash) {
		int c = fgetc(fp), bytes_per_word;

		if (sha2_type == SHA256 || sha2_type == SHA224) {
			bytes_per_word = 4;
		} else {
			bytes_per_word = 8;
		}

		while (c != EOF) {
			cur_chunk[cur_chunk_pos / bytes_per_word][cur_chunk_pos % bytes_per_word] = (unsigned char) c;
			cur_chunk_pos++;
			hash_length += 8;

			if (hash_length == 0) {
				/* Overflowed */
				hash_length2 += 1;
			}

			if (cur_chunk_pos >= 16 * bytes_per_word) {
				sha2_add_chunk();
				cur_chunk_pos = 0;
			}

			c = fgetc(fp);
		};

		return 1;
	} else {
		return 0;
	}
}

char sha2_get_hash(unsigned long long hash_out[])
{
	if (in_hash) {
		int bytes_per_word;

		if (sha2_type == SHA256 || sha2_type == SHA224) {
			bytes_per_word = 4;
		} else {
			bytes_per_word = 8;
		}

		/* Begin appending */
		cur_chunk[cur_chunk_pos / bytes_per_word][cur_chunk_pos % bytes_per_word] = 0x80;
		cur_chunk_pos++;

		/* If the chunk pos is over 448 bits (56 bytes) - complete the current chunk */
		if (cur_chunk_pos > (16 * bytes_per_word) - (2* bytes_per_word)) {
			while (cur_chunk_pos < 16 * bytes_per_word) {
				cur_chunk[cur_chunk_pos / bytes_per_word][cur_chunk_pos % bytes_per_word] = 0x00;
				cur_chunk_pos++;
			}

			sha2_add_chunk();
			cur_chunk_pos = 0;
		}

		/* Append 0's up until the 56th byte of the chunk (leaving 8 bytes for the length) */
		while (cur_chunk_pos < (16 * bytes_per_word) - (2 * bytes_per_word)) {
			cur_chunk[cur_chunk_pos / bytes_per_word][cur_chunk_pos % bytes_per_word] = 0x00;
			cur_chunk_pos++;
		}

		/* Append length */
		unsigned char length_b[16];
		if (sha2_type == SHA256 || sha2_type == SHA224) {
			be_ll_to_b(hash_length, length_b);
		} else {
			be_llll_to_b(hash_length2, hash_length, length_b);
		}
		while (cur_chunk_pos < 16 * bytes_per_word) {
			cur_chunk[cur_chunk_pos / bytes_per_word][cur_chunk_pos % bytes_per_word] = length_b[cur_chunk_pos - ((16 * bytes_per_word) - (2 * bytes_per_word))];
			cur_chunk_pos++;
		}

		/* Process the chunk */
		sha2_add_chunk();

		/* Copy the hash over */
		if (sha2_type == SHA256 || sha2_type == SHA224) {
			hash_out[0] = i_hash[0];
			hash_out[1] = i_hash[1];
			hash_out[2] = i_hash[2];
			hash_out[3] = i_hash[3];
			hash_out[4] = i_hash[4];
			hash_out[5] = i_hash[5];
			hash_out[6] = i_hash[6];
			hash_out[7] = i_hash[7];
		} else {
			hash_out[0] = ll_hash[0];
			hash_out[1] = ll_hash[1];
			hash_out[2] = ll_hash[2];
			hash_out[3] = ll_hash[3];
			hash_out[4] = ll_hash[4];
			hash_out[5] = ll_hash[5];
			hash_out[6] = ll_hash[6];
			hash_out[7] = ll_hash[7];
		}

		in_hash = 0;

		return 1;
	} else {
		return 0;
	}
}

void sha2_add_chunk()
{
	int i;

	if (sha2_type == SHA256 || sha2_type == SHA224) {
		unsigned int temp[2];
		unsigned int words[64];

		unsigned int a = i_hash[0], b = i_hash[1], c = i_hash[2], d = i_hash[3];
		unsigned int e = i_hash[4], f = i_hash[5], g = i_hash[6], h = i_hash[7];

		for (i = 0; i < 16; i++) {
			words[i] = be_i_b_to_w(cur_chunk[i]);
		}

		for (i = 16; i < 64; i++) {
			words[i] = SHA2_I_LSIG_1(words[i - 2]) + words[i - 7] + SHA2_I_LSIG_0(words[i - 15]) + words[i - 16];
		}

		for (i = 0; i < 64; i++) {
			temp[0] = h + SHA2_I_SIG_1(e) + SHA2_CH(e, f, g) + sha2_i_operation_constants[i] + words[i];
			temp[1] = SHA2_I_SIG_0(a) + SHA2_MAJ(a, b, c);

			h = g;
			g = f;
			f = e;
			e = d + temp[0];
			d = c;
			c = b;
			b = a;
			a = temp[0] + temp[1];
		}

		i_hash[0] += a;
		i_hash[1] += b;
		i_hash[2] += c;
		i_hash[3] += d;
		i_hash[4] += e;
		i_hash[5] += f;
		i_hash[6] += g;
		i_hash[7] += h;
	} else {
		unsigned long long temp[2];
		unsigned long long words[80];

		unsigned long long a = ll_hash[0], b = ll_hash[1], c = ll_hash[2], d = ll_hash[3];
		unsigned long long e = ll_hash[4], f = ll_hash[5], g = ll_hash[6], h = ll_hash[7];

		for (i = 0; i < 16; i++) {
			words[i] = be_ll_b_to_w(cur_chunk[i]);
		}

		for (i = 16; i < 80; i++) {
			words[i] = SHA2_LL_LSIG_1(words[i - 2]) + words[i - 7] + SHA2_LL_LSIG_0(words[i - 15]) + words[i - 16];
		}

		for (i = 0; i < 80; i++) {
			temp[0] = h + SHA2_LL_SIG_1(e) + SHA2_CH(e, f, g) + sha2_ll_operation_constants[i] + words[i];
			temp[1] = SHA2_LL_SIG_0(a) + SHA2_MAJ(a, b, c);

			h = g;
			g = f;
			f = e;
			e = d + temp[0];
			d = c;
			c = b;
			b = a;
			a = temp[0] + temp[1];
		}

		ll_hash[0] += a;
		ll_hash[1] += b;
		ll_hash[2] += c;
		ll_hash[3] += d;
		ll_hash[4] += e;
		ll_hash[5] += f;
		ll_hash[6] += g;
		ll_hash[7] += h;
	}
}
