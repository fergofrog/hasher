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
#include <string.h>

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
/*
const unsigned int rotate_amounts[4][4] =
	{{ 7, 12, 17, 22},
	 { 5,  9, 14, 20},
	 { 4, 11, 16, 23},
	 { 6, 10, 15, 21}};
*/
/** Per-round addition constants - from RFC 1321 */
/*
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
*/

void md5_add_chunk(struct md5_state *);

/**
 * Initialise MD5 hashing
 *
 * @return 1 if MD5 hashing was initialised (nothing else initialised), else 0
 */
char md5_init(struct md5_state *state)
{
	/* Initialise the hash variables - from RFC 1321 */
	state->hash[0] = 0x67452301;
	state->hash[1] = 0xEFCDAB89;
	state->hash[2] = 0x98BADCFE;
	state->hash[3] = 0x10325476;

	/* Initialise length and position variables */
	state->length = 0;
	state->cur_chunk_pos = 0;

	return 1;
}

/**
 * Add a string into the current chunk
 *
 * @param str Null terminated string
 * @return 1 if the string was added, else 0
 */
char md5_add_string(struct md5_state *state, char *str)
{
    unsigned int n;
	unsigned int str_pos;
    size_t str_length;

    str_pos = 0;
    str_length = strlen(str);

    while (str_length > 0) {
        n = min(str_length, MD5_CHUNK_LEN - state->cur_chunk_pos);
        memcpy(((unsigned char *) state->cur_chunk) + state->cur_chunk_pos, str + str_pos, n);

        str_pos += n;
        state->cur_chunk_pos += n;
        str_length -= n;

        if (state->cur_chunk_pos >= 64) {
            md5_add_chunk(state);
            state->cur_chunk_pos = 0;
        }
    }

	return 1;
}

/**
 * Add a file into the current chunk
 *
 * @param fp File pointer to the file to be read from
 * @return 1 if the file's contents was added, else 0
 */
char md5_add_file(struct md5_state *state, FILE *fp)
{
    char complete = 0;
    size_t num_read;

    do {
        num_read = fread(((unsigned char *) state->cur_chunk) + state->cur_chunk_pos, sizeof(unsigned char), MD5_CHUNK_LEN - state->cur_chunk_pos, fp);
        /* Increment the chunk position */
        state->cur_chunk_pos += num_read;
        /* Add to the length */
        state->length += num_read * 8;

        /* If 64 bytes have been added, the chunk has been filled - process it */
        if (state->cur_chunk_pos == MD5_CHUNK_LEN) {
            md5_add_chunk(state);
            state->cur_chunk_pos = 0;
        } else {
            /* The full chunk wasn't filled by fread - check for eof or error */
            if (feof(fp)) {
                complete = 1;
            } else {
                perror("An error in reading the file occurred.");
            }
        }
    } while (!complete);

	return 1;
}

/**
 * Complete hashing and get a copy of the hash
 *
 * @param hash_out Array of at least 4 unsigned ints
 * @return 1 if the hash was copied, else 0
 */
char md5_get_hash(struct md5_state *state, unsigned int hash_out[])
{
	/* Begin completing the hash by appending 0b10000000 */
    *(((unsigned char *) state->cur_chunk) + state->cur_chunk_pos) = 0x80;
	state->cur_chunk_pos++;

	/*
	 * If the chunk pos is over 448 bits (56 bytes)
	 *  complete the current chunk
	 */
	if (state->cur_chunk_pos > 56) {
		/* Append 0's until the chunk is completed */
        memset(((unsigned char *) state->cur_chunk) + state->cur_chunk_pos, 0x00, 64 - state->cur_chunk_pos);

		/* Process the chunk */
		md5_add_chunk(state);
		state->cur_chunk_pos = 0;
	}

	/* Append 0's up until the 56th byte of the chunk
	 * (leaving 8 bytes for the length)
	 */
    memset(((unsigned char *) state->cur_chunk) + state->cur_chunk_pos, 0x00, 56 - state->cur_chunk_pos);
	state->cur_chunk_pos = 56;

	/* Convert length into a byte array */
	unsigned int length_i[8];
	le_ll_to_i(state->length, length_i);
    /* Append the length byte array until the end */
    state->cur_chunk[14] = length_i[0];
    state->cur_chunk[15] = length_i[1];

	/* Process the chunk */
	md5_add_chunk(state);

	/* Copy the hash over */
	hash_out[0] = state->hash[0];
	hash_out[1] = state->hash[1];
	hash_out[2] = state->hash[2];
	hash_out[3] = state->hash[3];

	return 1;
}

