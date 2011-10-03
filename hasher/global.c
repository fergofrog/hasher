/**
 * @file global.c
 * A collection of various type and number manipulation algorithms
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
#import "global.h"

/**
 * Left rotate for unsigned integers
 *
 * @param value Value to be rotated
 * @param shift Amount to be rotated by
 * @return The rotated value
 */
unsigned int i_l_rot(unsigned int value, unsigned int shift)
{
	return (value << shift) | (value >> (32 - shift));
}

/**
 * Right rotate for unsigned integers
 *
 * @param value Value to be rotated
 * @param shift Amount to be rotated by
 * @return The rotated value
 */
unsigned int i_r_rot(unsigned int value, unsigned int shift)
{
	return (value >> shift) | (value << (32 - shift));
}

/**
 * Right rotate for unsigned double longs
 *
 * @param value Value to be rotated
 * @param shift Amount to be rotated by
 * @return The rotated value
 */
unsigned long long ll_r_rot(unsigned long long value, unsigned long long shift)
{
	return (value >> shift) | (value << (64ULL - shift));
}

/**
 * Left rotate for an array of 4 bytes (an integer)

 *
 * @param w The integer, an array of 4 big endian bytes
 *  - the result is also stored here
 * @param shift Amount to be rotated by
 */
void be_w_l_rot(unsigned char w[], unsigned int shift)
{
	unsigned char temp;
	temp = (w[0] << shift) | (w[1] >> (8 - shift));
	w[1] = (w[1] << shift) | (w[2] >> (8 - shift));
	w[2] = (w[2] << shift) | (w[3] >> (8 - shift));
	w[3] = (w[3] << shift) | (w[0] >> (8 - shift));
	w[0] = temp;
}

/**
 * Convert a big endian array of 4 bytes to 32 bit word
 *
 * @param b Big endian array of 4 bytes
 * @return Unsigned 32 bit word of the bytes
 */
unsigned int be_i_b_to_w(unsigned char b[])
{
	return b[3] | (b[2] << 8) | (b[1] << 16) | (b[0] << 24);
}

/**
 * Convert a big endian array of 8 bytes to 64 bit word
 *
 * @param b Big endian array of 8 bytes
 * @return Unsigned 64 bit word of the bytes
 */
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

/**
 * Convert a 64 bit word to a big endian array of 8 bytes
 *
 * @param ll Unsigned 64 bit word to be converted
 * @param b Array of 8 bytes to store the big endian representation of the word
 */
void be_ll_to_b(unsigned long long ll, unsigned char b[])
{
	b[7] = (ll & 0x00000000000000FF) >>  0;
	b[6] = (ll & 0x000000000000FF00) >>  8;
	b[5] = (ll & 0x0000000000FF0000) >> 16;
	b[4] = (ll & 0x00000000FF000000) >> 24;
	b[3] = (ll & 0x000000FF00000000) >> 32;
	b[2] = (ll & 0x0000FF0000000000) >> 40;
	b[1] = (ll & 0x00FF000000000000) >> 48;
	b[0] = (ll & 0xFF00000000000000) >> 56;
}

/**
 * Convert a 128 bit word to a big endian array of 16 bytes
 *
 * @param ms_ll Most significant unsigned 64 bit word to be converted
 * @param ls_ll Least significant unsigned 64 bit word to be converted
 * @param b Array of 16 bytes to store the big endian representation
 */
void be_llll_to_b(unsigned long long ms_ll, unsigned long long ls_ll,
		unsigned char b[])
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

/**
 * Convert a little endian array of 4 bytes to 32 bit word
 *
 * @param b Little endian array of 4 bytes
 * @return Unsigned 32 bit word of the bytes
 */
unsigned int le_b_to_w(unsigned char b[])
{
	return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
}

/**
 * Convert a 64 bit word to a little endian array of 8 bytes
 *
 * @param ll Unsigned 64 bit word to be converted
 * @param b Array of 8 bytes to store the little endian representation
 */
void le_ll_to_b(unsigned long long ll, unsigned char b[])
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
