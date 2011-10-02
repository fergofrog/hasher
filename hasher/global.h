/*
 * global.h
 *
 *  Created on: Oct 2, 2011
 *      Author: FergoFrog
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

unsigned int hash[5];
unsigned long long hash_length;
char in_hash;
unsigned char cur_chunk[80][4], cur_chunk_pos;

unsigned int be_l_rot(unsigned int, unsigned int);
void be_w_l_rot(unsigned char [], unsigned int);
unsigned int be_b_to_w(unsigned char []);
void be_ll_to_b(unsigned long long ll, unsigned char b[]);

unsigned int le_l_rot(unsigned int, unsigned int);
unsigned int le_b_to_w(unsigned char []);
void le_ll_to_b(unsigned long long, unsigned char []);

#endif /* GLOBAL_H_ */