/**
 * Complete hashing and get a copy of the hash (as a string)
 *
 * @param hash_out A string of 32 chars to copy the hash into
 * @return 1 if the hash was copied, else 0
 */
char md5_get_hash_str(struct md5_state *state, char hash_out[])
{
	char res, temp[2];
	unsigned int hash_out_i[4], i, j;

	/* Get the hash as a series of unsigned little-endian ints */
	res = md5_get_hash(state, hash_out_i);

	if (!res) return 0;

	/* Get the string representation of the hash */
	sprintf(hash_out, "%08x%08x%08x%08x", hash_out_i[0], hash_out_i[1], hash_out_i[2], hash_out_i[3]);

	/* Swap the characters around */
	/* Outer loop: 4 groups of 8 */
	for (i = 0; i < 4; i++) {
		/* Inner loop: 2 pairs of 2 characters */
		for (j = 0; j < 2; j++) {
			/* Save the left pair */
			temp[0] = hash_out[(i * 8) + (j * 2)];
			temp[1] = hash_out[(i * 8) + (j * 2) + 1];

			/* Copy the right pair across */
			hash_out[(i * 8) + (j * 2)] = hash_out[(i * 8) + (6 - (j * 2))];
			hash_out[(i * 8) + (j * 2) + 1] = hash_out[(i * 8) + (7 - (j * 2))];

			/* Copy the original left to the right */
			hash_out[(i * 8) + (6 - (j * 2))] = temp[0];
			hash_out[(i * 8) + (7 - (j * 2))] = temp[1];
		}
	}

	return 1;
}

#define MD5_STEP(f, w, x, y, z, data, constant, rot)    \
    do {                                                \
        w += f + data + constant;                       \
        w = w << rot | w >> (32 - rot);                 \
        w += x;                                         \
    } while (0)

/**
 * Process the current chunk
 */
