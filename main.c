/**
 * @file main.c
 * A collection of common hashing algorithms for strings and files
 *
 * Front-end for the libraries
 *
 * @author	FergoFrog <fergofrog@fergofrog.com>
 * @version 0.4
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "global.h"
#include "file.h"
#include "md5/md5.h"
#include "sha1/sha1.h"
#include "sha2/sha2.h"

/** Version number */
#define VERSION "0.4 pre-alpha"

/** Boolean True definition */
#define TRUE 1
/** Boolean False definition */
#define FALSE 0

enum target_t {
    T_FILE,
    T_STRING
};

struct args_t {
    enum hash_t hash;
    enum target_t target_type;
    unsigned int no_targets;
    struct file_list_t *target;
};

/** Print version to stdout */
void print_version()
{
	printf("hasher, version " VERSION "\n");
}

/**
 * Print help to stdout
 *
 * @param program Program executable name
 */
void print_help(char *program, FILE *fout)
{
	fprintf(fout, "usage: %s [--algo] [-s string] [-hv] [file]\n\n",
			program);
	fprintf(fout, "\t-s, --string\tstring input\n");
	fprintf(fout, "\t-h, --help\tthis message\n");
    fprintf(fout, "\t-v, --version\tversion info");
    fprintf(fout, "\nKnown Algorithms:\n");
    fprintf(fout, "\t    --md5\tMD5 (RFC 1321) - Default\n");
    fprintf(fout, "\t    --sha1\tSHA1 (RFC 3174)\n");
    fprintf(fout, "\t    --sha256\tSHA256 (RFC 3174)\n");
    fprintf(fout, "\t    --sha224\tSHA224 (RFC 3174)\n");
    fprintf(fout, "\t    --sha512\tSHA512 (RFC 3174)\n");
    fprintf(fout, "\t    --sha384\tSHA384 (RFC 3174)\n");
}

/**
 * Process the arguments given to the program (using getopt)
 *
 * @param argc Number of arguments
 * @param argv The arguments (array of strings)
 * @param args The args_t structure to store the resulting arguments
 */
void process_args(int argc, char *const *argv, struct args_t *args)
{
    /* Variable to store temporary copy of hash */
    static int hash_type;
    /* Recognised long options */
    static struct option longopts[] = {
        {"md5", no_argument, &hash_type, H_MD5},
        {"sha1", no_argument, &hash_type, H_SHA1},
        {"sha256", no_argument, &hash_type, H_SHA256},
        {"sha224", no_argument, &hash_type, H_SHA224},
        {"sha512", no_argument, &hash_type, H_SHA512},
        {"sha384", no_argument, &hash_type, H_SHA384},
        {"string", required_argument, NULL, 's'},
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'}
    };
    /* Recognised short options */
    static char *shortopts = "s:hvV";

    struct file_list_t *cur_file;
    int indexptr;
    int c;
    ino_t cur_ino;

    /* Assuming it's a file input, unless string given */
    args->target_type = T_FILE;

    while ((c = getopt_long(argc, argv, shortopts, longopts, &indexptr)) != -1){
        switch (c) {
        case 0:
            /* Long options */
            /* Those with short options will go to their short cases,
             * otherwise the specified flag will be set
             */
            break;
        case 's':
            /* String target selected */
            args->target_type = T_STRING;
            args->target = malloc(sizeof(struct file_list_t));
            args->target->file = optarg;
            /* Helps with freeing in main */
            args->no_targets = 1;
            args->target->dyn_alloc = 0;
            break;
        case 'h':
            /* Help requested */
            print_help(argv[0], stdout);
            exit(0);
            break;
        case 'v':
            /* Program version */
            print_version();
            exit(0);
            break;
        case 'V':
            /* Increment verbosity level */
            verbose_level++;
            break;
        case '?':
            /* Unknown option */
            print_help(argv[0], stderr);
            exit(1);
            break;
        default:
            /* Something really buggered */
            fprintf(stderr,
                    "Error: getopt returned an unexpected value (0x%0x)!\n",
                    c);
            abort();
        }
    }

    /* Copy hash type over */
    args->hash = hash_type;

    /* Check whether a string was given as an option argument */
    if (args->target_type == T_FILE) {
        /* Check whether any non-option arguments remain
         *  loop through creating a list if so
         */
        if (optind < argc) {
            args->no_targets = 0;

            /* Check the argument is a file or directory (and get its ino) */
            if (!(cur_ino = is_filedir(argv[optind]))) {
                fprintf(stderr,
                    "Error: \"%s\" is not a file or directory.\n\n",
                    argv[optind]);
                print_help(argv[0], stderr);
                exit(1);
            }

            /* Create the file list head */
            args->target = cur_file = malloc(sizeof(struct file_list_t));
            cur_file->dyn_alloc = 0;
            cur_file->file = argv[optind++];
            cur_file->ino = cur_ino;
            args->no_targets++;

            /* Push the files onto the list */
            while (optind < argc) {
                if (!(cur_ino = is_filedir(argv[optind]))) {
                    fprintf(stderr,
                        "Error: \"%s\" is not a file or directory.\n\n",
                        argv[optind]);
                    print_help(argv[0], stderr);
                    exit(1);
                } 

                if (file_listed(cur_ino, args->no_targets, args->target)) {
                    fprintf(stderr, "Error: \"%s\" is listed twice.\n",
                        argv[optind]);
                    exit(1);
                }

                cur_file->next = malloc(sizeof(struct file_list_t));
                cur_file = cur_file->next;

                cur_file->dyn_alloc = 0;
                cur_file->file = argv[optind++];
                cur_file->ino = cur_ino;
                args->no_targets++;
           }
        } else {
            /* No string given and no additional arguments remain */
            fprintf(stderr, "Error: No string or file given.\n\n");
            print_help(argv[0], stderr);
            exit(1);
        }
    }

    /* Verbose processing */
    if (verbose_level >= 1) {
        printf("V: Verbose level: %i\n", verbose_level);

        printf("V: Hashing algo: %s\n", hash_names[args->hash]);

        printf("V: Hash target: %s\n", args->target_type == T_STRING ? "String" : "File");

        if (args->target_type == T_STRING) {
            printf("V: String: %s\n", args->target->file);
        } else {
            if (verbose_level >=2) {
                int i;
                printf("VV: Files (%i):\n", args->no_targets);
                cur_file = args->target;
                for (i = 0; i < args->no_targets; i++) {
                    if (verbose_level >= 3) {
                        printf("VVV:\t%lu\t%s\n", cur_file->ino, cur_file->file);
                    } else {
                        printf("VV:\t%s\n", cur_file->file);
                    }
                
                    cur_file = cur_file->next;
                }
            } else {
                printf("V: Files given: %i\n", args->no_targets);
            }
        }
    }
}

