/*
 *  A collection of common hashing algorithms for hashing strings and files
 *  main.c - the front-end to the libraries
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

#include <stdio.h>
#include <string.h>

#include "global.h"
#include "md5/md5.h"
#include "sha1/sha1.h"
#include "sha2/sha2.h"

#define VERSION "0.2"

#define TRUE 1
#define FALSE 0

enum hash_t {
	H_MD5,
	H_SHA1,
	H_SHA256,
	H_SHA224,
	H_SHA512,
	H_SHA384
};

void print_version()
{
	printf("hasher, version " VERSION "\n\n");
}

void print_help(char *program)
{
	printf("usage: %s [-h] [--md5] [--sha1] [-s string] [-f file]\n\n", program);
	printf("\t    --md5\tuse md5\n");
	printf("\t    --sha1\tuse sha1\n");
	printf("\t-s, --string\tstring input\n");
	printf("\t-f, --file\tfile input\n");
	printf("\t-h, --help\tthis message\n");
}

int main(int argc, char *argv[])
{
	char string_input = FALSE, file_input = FALSE;
	enum hash_t hash;

	char *string_to_process;
	char *file_to_process;

	in_hash = 0;

	int i = 1;
	while (i < argc) {
		switch (argv[i][1]) {
		case '-':
			if (strcmp(argv[i] + 2, "md5") == 0) {
				hash = H_MD5;
			} else if (strcmp(argv[i] + 2, "sha1") == 0) {
				hash = H_SHA1;
			} else if (strcmp(argv[i] + 2, "sha256") == 0) {
				hash = H_SHA256;
			} else if (strcmp(argv[i] + 2, "sha224") == 0) {
				hash = H_SHA224;
			} else if (strcmp(argv[i] + 2, "sha512") == 0) {
				hash = H_SHA512;
			} else if (strcmp(argv[i] + 2, "sha384") == 0) {
				hash = H_SHA384;
			} else if (strcmp(argv[i] + 2, "string") == 0) {
				string_input = TRUE;
				string_to_process = argv[++i];
			} else if (strcmp(argv[i] + 2, "file") == 0) {
				file_input = TRUE;
				file_to_process = argv[++i];
			} else if (strcmp(argv[i] + 2, "help") == 0) {
				string_input = file_input = 0;
				print_help(argv[0]);
				return 0;
			} else {
				string_input = file_input = 0;
				printf("Unknown command %s\n\n", argv[i]);
				print_help(argv[0]);
				return 1;
			}
			break;
		case 's':
			/* String Input */
			string_input = TRUE;
			string_to_process = argv[++i];
			break;
		case 'f':
			/* File Input */
			file_input = TRUE;
			file_to_process = argv[++i];
			break;
		case 'h':
			/* Help message */
			string_input = file_input = 0;
			print_help(argv[0]);
			return 0;
		default:
			string_input = file_input = 0;
			printf("Unknown command %s\n\n", argv[i]);
			print_help(argv[0]);
			return 1;
		}
		i++;
	}

	unsigned int i_hash_out[8];
	unsigned long long ll_hash_out[8];
	char hash_out_str[129];
	FILE *fp;

	switch (hash) {
	case H_MD5:
		md5_init();
		if (string_input && string_to_process != NULL) {
			md5_add_string(string_to_process);
		} else if (file_input) {
			fp = fopen(file_to_process, "r");
			md5_add_file(fp);
		}
		md5_get_hash(i_hash_out);

		sprintf(hash_out_str, "%08x%08x%08x%08x", i_hash_out[0], i_hash_out[1], i_hash_out[2], i_hash_out[3]);

		for (i = 0; i < 4; i++) {
			int j;
			for (j = 0; j < 2; j++) {
				char temp[2];

				temp[0] = hash_out_str[(i * 8) + (j * 2)];
                temp[1] = hash_out_str[(i * 8) + (j * 2) + 1];

                hash_out_str[(i * 8) + (j * 2)] = hash_out_str[(i * 8) + (6 - (j * 2))];
                hash_out_str[(i * 8) + (j * 2) + 1] = hash_out_str[(i * 8) + (7 - (j * 2))];

                hash_out_str[(i * 8) + (6 - (j * 2))] = temp[0];
                hash_out_str[(i * 8) + (7 - (j * 2))] = temp[1];
			}
		}

		printf("%s\n", hash_out_str);
		break;
	case H_SHA1:
		sha1_init();
		if (string_input && string_to_process != NULL) {
			sha1_add_string(string_to_process);
		} else if (file_input) {
			fp = fopen(file_to_process, "r");
			sha1_add_file(fp);
		}
		sha1_get_hash(i_hash_out);

		sprintf(hash_out_str, "%08x%08x%08x%08x%08x", i_hash_out[0], i_hash_out[1], i_hash_out[2], i_hash_out[3], i_hash_out[4]);

		/*
		for (i = 0; i < 5; i++) {
			int j;
			for (j = 0; j < 2; j++) {
				char temp[2];

				temp[0] = hash_out_str[(i * 8) + (j * 2)];
                temp[1] = hash_out_str[(i * 8) + (j * 2) + 1];

                hash_out_str[(i * 8) + (j * 2)] = hash_out_str[(i * 8) + (6 - (j * 2))];
                hash_out_str[(i * 8) + (j * 2) + 1] = hash_out_str[(i * 8) + (7 - (j * 2))];

                hash_out_str[(i * 8) + (6 - (j * 2))] = temp[0];
                hash_out_str[(i * 8) + (7 - (j * 2))] = temp[1];
			}
		}
		 */

		printf("%s\n", hash_out_str);
		break;
	case H_SHA256:
		sha2_init(SHA256);
		if (string_input && string_to_process != NULL) {
			sha2_add_string(string_to_process);
		} else if (file_input) {
			fp = fopen(file_to_process, "r");
			sha2_add_file(fp);
		}
		sha2_get_hash(ll_hash_out);

		sprintf(hash_out_str, "%08llx%08llx%08llx%08llx%08llx%08llx%08llx%08llx", ll_hash_out[0], ll_hash_out[1], ll_hash_out[2], ll_hash_out[3], ll_hash_out[4], ll_hash_out[5], ll_hash_out[6], ll_hash_out[7]);

		/*
		for (i = 0; i < 5; i++) {
			int j;
			for (j = 0; j < 2; j++) {
				char temp[2];

				temp[0] = hash_out_str[(i * 8) + (j * 2)];
                temp[1] = hash_out_str[(i * 8) + (j * 2) + 1];

                hash_out_str[(i * 8) + (j * 2)] = hash_out_str[(i * 8) + (6 - (j * 2))];
                hash_out_str[(i * 8) + (j * 2) + 1] = hash_out_str[(i * 8) + (7 - (j * 2))];

                hash_out_str[(i * 8) + (6 - (j * 2))] = temp[0];
                hash_out_str[(i * 8) + (7 - (j * 2))] = temp[1];
			}
		}
		 */

		printf("%s\n", hash_out_str);
		break;
	case H_SHA224:
		sha2_init(SHA224);
		if (string_input && string_to_process != NULL) {
			sha2_add_string(string_to_process);
		} else if (file_input) {
			fp = fopen(file_to_process, "r");
			sha2_add_file(fp);
		}
		sha2_get_hash(ll_hash_out);

		sprintf(hash_out_str, "%08llx%08llx%08llx%08llx%08llx%08llx%08llx", ll_hash_out[0], ll_hash_out[1], ll_hash_out[2], ll_hash_out[3], ll_hash_out[4], ll_hash_out[5], ll_hash_out[6]);

		/*
		for (i = 0; i < 5; i++) {
			int j;
			for (j = 0; j < 2; j++) {
				char temp[2];

				temp[0] = hash_out_str[(i * 8) + (j * 2)];
                temp[1] = hash_out_str[(i * 8) + (j * 2) + 1];

                hash_out_str[(i * 8) + (j * 2)] = hash_out_str[(i * 8) + (6 - (j * 2))];
                hash_out_str[(i * 8) + (j * 2) + 1] = hash_out_str[(i * 8) + (7 - (j * 2))];

                hash_out_str[(i * 8) + (6 - (j * 2))] = temp[0];
                hash_out_str[(i * 8) + (7 - (j * 2))] = temp[1];
			}
		}
		 */

		printf("%s\n", hash_out_str);
		break;
	case H_SHA512:
		sha2_init(SHA512);
		if (string_input && string_to_process != NULL) {
			sha2_add_string(string_to_process);
		} else if (file_input) {
			fp = fopen(file_to_process, "r");
			sha2_add_file(fp);
		}
		sha2_get_hash(ll_hash_out);

		sprintf(hash_out_str, "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx", ll_hash_out[0], ll_hash_out[1], ll_hash_out[2], ll_hash_out[3], ll_hash_out[4], ll_hash_out[5], ll_hash_out[6], ll_hash_out[7]);

		/*
		for (i = 0; i < 5; i++) {
			int j;
			for (j = 0; j < 2; j++) {
				char temp[2];

				temp[0] = hash_out_str[(i * 8) + (j * 2)];
                temp[1] = hash_out_str[(i * 8) + (j * 2) + 1];

                hash_out_str[(i * 8) + (j * 2)] = hash_out_str[(i * 8) + (6 - (j * 2))];
                hash_out_str[(i * 8) + (j * 2) + 1] = hash_out_str[(i * 8) + (7 - (j * 2))];

                hash_out_str[(i * 8) + (6 - (j * 2))] = temp[0];
                hash_out_str[(i * 8) + (7 - (j * 2))] = temp[1];
			}
		}
		 */

		printf("%s\n", hash_out_str);
		break;
	case H_SHA384:
		sha2_init(SHA384);
		if (string_input && string_to_process != NULL) {
			sha2_add_string(string_to_process);
		} else if (file_input) {
			fp = fopen(file_to_process, "r");
			sha2_add_file(fp);
		}
		sha2_get_hash(ll_hash_out);

		sprintf(hash_out_str, "%016llx%016llx%016llx%016llx%016llx%016llx", ll_hash_out[0], ll_hash_out[1], ll_hash_out[2], ll_hash_out[3], ll_hash_out[4], ll_hash_out[5]);

		/*
		for (i = 0; i < 5; i++) {
			int j;
			for (j = 0; j < 2; j++) {
				char temp[2];

				temp[0] = hash_out_str[(i * 8) + (j * 2)];
                temp[1] = hash_out_str[(i * 8) + (j * 2) + 1];

                hash_out_str[(i * 8) + (j * 2)] = hash_out_str[(i * 8) + (6 - (j * 2))];
                hash_out_str[(i * 8) + (j * 2) + 1] = hash_out_str[(i * 8) + (7 - (j * 2))];

                hash_out_str[(i * 8) + (6 - (j * 2))] = temp[0];
                hash_out_str[(i * 8) + (7 - (j * 2))] = temp[1];
			}
		}
		 */

		printf("%s\n", hash_out_str);
		break;
	}

	return 0;
}
