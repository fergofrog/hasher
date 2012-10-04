/**
 * @file md5.h
 * Header for md5.c
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
#ifndef MD5_H_
#define MD5_H_

#define MD5_CHUNK_WORD_SIZE 4
#define MD5_CHUNK_NUM_WORDS 16
#define MD5_CHUNK_LEN (MD5_CHUNK_WORD_SIZE * MD5_CHUNK_NUM_WORDS)

struct md5_state {
	unsigned int hash[4];
	unsigned int length;
	unsigned int cur_chunk[MD5_CHUNK_NUM_WORDS];
	unsigned int cur_chunk_pos;
};

char md5_init(struct md5_state *);
char md5_add_string(struct md5_state *, char *);
char md5_add_file(struct md5_state *, FILE *);
char md5_get_hash(struct md5_state *, unsigned int []);
char md5_get_hash_str(struct md5_state *, char []);

#endif /* MD5_H_ */
