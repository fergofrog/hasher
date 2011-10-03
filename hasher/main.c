/**
 * @file main.c
 * A collection of common hashing algorithms for strings and files
 *
 * Front-end for the libraries
 *
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
/* Includes */
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "md5/md5.h"
#include "sha1/sha1.h"
#include "sha2/sha2.h"

/** Version number */
#define VERSION "0.3"

/** Boolean True definition */
#define TRUE 1
/** Boolean False definition */
#define FALSE 0

/** Hash types known */
enum hash_t {
	H_MD5,
	H_SHA1,
	H_SHA256,
	H_SHA224,
	H_SHA512,
	H_SHA384
};

extern char in_hash;

/** Print version to stdout */
void print_version()
{
	printf("hasher, version " VERSION "\n\n");
}

/**
 * Print help to stdout
 *
 * @param program Program executable name
 */
void print_help(char *program)
{
	printf("usage: %s [-h] [--md5] [--sha1] [-s string] [-f file]\n\n", program);
	printf("\t    --md5\tuse md5\n");
	printf("\t    --sha1\tuse sha1\n");
	printf("\t-s, --string\tstring input\n");
	printf("\t-f, --file\tfile input\n");
	printf("\t-h, --help\tthis message\n");
}

/**
 * Main method
 *
 * @param argc Argument count
 * @param argv Arguments
 */
