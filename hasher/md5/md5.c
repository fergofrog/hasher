/*
 * md5.c
 *
 *  Created on: Oct 1, 2011
 *      Author: FergoFrog
 */

#include <stdio.h>

#include "md5.h"

#define FUNC_F(X, Y, Z) (((X) & (Y)) | (~(X) & (Z)))
#define FUNC_G(X, Y, Z) (((X) & (Z)) | ((Y) & ~(Z)))
#define FUNC_H(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define FUNC_I(X, Y, Z) ((Y) ^ ((X) | ~(Z)))

//const int shiftAmounts[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
//                            5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
//                            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
//                            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
const unsigned int shift_amounts[4][4] = {{ 7, 12, 17, 22},
		                      { 5,  9, 14, 20},
		                      { 4, 11, 16, 23},
		                      { 6, 10, 15, 21}};
const unsigned int operation_constants[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
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

unsigned int hash[4];
unsigned long long hash_length;
char in_hash = 0;
unsigned char cur_chunk[16][4], cur_chunk_pos;

void add_chunk();
unsigned int b_to_w(unsigned char []);
void ll_to_b(unsigned long long, unsigned char []);

char md5_init()
{
	if (!in_hash) {
		hash[0] = 0x67452301;
		hash[1] = 0xEFCDAB89;
		hash[2] = 0x98BADCFE;
		hash[3] = 0x10325476;

		cur_chunk_pos = 0;

		in_hash = 1;

		return 1;
	} else {
		return 0;
	}
}

char md5_add_file(FILE *fp)
{
	if (in_hash) {
		int c = fgetc(fp);
		while (c != EOF) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = (unsigned char) c;
			cur_chunk_pos++;
			hash_length += 8;

			if (cur_chunk_pos >= 64) {
				add_chunk();
				cur_chunk_pos = 0;
			}

			c = fgetc(fp);
		};

		return 1;
	} else {
		return 0;
	}
}

char md5_add_string(char *str, unsigned int len)
{
	if (in_hash) {
		int i;
		for (i = 0; i < len; i++) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = str[i];
			cur_chunk_pos++;

			if (cur_chunk_pos >= 64) {
				add_chunk();
				cur_chunk_pos = 0;
			}
		}

		hash_length += len * 8;

		return 1;
	} else {
		return 0;
	}
}

char md5_get_hash(unsigned int hash_out[])
{
	if (in_hash) {
		/* Begin appending */
		cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = 0x80;
		cur_chunk_pos++;

		/* If the chunk pos is over 448 bits (56 bytes) - complete the current chunk */
		if (cur_chunk_pos > 56) {
			while (cur_chunk_pos < 64) {
				cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = 0x00;
				cur_chunk_pos++;
			}

			add_chunk();
			cur_chunk_pos = 0;
		}

		/* Append 0's up until the 56th byte of the chunk (leaving 8 bytes for the length) */
		while (cur_chunk_pos < 56) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = 0x00;
			cur_chunk_pos++;
		}

		/* Append length */
		unsigned char length_b[8];
		ll_to_b(hash_length, length_b);
		while (cur_chunk_pos < 64) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = length_b[cur_chunk_pos - 56];
			cur_chunk_pos++;
		}

		/* Process the chunk */
		add_chunk();

		/* Copy the hash over */
		hash_out[0] = hash[0];
		hash_out[1] = hash[1];
		hash_out[2] = hash[2];
		hash_out[3] = hash[3];

		return 1;
	} else {
		return 0;
	}
}

unsigned int l_rot(unsigned int value, unsigned int shift)
{
	return (value << shift) | (value >> (32 - shift));
}

//TODO: Globalise add_chunk (a-d goes out)
void add_chunk()
{
	unsigned int func_out, word_idx;
	int i;

	unsigned int a = hash[0], b = hash[1], c = hash[2], d = hash[3];

	for (i = 0; i < 64; i++) {
		if (i >= 0 && i < 16) {
			func_out = FUNC_F(b, c, d);
			word_idx = i;
		} else if (i >= 16 && i < 32) {
			func_out = FUNC_G(b, c, d);
			word_idx = (5 * i + 1) % 16;
		} else if (i >= 32 && i < 48) {
			func_out = FUNC_H(b, c, d);
			word_idx = (3 * i + 5) % 16;
		} else {
			func_out = FUNC_I(b, c, d);
			word_idx = (7 * i) % 16;
		}

		unsigned int tmp = d;
		d = c;
		c = b;
		b = b + l_rot(a + func_out + operation_constants[i] + b_to_w(cur_chunk[word_idx]), shift_amounts[i / 16][i % 4]);
		a = tmp;
	}

	hash[0] += a;
	hash[1] += b;
	hash[2] += c;
	hash[3] += d;
}

unsigned int b_to_w(unsigned char b[])
{
	return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
	//return b[3] | (b[2] << 8) | (b[1] << 16) | (b[0] << 24);
}

void ll_to_b(unsigned long long ll, unsigned char b[])
{
	b[0] = (ll & 0x00000000000000FF) >>  0;
	b[1] = (ll & 0x000000000000FF00) >>  8;
	b[2] = (ll & 0x0000000000FF0000) >> 16;
	b[3] = (ll & 0x00000000FF000000) >> 24;
	b[4] = (ll & 0x000000FF00000000) >> 32;
	b[5] = (ll & 0x0000FF0000000000) >> 40;
	b[6] = (ll & 0x00FF000000000000) >> 48;
	b[7] = (ll & 0xFF00000000000000) >> 56;
}
