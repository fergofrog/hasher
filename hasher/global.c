/*
 * global.c
 *
 *  Created on: Oct 2, 2011
 *      Author: FergoFrog
 */

#import "global.h"

unsigned int be_l_rot(unsigned int value, unsigned int shift)
{
	return (value << shift) | (value >> (32 - shift));
}

void be_w_l_rot(unsigned char w[], unsigned int shift)
{
	unsigned char temp;
	temp = (w[0] << shift) | (w[1] >> (8 - shift));
	w[1] = (w[1] << shift) | (w[2] >> (8 - shift));
	w[2] = (w[2] << shift) | (w[3] >> (8 - shift));
	w[3] = (w[3] << shift) | (w[0] >> (8 - shift));
	w[0] = temp;
}

unsigned int be_b_to_w(unsigned char b[])
{
	/* Little Endian */
	/*
	return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
	 */
	/* Big Endian */
	return b[3] | (b[2] << 8) | (b[1] << 16) | (b[0] << 24);
}

void be_ll_to_b(unsigned long long ll, unsigned char b[])
{
	/* Little Endian */
	/*
	b[0] = (ll & 0x00000000000000FF) >>  0;
	b[1] = (ll & 0x000000000000FF00) >>  8;
	b[2] = (ll & 0x0000000000FF0000) >> 16;
	b[3] = (ll & 0x00000000FF000000) >> 24;
	b[4] = (ll & 0x000000FF00000000) >> 32;
	b[5] = (ll & 0x0000FF0000000000) >> 40;
	b[6] = (ll & 0x00FF000000000000) >> 48;
	b[7] = (ll & 0xFF00000000000000) >> 56;
	 */
	/* Big Endian */
	b[7] = (ll & 0x00000000000000FF) >>  0;
	b[6] = (ll & 0x000000000000FF00) >>  8;
	b[5] = (ll & 0x0000000000FF0000) >> 16;
	b[4] = (ll & 0x00000000FF000000) >> 24;
	b[3] = (ll & 0x000000FF00000000) >> 32;
	b[2] = (ll & 0x0000FF0000000000) >> 40;
	b[1] = (ll & 0x00FF000000000000) >> 48;
	b[0] = (ll & 0xFF00000000000000) >> 56;
}


unsigned int le_l_rot(unsigned int value, unsigned int shift)
{
	return (value << shift) | (value >> (32 - shift));
}

unsigned int le_b_to_w(unsigned char b[])
{
	/* Little Endian */
	return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
	/* Big Endian */
	/*
	return b[3] | (b[2] << 8) | (b[1] << 16) | (b[0] << 24);
	 */
}

void le_ll_to_b(unsigned long long ll, unsigned char b[])
{
	/* Little Endian */
	b[0] = (ll & 0x00000000000000FF) >>  0;
	b[1] = (ll & 0x000000000000FF00) >>  8;
	b[2] = (ll & 0x0000000000FF0000) >> 16;
	b[3] = (ll & 0x00000000FF000000) >> 24;
	b[4] = (ll & 0x000000FF00000000) >> 32;
	b[5] = (ll & 0x0000FF0000000000) >> 40;
	b[6] = (ll & 0x00FF000000000000) >> 48;
	b[7] = (ll & 0xFF00000000000000) >> 56;
	/* Big Endian */
	/*
	b[7] = (ll & 0x00000000000000FF) >>  0;
	b[6] = (ll & 0x000000000000FF00) >>  8;
	b[5] = (ll & 0x0000000000FF0000) >> 16;
	b[4] = (ll & 0x00000000FF000000) >> 24;
	b[3] = (ll & 0x000000FF00000000) >> 32;
	b[2] = (ll & 0x0000FF0000000000) >> 40;
	b[1] = (ll & 0x00FF000000000000) >> 48;
	b[0] = (ll & 0xFF00000000000000) >> 56;
	 */
}
