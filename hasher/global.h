/*
 *  A collection of common hashing algorithms for hashing strings and files
 *  global.h - header for global.c
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

#ifndef GLOBAL_H_
#define GLOBAL_H_

unsigned int i_hash[8];
unsigned long long ll_hash[8];
unsigned long long hash_length, hash_length2;
char in_hash;
unsigned char cur_chunk[80][8], cur_chunk_pos;

unsigned int i_l_rot(unsigned int, unsigned int);
unsigned int i_r_rot(unsigned int, unsigned int);
unsigned long long ll_r_rot(unsigned long long, unsigned long long);

void be_w_l_rot(unsigned char [], unsigned int);
unsigned int be_i_b_to_w(unsigned char []);
unsigned long long be_ll_b_to_w(unsigned char []);
void be_ll_to_b(unsigned long long, unsigned char []);
void be_llll_to_b(unsigned long long, unsigned long long, unsigned char []);

unsigned int le_b_to_w(unsigned char []);
void le_ll_to_b(unsigned long long, unsigned char []);

#endif /* GLOBAL_H_ */
