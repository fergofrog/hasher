/**
 * @file global.h
 * Header for global.c
 * @author	FergoFrog <fergofrog@fergofrog.com>
 * @version 0.4
 *
 * @section LICENSE
 * Copyright (C) 2011-2012 FergoFrog
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
#ifndef GLOBAL_H_
#define GLOBAL_H_

#define check_malloc(a)                                         \
    do {                                                        \
        if ((a) == NULL) {                                      \
            perror("Error: could not allocate enough memory");  \
            exit(1);                                            \
        }                                                       \
    } while(0)

#define min(A, B) ((A) < (B) ? (A) : (B))

/** Do not hash flag */
#define ARG_NO_HASH     1 << 0
#define ARG_NO_THREAD   1 << 1

/** Hash types known */
enum hash_t {
    H_MD5    = 0,
    H_SHA1   = 1,
    H_SHA256 = 2,
    H_SHA224 = 3,
    H_SHA512 = 4,
    H_SHA384 = 5
};
static char *hash_names[6] = {"md5", "sha1", "sha256", "sha224", "sha512", "sha384"};

enum target_t {
    T_FILE,
    T_STRING
};

struct args_t {
    enum hash_t hash;
    enum target_t target_type;
    unsigned int flags;
    unsigned int no_threads;
    unsigned int no_targets;
    struct file_list_t *target;
};

extern enum hash_t hash_algorithm;
extern unsigned int i_hash[8];
extern unsigned long long ll_hash[8];
extern unsigned long long hash_length;
extern unsigned long long hash_length2;
extern char in_hash;
extern unsigned char cur_chunk[80][8];
extern unsigned int cur_chunk_pos;
extern unsigned int verbose_level;

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

/* NEW */
void le_ll_to_i(unsigned long long, unsigned int []);

#endif /* GLOBAL_H_ */