int main(int argc, char *argv[])
{
	/* Type of input */
	char string_input = FALSE, file_input = FALSE;
	/* Hash type */
	enum hash_t hash;

	/* Pointers to strings */
	char *string_to_process;
	char *file_to_process;

	/* Not in hash yet */
	in_hash = 0;

	int i = 1;
	while (i < argc) {
		switch (argv[i][1]) {
		case '-':
			/* Double dash given (--) */
			if (strcmp(argv[i] + 2, "md5") == 0) {
				/* --md5 */
				hash = H_MD5;
			} else if (strcmp(argv[i] + 2, "sha1") == 0) {
				/* --sha1 */
				hash = H_SHA1;
			} else if (strcmp(argv[i] + 2, "sha256") == 0) {
				/* --sha256 */
				hash = H_SHA256;
			} else if (strcmp(argv[i] + 2, "sha224") == 0) {
				/* --sha224 */
				hash = H_SHA224;
			} else if (strcmp(argv[i] + 2, "sha512") == 0) {
				/* --sha512 */
				hash = H_SHA512;
			} else if (strcmp(argv[i] + 2, "sha384") == 0) {
				/* --sha384 */
				hash = H_SHA384;
			} else if (strcmp(argv[i] + 2, "string") == 0) {
				/* --string */
				string_input = TRUE;
				string_to_process = argv[++i];
			} else if (strcmp(argv[i] + 2, "file") == 0) {
				/* --file */
				file_input = TRUE;
				file_to_process = argv[++i];
			} else if (strcmp(argv[i] + 2, "help") == 0) {
				/* --help */
				string_input = file_input = FALSE;
				print_help(argv[0]);
				return 0;
			} else {
				/* --[something else] */
				string_input = file_input = FALSE;
				printf("Unknown command %s\n\n", argv[i]);
				print_help(argv[0]);
				return 1;
			}
			break;
		case 's':
			/* String Input (-s) */
			string_input = TRUE;
			string_to_process = argv[++i];
			break;
		case 'f':
			/* File Input (-f) */
			file_input = TRUE;
			file_to_process = argv[++i];
			break;
		case 'h':
			/* Help message (-h) */
			string_input = file_input = 0;
			print_help(argv[0]);
			return 0;
		default:
			/* Something unknown */
			string_input = file_input = 0;
			printf("Unknown command %s\n\n", argv[i]);
			print_help(argv[0]);
			return 1;
		}

		/* Go to the next argument */
		i++;
	}

	/* Array of 32 bit unsigned ints for MD5, SHA1, SHA256, SHA224 */
	unsigned int i_hash_out[8];
	/* Array of 64 bit unsigned ints for SHA512, SHA384 */
	unsigned long long ll_hash_out[8];
	/* String for the hash representation */
	char hash_out_str[129];
	/* File pointer for file input */
	FILE *fp;

	/* Start processing the hash */
	switch (hash) {
	case H_MD5:
		/* Initialise MD5 hashing */
		md5_init();

		if (string_input && string_to_process != NULL) {
			/* Hash the string */
			md5_add_string(string_to_process);
		} else if (file_input && file_to_process != NULL) {
			/* Hash the file */
			fp = fopen(file_to_process, "r");
			if (fp != NULL)
				md5_add_file(fp);
		}

		/* Get the hash */
		md5_get_hash(i_hash_out);

		/* Get the string representation of the hash */
		sprintf(hash_out_str, "%08x%08x%08x%08x",
				i_hash_out[0], i_hash_out[1],
				i_hash_out[2], i_hash_out[3]);

		/* Swap the characters around */
		/* Outer loop: 4 groups of 8 */
		for (i = 0; i < 4; i++) {
			/* Inner loop: 2 pairs of 2 characters */
			int j;
			for (j = 0; j < 2; j++) {
				char temp[2];

				/* Save the left pair */
				temp[0] = hash_out_str[(i * 8) + (j * 2)];
                temp[1] = hash_out_str[(i * 8) + (j * 2) + 1];

                /* Copy the right pair across */
                hash_out_str[(i * 8) + (j * 2)] =
                		hash_out_str[(i * 8) + (6 - (j * 2))];
                hash_out_str[(i * 8) + (j * 2) + 1] =
                		hash_out_str[(i * 8) + (7 - (j * 2))];

                /* Copy the original left to the right */
                hash_out_str[(i * 8) + (6 - (j * 2))] = temp[0];
                hash_out_str[(i * 8) + (7 - (j * 2))] = temp[1];
			}
		}

		/* Print the string */
		printf("%s\n", hash_out_str);
		break;
	case H_SHA1:
		/* Initialise SHA1 hashing */
		sha1_init();

		if (string_input && string_to_process != NULL) {
			/* Hash the string */
			sha1_add_string(string_to_process);
		} else if (file_input && file_to_process != NULL) {
			/* Hash the file */
			fp = fopen(file_to_process, "r");
			if (fp != NULL)
				sha1_add_file(fp);
		}

		/* Get the hash */
		sha1_get_hash(i_hash_out);

		/* Get the string representation and print */
		printf("%08x%08x%08x%08x%08x\n",
				i_hash_out[0], i_hash_out[1], i_hash_out[2],
				i_hash_out[3], i_hash_out[4]);
		break;
	case H_SHA256:
		/* Initialise SHA256 hashing */
		sha2_init(SHA256);

		if (string_input && string_to_process != NULL) {
			/* Hash the string */
			sha2_add_string(string_to_process);
		} else if (file_input && file_to_process != NULL) {
			/* Hash the file */
			fp = fopen(file_to_process, "r");
			if (fp != NULL)
				sha2_add_file(fp);
		}

		/* Get the hash */
		sha2_get_hash(ll_hash_out);

		/* Get the string representation and print */
		printf("%08llx%08llx%08llx%08llx%08llx%08llx%08llx%08llx\n",
			ll_hash_out[0], ll_hash_out[1], ll_hash_out[2], ll_hash_out[3],
			ll_hash_out[4], ll_hash_out[5], ll_hash_out[6], ll_hash_out[7]);
		break;
	case H_SHA224:
		/* Initialise SHA224 hashing */
		sha2_init(SHA224);

		if (string_input && string_to_process != NULL) {
			/* Hash the string */
			sha2_add_string(string_to_process);
		} else if (file_input && file_to_process != NULL) {
			/* Hash the file */
			fp = fopen(file_to_process, "r");
			if (fp != NULL)
				sha2_add_file(fp);
		}

		/* Get the hash */
		sha2_get_hash(ll_hash_out);

		/* Get the string representation and print */
		printf("%08llx%08llx%08llx%08llx%08llx%08llx%08llx\n",
			ll_hash_out[0], ll_hash_out[1], ll_hash_out[2], ll_hash_out[3],
			ll_hash_out[4], ll_hash_out[5], ll_hash_out[6]);
		break;
	case H_SHA512:
		/* Initialise SHA512 hashing */
		sha2_init(SHA512);

		if (string_input && string_to_process != NULL) {
			/* Hash the string */
			sha2_add_string(string_to_process);
		} else if (file_input && file_to_process != NULL) {
			/* Hash the file */
			fp = fopen(file_to_process, "r");
			if (fp != NULL)
				sha2_add_file(fp);
		}

		/* Get the hash */
		sha2_get_hash(ll_hash_out);

		/* Get the string representation and print */
		printf("%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx\n",
			ll_hash_out[0], ll_hash_out[1], ll_hash_out[2], ll_hash_out[3],
			ll_hash_out[4], ll_hash_out[5], ll_hash_out[6], ll_hash_out[7]);
		break;
	case H_SHA384:
		/* Initialise SHA384 hashing */
		sha2_init(SHA384);

		if (string_input && string_to_process != NULL) {
			/* Hash the string */
			sha2_add_string(string_to_process);
		} else if (file_input) {
			/* Hash the file */
			fp = fopen(file_to_process, "r");
			if (fp != NULL)
				sha2_add_file(fp);
		}

		/* Get the hash */
		sha2_get_hash(ll_hash_out);

		/* Get the string representation and print */
		printf("%016llx%016llx%016llx%016llx%016llx%016llx\n",
				ll_hash_out[0], ll_hash_out[1], ll_hash_out[2],
				ll_hash_out[3], ll_hash_out[4], ll_hash_out[5]);
		break;
	}

	return 0;
}
