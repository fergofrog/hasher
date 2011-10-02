/*
 * sha1.h
 *
 *  Created on: Oct 2, 2011
 *      Author: FergoFrog
 */

#ifndef SHA1_H_
#define SHA1_H_

char sha1_init();
char sha1_add_string(char *);
char sha1_add_file(FILE *);
char sha1_get_hash(unsigned int[]);

#endif /* SHA1_H_ */
