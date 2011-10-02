/*
 * main.c
 *
 *  Created on: Oct 1, 2011
 *      Author: FergoFrog
 */

#include <stdio.h>
#include <string.h>

#include "md5/md5.h"

#define VERSION "0.1"

#define TRUE 1
#define FALSE 0

enum hash_t {
	MD5,
	SHA1
};

void print_version()
{
	printf("hasher, version " VERSION "\n\n");
}

void print_help(char *program)
{
	printf("usage: %s [-h] [--md5] [-s string] [-f file]\n\n", program);
	printf("\t    --md5\tuse md5\n");
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

	unsigned int hash_out[4];

	int i = 1;
	while (i < argc) {
		switch (argv[i][1]) {
		case '-':
			if (strcmp(argv[i] + 2, "md5") == 0) {
				hash = MD5;
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

	switch (hash) {
	case MD5:
		md5_init();
		if (string_input) {
			unsigned int len = (unsigned int) strlen(string_to_process);

			md5_add_string(string_to_process, len);
		} else if (file_input) {
			FILE *fp = fopen(file_to_process, "r");
			md5_add_file(fp);
		}
		md5_get_hash(hash_out);

		char hash_out_str[34];
		sprintf(hash_out_str, "%08x%08x%08x%08x", hash_out[0], hash_out[1], hash_out[2], hash_out[3]);

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
	case SHA1:
		break;
	}

	return 0;
}
