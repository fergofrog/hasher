/*
 * md5.h
 *
 *  Created on: Oct 1, 2011
 *      Author: FergoFrog
 */

#ifndef MD5_H_
#define MD5_H_

char md5_init();
char md5_add_string(char *);
char md5_add_file(FILE *);
char md5_get_hash(unsigned int []);

#endif /* MD5_H_ */
