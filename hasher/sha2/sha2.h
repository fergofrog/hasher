/*
 *  A collection of common hashing algorithms for hashing strings and files
 *  sha2.h - header for sha2.c
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

#ifndef SHA2_H_
#define SHA2_H_

enum sha2_t {
	SHA256,
	SHA224,
	SHA512,
	SHA384
};

char sha2_init();
char sha2_add_string(char *);
char sha2_add_file(FILE *);
char sha2_get_hash(unsigned long long[]);

#endif /* SHA2_H_ */