void md5_add_chunk(struct md5_state *state)
{
	/* Copy the current hash into the chunk variables */
	unsigned int a = state->hash[0], b = state->hash[1], c = state->hash[2], d = state->hash[3];

    MD5_STEP(MD5_FUNC_F(b,c,d), a, b, c, d, state->cur_chunk[0], 0xd76aa478L, 7);
    MD5_STEP(MD5_FUNC_F(a,b,c), d, a, b, c, state->cur_chunk[1], 0xe8c7b756L, 12);
    MD5_STEP(MD5_FUNC_F(d,a,b), c, d, a, b, state->cur_chunk[2], 0x242070dbL, 17);
    MD5_STEP(MD5_FUNC_F(c,d,a), b, c, d, a, state->cur_chunk[3], 0xc1bdceeeL, 22);
    MD5_STEP(MD5_FUNC_F(b,c,d), a, b, c, d, state->cur_chunk[4], 0xf57c0fafL, 7);
    MD5_STEP(MD5_FUNC_F(a,b,c), d, a, b, c, state->cur_chunk[5], 0x4787c62aL, 12);
    MD5_STEP(MD5_FUNC_F(d,a,b), c, d, a, b, state->cur_chunk[6], 0xa8304613L, 17);
    MD5_STEP(MD5_FUNC_F(c,d,a), b, c, d, a, state->cur_chunk[7], 0xfd469501L, 22);
    MD5_STEP(MD5_FUNC_F(b,c,d), a, b, c, d, state->cur_chunk[8], 0x698098d8L, 7);
    MD5_STEP(MD5_FUNC_F(a,b,c), d, a, b, c, state->cur_chunk[9], 0x8b44f7afL, 12);
    MD5_STEP(MD5_FUNC_F(d,a,b), c, d, a, b, state->cur_chunk[10], 0xffff5bb1L, 17);
    MD5_STEP(MD5_FUNC_F(c,d,a), b, c, d, a, state->cur_chunk[11], 0x895cd7beL, 22);
    MD5_STEP(MD5_FUNC_F(b,c,d), a, b, c, d, state->cur_chunk[12], 0x6b901122L, 7);
    MD5_STEP(MD5_FUNC_F(a,b,c), d, a, b, c, state->cur_chunk[13], 0xfd987193L, 12);
    MD5_STEP(MD5_FUNC_F(d,a,b), c, d, a, b, state->cur_chunk[14], 0xa679438eL, 17);
    MD5_STEP(MD5_FUNC_F(c,d,a), b, c, d, a, state->cur_chunk[15], 0x49b40821L, 22);

    MD5_STEP(MD5_FUNC_G(b,c,d), a, b, c, d, state->cur_chunk[1], 0xf61e2562L, 5);
    MD5_STEP(MD5_FUNC_G(a,b,c), d, a, b, c, state->cur_chunk[6], 0xc040b340L, 9);
    MD5_STEP(MD5_FUNC_G(d,a,b), c, d, a, b, state->cur_chunk[11], 0x265e5a51L, 14);
    MD5_STEP(MD5_FUNC_G(c,d,a), b, c, d, a, state->cur_chunk[0], 0xe9b6c7aaL, 20);
    MD5_STEP(MD5_FUNC_G(b,c,d), a, b, c, d, state->cur_chunk[5], 0xd62f105dL, 5);
    MD5_STEP(MD5_FUNC_G(a,b,c), d, a, b, c, state->cur_chunk[10], 0x02441453L, 9);
    MD5_STEP(MD5_FUNC_G(d,a,b), c, d, a, b, state->cur_chunk[15], 0xd8a1e681L, 14);
    MD5_STEP(MD5_FUNC_G(c,d,a), b, c, d, a, state->cur_chunk[4], 0xe7d3fbc8L, 20);
    MD5_STEP(MD5_FUNC_G(b,c,d), a, b, c, d, state->cur_chunk[9], 0x21e1cde6L, 5);
    MD5_STEP(MD5_FUNC_G(a,b,c), d, a, b, c, state->cur_chunk[14], 0xc33707d6L, 9);
    MD5_STEP(MD5_FUNC_G(d,a,b), c, d, a, b, state->cur_chunk[3], 0xf4d50d87L, 14);
    MD5_STEP(MD5_FUNC_G(c,d,a), b, c, d, a, state->cur_chunk[8], 0x455a14edL, 20);
    MD5_STEP(MD5_FUNC_G(b,c,d), a, b, c, d, state->cur_chunk[13], 0xa9e3e905L, 5);
    MD5_STEP(MD5_FUNC_G(a,b,c), d, a, b, c, state->cur_chunk[2], 0xfcefa3f8L, 9);
    MD5_STEP(MD5_FUNC_G(d,a,b), c, d, a, b, state->cur_chunk[7], 0x676f02d9L, 14);
    MD5_STEP(MD5_FUNC_G(c,d,a), b, c, d, a, state->cur_chunk[12], 0x8d2a4c8aL, 20);

    MD5_STEP(MD5_FUNC_H(b,c,d), a, b, c, d, state->cur_chunk[5], 0xfffa3942L, 4);
    MD5_STEP(MD5_FUNC_H(a,b,c), d, a, b, c, state->cur_chunk[8], 0x8771f681L, 11);
    MD5_STEP(MD5_FUNC_H(d,a,b), c, d, a, b, state->cur_chunk[11], 0x6d9d6122L, 16);
    MD5_STEP(MD5_FUNC_H(c,d,a), b, c, d, a, state->cur_chunk[14], 0xfde5380cL, 23);
    MD5_STEP(MD5_FUNC_H(b,c,d), a, b, c, d, state->cur_chunk[1], 0xa4beea44L, 4);
    MD5_STEP(MD5_FUNC_H(a,b,c), d, a, b, c, state->cur_chunk[4], 0x4bdecfa9L, 11);
    MD5_STEP(MD5_FUNC_H(d,a,b), c, d, a, b, state->cur_chunk[7], 0xf6bb4b60L, 16);
    MD5_STEP(MD5_FUNC_H(c,d,a), b, c, d, a, state->cur_chunk[10], 0xbebfbc70L, 23);
    MD5_STEP(MD5_FUNC_H(b,c,d), a, b, c, d, state->cur_chunk[13], 0x289b7ec6L, 4);
    MD5_STEP(MD5_FUNC_H(a,b,c), d, a, b, c, state->cur_chunk[0], 0xeaa127faL, 11);
    MD5_STEP(MD5_FUNC_H(d,a,b), c, d, a, b, state->cur_chunk[3], 0xd4ef3085L, 16);
    MD5_STEP(MD5_FUNC_H(c,d,a), b, c, d, a, state->cur_chunk[6], 0x04881d05L, 23);
    MD5_STEP(MD5_FUNC_H(b,c,d), a, b, c, d, state->cur_chunk[9], 0xd9d4d039L, 4);
    MD5_STEP(MD5_FUNC_H(a,b,c), d, a, b, c, state->cur_chunk[12], 0xe6db99e5L, 11);
    MD5_STEP(MD5_FUNC_H(d,a,b), c, d, a, b, state->cur_chunk[15], 0x1fa27cf8L, 16);
    MD5_STEP(MD5_FUNC_H(c,d,a), b, c, d, a, state->cur_chunk[2], 0xc4ac5665L, 23);

    MD5_STEP(MD5_FUNC_I(b,c,d), a, b, c, d, state->cur_chunk[0], 0xf4292244L, 6);
    MD5_STEP(MD5_FUNC_I(a,b,c), d, a, b, c, state->cur_chunk[7], 0x432aff97L, 10);
    MD5_STEP(MD5_FUNC_I(d,a,b), c, d, a, b, state->cur_chunk[14], 0xab9423a7L, 15);
    MD5_STEP(MD5_FUNC_I(c,d,a), b, c, d, a, state->cur_chunk[5], 0xfc93a039L, 21);
    MD5_STEP(MD5_FUNC_I(b,c,d), a, b, c, d, state->cur_chunk[12], 0x655b59c3L, 6);
    MD5_STEP(MD5_FUNC_I(a,b,c), d, a, b, c, state->cur_chunk[3], 0x8f0ccc92L, 10);
    MD5_STEP(MD5_FUNC_I(d,a,b), c, d, a, b, state->cur_chunk[10], 0xffeff47dL, 15);
    MD5_STEP(MD5_FUNC_I(c,d,a), b, c, d, a, state->cur_chunk[1], 0x85845dd1L, 21);
    MD5_STEP(MD5_FUNC_I(b,c,d), a, b, c, d, state->cur_chunk[8], 0x6fa87e4fL, 6);
    MD5_STEP(MD5_FUNC_I(a,b,c), d, a, b, c, state->cur_chunk[15], 0xfe2ce6e0L, 10);
    MD5_STEP(MD5_FUNC_I(d,a,b), c, d, a, b, state->cur_chunk[6], 0xa3014314L, 15);
    MD5_STEP(MD5_FUNC_I(c,d,a), b, c, d, a, state->cur_chunk[13], 0x4e0811a1L, 21);
    MD5_STEP(MD5_FUNC_I(b,c,d), a, b, c, d, state->cur_chunk[4], 0xf7537e82L, 6);
    MD5_STEP(MD5_FUNC_I(a,b,c), d, a, b, c, state->cur_chunk[11], 0xbd3af235L, 10);
    MD5_STEP(MD5_FUNC_I(d,a,b), c, d, a, b, state->cur_chunk[2], 0x2ad7d2bbL, 15);
    MD5_STEP(MD5_FUNC_I(c,d,a), b, c, d, a, state->cur_chunk[9], 0xeb86d391L, 21);

	/* Add the chunk variables back into the current hash */
	state->hash[0] += a;
	state->hash[1] += b;
	state->hash[2] += c;
	state->hash[3] += d;
}
