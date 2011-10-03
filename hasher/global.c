/*
 *  A collection of common hashing algorithms for hashing strings and files
 *  global.c - a collection of various type and number manipulation algorithms
 *  Copyright (C) 2011 FergoFrog
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#import "global.h"

unsigned int i_l_rot(unsigned int value, unsigned int shift)
{
	return (value << shift) | (value >> (32 - shift));
}

unsigned int i_r_rot(unsigned int value, unsigned int shift)
{
	return (value >> shift) | (value << (32 - shift));
}

unsigned long long ll_r_rot(unsigned long long value, unsigned long long shift)
{
	return (value >> shift) | (value << (64ULL - shift));
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

unsigned int be_i_b_to_w(unsigned char b[])
{
	/* Little Endian */
	/*
	return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
	 */
	/* Big Endian */
	return b[3] | (b[2] << 8) | (b[1] << 16) | (b[0] << 24);
}

unsigned long long be_ll_b_to_w(unsigned char b[])
{
	unsigned long long temp = 0ULL;
	temp |= (unsigned long long) b[7];
	temp |= ((unsigned long long) b[6] << 8);
	temp |= ((unsigned long long) b[5] << 16);
	temp |= ((unsigned long long) b[4] << 24);
	temp |= ((unsigned long long) b[3] << 32);
	temp |= ((unsigned long long) b[2] << 40);
	temp |= ((unsigned long long) b[1] << 48);
	temp |= ((unsigned long long) b[0] << 56);
	return temp;
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

void be_llll_to_b(unsigned long long ms_ll, unsigned long long ls_ll, unsigned char b[])
{
	b[15] = (ls_ll & 0x00000000000000FF) >>  0;
	b[14] = (ls_ll & 0x000000000000FF00) >>  8;
	b[13] = (ls_ll & 0x0000000000FF0000) >> 16;
	b[12] = (ls_ll & 0x00000000FF000000) >> 24;
	b[11] = (ls_ll & 0x000000FF00000000) >> 32;
	b[10] = (ls_ll & 0x0000FF0000000000) >> 40;
	b[9] = (ls_ll & 0x00FF000000000000) >> 48;
	b[8] = (ls_ll & 0xFF00000000000000) >> 56;
	b[7] = (ms_ll & 0x00000000000000FF) >>  0;
	b[6] = (ms_ll & 0x000000000000FF00) >>  8;
	b[5] = (ms_ll & 0x0000000000FF0000) >> 16;
	b[4] = (ms_ll & 0x00000000FF000000) >> 24;
	b[3] = (ms_ll & 0x000000FF00000000) >> 32;
	b[2] = (ms_ll & 0x0000FF0000000000) >> 40;
	b[1] = (ms_ll & 0x00FF000000000000) >> 48;
	b[0] = (ms_ll & 0xFF00000000000000) >> 56;
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
