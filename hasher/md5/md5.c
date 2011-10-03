/**
 * @file md5.c
 * An implementation of the MD5 algorithm, described in RFC 1321
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
/* Includes */
#include <stdio.h>

#include "../global.h"
#include "md5.h"

/** Per-round function F (0 <= r < 16) */
#define MD5_FUNC_F(X, Y, Z) (((X) & (Y)) | (~(X) & (Z)))
/** Per-round function G (16 <= r < 32) */
#define MD5_FUNC_G(X, Y, Z) (((X) & (Z)) | ((Y) & ~(Z)))
/** Per-round function H (32 <= r < 48) */
#define MD5_FUNC_H(X, Y, Z) ((X) ^ (Y) ^ (Z))
/** Per-round function I (48 <= r < 64) */
#define MD5_FUNC_I(X, Y, Z) ((Y) ^ ((X) | ~(Z)))

/** Per-round rotate amounts - from RFC 1321 */
const unsigned int rotate_amounts[4][4] =
	{{ 7, 12, 17, 22},
	 { 5,  9, 14, 20},
	 { 4, 11, 16, 23},
	 { 6, 10, 15, 21}};
/** Per-round addition constants - from RFC 1321 */
const unsigned int operation_constants[64] =
	{0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

extern unsigned int i_hash[8];
extern unsigned long long hash_length;
extern char in_hash;
extern unsigned char cur_chunk[80][8];
extern unsigned int cur_chunk_pos;

void md5_add_chunk();

/**
 * Initialise MD5 hashing
 *
 * @return 1 if MD5 hashing was initialised (nothing else initialised), else 0
 */
char md5_init()
{
	/* Begin hashing if not currently hashing */
	if (!in_hash) {
		/* Initialise the hash variables - from RFC 1321 */
		i_hash[0] = 0x67452301;
		i_hash[1] = 0xEFCDAB89;
		i_hash[2] = 0x98BADCFE;
		i_hash[3] = 0x10325476;

		/* Initialise length and position variables */
		hash_length = 0;
		cur_chunk_pos = 0;

		/* Now in hash */
		in_hash = 1;

		return 1;
	} else {
		return 0;
	}
}

/**
 * Add a string into the current chunk
 *
 * @param str Null terminated string
 * @return 1 if the string was added, else 0
 */
char md5_add_string(char *str)
{
	/* TODO: Ensure the hash type is MD5 */
	/* Ensure we're currently hashing */
	if (in_hash) {
		int i;
		/*
		 * Loop through the string
		 * character by character until null is reached
		 */
		for (i = 0; str[i] != '\0'; i++) {
			/*
			 * Convert the chunk position into the 2d chunk address
			 *  and store the byte
			 */
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = str[i];
			/* Increment the chunk position */
			cur_chunk_pos++;
			/* Add 8 bits to the length */
			hash_length += 8;

			/*
			 * If 64 bytes have been added, the chunk has
			 *  been filled - process it
			 */
			if (cur_chunk_pos >= 64) {
				md5_add_chunk();
				cur_chunk_pos = 0;
			}
		}

		return 1;
	} else {
		return 0;
	}
}

/**
 * Add a file into the current chunk
 *
 * @param fp File pointer to the file to be read from
 * @return 1 if the file's contents was added, else 0
 */
char md5_add_file(FILE *fp)
{
	/* Ensure we're currently hashing */
	if (in_hash) {
		/* Get the first character */
		int c = fgetc(fp);
		/* Loop until end-of-file is reached */
		while (c != EOF) {
			/*
			 * Convert the chunk position into the 2d chunk address
			 *  and store the byte
			 */
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = (unsigned char)c;
			/* Increment the chunk position */
			cur_chunk_pos++;
			/* Add 8 bits to the length */
			hash_length += 8;

			/*
			 * If 64 bytes have been added, the chunk has
			 *  been filled - process it
			 */
			if (cur_chunk_pos >= 64) {
				md5_add_chunk();
				cur_chunk_pos = 0;
			}

			/* Get the next character */
			c = fgetc(fp);
		};

		return 1;
	} else {
		return 0;
	}
}

/**
 * Complete hashing and get a copy of the hash
 *
 * @param hash_out Array of at least 4 unsigned ints
 * @return 1 if the hash was copied, else 0
 */
char md5_get_hash(unsigned int hash_out[])
{
	if (in_hash) {
		/* Begin completing the hash by appending 0b10000000 */
		cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = 0x80;
		cur_chunk_pos++;

		/*
		 * If the chunk pos is over 448 bits (56 bytes)
		 *  complete the current chunk
		 */
		if (cur_chunk_pos > 56) {
			/* Append 0's until the chunk is completed */
			while (cur_chunk_pos < 64) {
				cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = 0x00;
				cur_chunk_pos++;
			}

			/* Process the chunk */
			md5_add_chunk();
			cur_chunk_pos = 0;
		}

		/* Append 0's up until the 56th byte of the chunk
		 * (leaving 8 bytes for the length)
		 */
		while (cur_chunk_pos < 56) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = 0x00;
			cur_chunk_pos++;
		}

		/* Convert length into a byte array */
		unsigned char length_b[8];
		le_ll_to_b(hash_length, length_b);
		/* Append the length byte array until the end */
		while (cur_chunk_pos < 64) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] =
					length_b[cur_chunk_pos - 56];
			cur_chunk_pos++;
		}

		/* Process the chunk */
		md5_add_chunk();

		/* Copy the hash over */
		hash_out[0] = i_hash[0];
		hash_out[1] = i_hash[1];
		hash_out[2] = i_hash[2];
		hash_out[3] = i_hash[3];

		/* End hashing */
		in_hash = 0;

		return 1;
	} else {
		return 0;
	}
}

/**
 * Process the current chunk
 */
void md5_add_chunk()
{
	unsigned int func_out, word_idx;
	int i;

	/* Copy the current hash into the chunk variables */
	unsigned int a = i_hash[0], b = i_hash[1], c = i_hash[2], d = i_hash[3];

	/* Loop through 64 times */
	for (i = 0; i < 64; i++) {
		/*
		 * Get the function output and determine which word to use -
		 *  according to the current pass
		 */
		if (i >= 0 && i < 16) {
			func_out = MD5_FUNC_F(b, c, d);
			word_idx = i;
		} else if (i >= 16 && i < 32) {
			func_out = MD5_FUNC_G(b, c, d);
			word_idx = (5 * i + 1) % 16;
		} else if (i >= 32 && i < 48) {
			func_out = MD5_FUNC_H(b, c, d);
			word_idx = (3 * i + 5) % 16;
		} else {
			func_out = MD5_FUNC_I(b, c, d);
			word_idx = (7 * i) % 16;
		}

		/* Do the shift and generate the new b */
		unsigned int tmp = d;
		d = c;
		c = b;
		unsigned int b_prerot_sum = a + func_out +
				operation_constants[i] + le_b_to_w(cur_chunk[word_idx]);
		b = b + i_l_rot(b_prerot_sum, rotate_amounts[i / 16][i % 4]);
		a = tmp;
	}

	/* Add the chunk variables back into the current hash */
	i_hash[0] += a;
	i_hash[1] += b;
	i_hash[2] += c;
	i_hash[3] += d;
}