/**
 * Main method
 *
 * @param argc Argument count
 * @param argv Arguments
 */
int main(int argc, char *argv[])
{
	unsigned int i;
    struct args_t args;
    struct file_list_t *cur_file;
    FILE *fp;
    char *hash_out;
    void *hash_state;
    
	/* Not in hash yet */
	in_hash = 0;

    /* Process the arguments */
    process_args(argc, argv, &args);
    
    /* Expand the directories */
    if (args.target_type == T_FILE)
        args.no_targets = expand_files(args.no_targets, &(args.target));

    /* Verbose processing */
    if (verbose_level >= 1 && args.target_type == T_FILE) {
        if (verbose_level >= 2) {
            printf("\nVV: Files expanded into (%i):\n", args.no_targets);
            cur_file = args.target;
            for (i = 0; i < args.no_targets; i++) {
                if (verbose_level >= 3) {
                    printf("VVV:\t%lu\t%s\n", cur_file->ino, cur_file->file);
                } else {
                    printf("VV:\t%s\n", cur_file->file);
                }

                cur_file = cur_file->next;
            }
        } else {
            printf("V: Expanded into: %i\n", args.no_targets);
        }
    }

    /* Loop through all of the targets (file(s)/string) */
    cur_file = args.target;
    for (i = 0; i < args.no_targets; i++) {
        /* Open the current file */
        if (args.target_type == T_FILE) {
            fp = fopen(cur_file->file, "r");
            if (fp == NULL) {
                fprintf(stderr, "Error: Couldn't open file \"%s\".\n",
                        cur_file->file);
            }
        }

        /* Do the correct hash */
        switch (args.hash) {
        case H_MD5:
            /* Initialise MD5 hashing */
            hash_state = malloc(sizeof(struct md5_state));
            if (!md5_init(hash_state)) {
                fprintf(stderr, "Error: Couldn't initialise MD5 hashing.\n");
                return 1;
            }

            if (args.target_type == T_STRING) {
                /* Hash the string */
                if (!md5_add_string(hash_state, cur_file->file)) {
                    fprintf(stderr, "Error: Couldn't add string.\n");
                    return 1;
                }
            } else if (args.target_type == T_FILE) {
                /* Hash the file */
                if (!md5_add_file(hash_state, fp)) {
                    fprintf(stderr, "Error: Couldn't add file \"%s\".\n",
                        cur_file->file);
                    return 1;
                }
            }

            /* Get the hash */
            hash_out = malloc(33 * sizeof(char));
            if (!md5_get_hash_str(hash_state, hash_out)) {
                fprintf(stderr, "Error: Couldn't complete hashing.\n");
                return 1;
            }

            /* Print the hash */
            if (args.target_type == T_STRING) {
                printf("\"%s\" = %s\n", cur_file->file, hash_out);
            } else {
                printf("%s  %s\n", hash_out, cur_file->file);
            }

            free(hash_out);
            free(hash_state);
            break;
        default:
            printf("Error: Hash function not implemented yet.\n");
            return 1;
        }
        
        /* Next file */
        cur_file = cur_file->next;
    }

#if 0
	/* Array of 32 bit unsigned ints for MD5, SHA1, SHA256, SHA224 */
	unsigned int i_hash_out[8];
	/* Array of 64 bit unsigned ints for SHA512, SHA384 */
	unsigned long long ll_hash_out[8];
	/* String for the hash representation */
	char hash_out_str[129];
	/* File pointer for file input */
	FILE *fp;

	/* Start processing the hash */
	switch (args.hash) {
	case H_MD5:
		/* Initialise MD5 hashing */
		md5_init();

		if (args.target_type == T_STRING) {
			/* Hash the string */
			md5_add_string(args->target->file);
		} else if (args.target_type == T_FILE) {
			/* Hash the file */
			fp = fopen(args->target->file, "r");
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
    default:
        break;
	}
#endif

    free_file_list(args.no_targets, args.target);

	return 0;
}
