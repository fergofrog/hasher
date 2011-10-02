/*
 * sha1.c
 *
 *  Created on: Oct 2, 2011
 *      Author: FergoFrog
 */

#import <stdio.h>

#import "../global.h"
#import "sha1.h"

#define SHA1_FUNC_F(X, Y, Z) (((X) & (Y)) | (~(X) & (Z)))
#define SHA1_FUNC_G(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define SHA1_FUNC_H(X, Y, Z) (((X) & (Y)) | ((X) & (Z)) | ((Y) & (Z)))
#define SHA1_FUNC_I(X, Y, Z) ((X) ^ (Y) ^ (Z))

extern unsigned int hash[5];
extern unsigned long long hash_length;
extern char in_hash;
extern unsigned char cur_chunk[80][4], cur_chunk_pos;

void sha1_add_chunk();

char sha1_init()
{
	if (!in_hash) {
		hash[0] = 0x67452301;
		hash[1] = 0xEFCDAB89;
		hash[2] = 0x98BADCFE;
		hash[3] = 0x10325476;
		hash[4] = 0xC3D2E1F0;

		hash_length = 0;
		cur_chunk_pos = 0;

		in_hash = 1;

		return 1;
	} else {
		return 0;
	}
}

char sha1_add_string(char *str)
{
	if (in_hash) {
		int i;
		for (i = 0; str[i] != '\0'; i++) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = str[i];
			cur_chunk_pos++;
			hash_length += 8;

			if (cur_chunk_pos >= 64) {
				sha1_add_chunk();
				cur_chunk_pos = 0;
			}
		}

		return 1;
	} else {
		return 0;
	}
}

char sha1_add_file(FILE *fp)
{
	if (in_hash) {
		int c = fgetc(fp);
		while (c != EOF) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = (unsigned char) c;
			cur_chunk_pos++;
			hash_length += 8;

			if (cur_chunk_pos >= 64) {
				sha1_add_chunk();
				cur_chunk_pos = 0;
			}

			c = fgetc(fp);
		};

		return 1;
	} else {
		return 0;
	}
}

char sha1_get_hash(unsigned int hash_out[])
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

			sha1_add_chunk();
			cur_chunk_pos = 0;
		}

		/* Append 0's up until the 56th byte of the chunk (leaving 8 bytes for the length) */
		while (cur_chunk_pos < 56) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = 0x00;
			cur_chunk_pos++;
		}

		/* Append length */
		unsigned char length_b[8];
		be_ll_to_b(hash_length, length_b);
		while (cur_chunk_pos < 64) {
			cur_chunk[cur_chunk_pos / 4][cur_chunk_pos % 4] = length_b[cur_chunk_pos - 56];
			cur_chunk_pos++;
		}

		/* Process the chunk */
		sha1_add_chunk();

		/* Copy the hash over */
		hash_out[0] = hash[0];
		hash_out[1] = hash[1];
		hash_out[2] = hash[2];
		hash_out[3] = hash[3];
		hash_out[4] = hash[4];

		in_hash = 0;

		return 1;
	} else {
		return 0;
	}
}

void sha1_add_chunk()
{
	unsigned int func_out, constant;
	int i;

	unsigned int a = hash[0], b = hash[1], c = hash[2], d = hash[3], e = hash[4];

	for (i = 16; i < 80; i++) {
		cur_chunk[i][0] = cur_chunk[i - 3][0] ^ cur_chunk[i - 8][0] ^ cur_chunk[i - 14][0] ^ cur_chunk[i - 16][0];
		cur_chunk[i][1] = cur_chunk[i - 3][1] ^ cur_chunk[i - 8][1] ^ cur_chunk[i - 14][1] ^ cur_chunk[i - 16][1];
		cur_chunk[i][2] = cur_chunk[i - 3][2] ^ cur_chunk[i - 8][2] ^ cur_chunk[i - 14][2] ^ cur_chunk[i - 16][2];
		cur_chunk[i][3] = cur_chunk[i - 3][3] ^ cur_chunk[i - 8][3] ^ cur_chunk[i - 14][3] ^ cur_chunk[i - 16][3];
		be_w_l_rot(cur_chunk[i], 1);
	}

	for (i = 0; i < 80; i++) {
		if (i >= 0 && i < 20) {
			func_out = SHA1_FUNC_F(b, c, d);
			constant = 0x5A827999;
		} else if (i >= 20 && i < 40) {
			func_out = SHA1_FUNC_G(b, c, d);
			constant = 0x6ED9EBA1;
		} else if (i >= 40 && i < 60) {
			func_out = SHA1_FUNC_H(b, c, d);
			constant = 0x8F1BBCDC;
		} else {
			func_out = SHA1_FUNC_I(b, c, d);
			constant = 0xCA62C1D6;
		}

		unsigned int temp = be_l_rot(a, 5) + func_out + e + constant + be_b_to_w(cur_chunk[i]);
		e = d;
		d = c;
		c = be_l_rot(b, 30);
		b = a;
		a = temp;
	}

	hash[0] += a;
	hash[1] += b;
	hash[2] += c;
	hash[3] += d;
	hash[4] += e;
}
