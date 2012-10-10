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
/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include "global.h"
#include "file.h"
#include "hash.h"
#include "md5/md5.h"
#include "sha1/sha1.h"
#include "sha2/sha2.h"

/** Version number */
#define VERSION "0.4 pre-alpha"

/** Boolean True definition */
#define TRUE 1
/** Boolean False definition */
#define FALSE 0

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
#ifndef NTHERAD
    fprintf(fout, "\t-t, --threads\tnumber of thread (default: no cores, 0 to disable)");
#endif
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
        {"threads", required_argument, NULL, 't'},
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'}
    };
    /* Recognised short options */
    static char *shortopts = "s:t:hvVH";

    struct file_list_t *cur_file;
    int indexptr;
    int c;
    ino_t cur_ino;

    /* Clear the flags */
    args->flags = 0;

    /* Assuming it's a file input, unless string given */
    args->target_type = T_FILE;

    /* Default of 2 threads */
    /* TODO: change to no. CPUs */
    args->no_threads = 2;

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
            check_malloc(args->target);
            args->target->file = optarg;
            /* Helps with freeing in main */
            args->no_targets = 1;
            args->target->flags &= !FILE_DYN_ALLOC;
            break;
        case 'h':
            /* Help requested */
            print_help(argv[0], stdout);
            exit(0);
            break;
        case 't':
            /* Number of threads */
            errno = 0;
            char *conv_end;
            long int temp_val = strtol(optarg, &conv_end, 10);
            if (errno != 0 || optarg == conv_end || temp_val < 0) {
                perror("Error: not a valid number of threads");
                exit(1);
            } else {
                args->no_threads = (unsigned int) temp_val;
            }
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
        case 'H':
            /* Do not hash */
            args->flags |= ARG_NO_HASH;
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

#ifdef NTHREAD
    /* Must disable threading */
    args->flags |= ARG_NO_THREAD
#endif

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
            check_malloc(cur_file);
            cur_file->flags &= !FILE_DYN_ALLOC;
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
                check_malloc(cur_file->next);
                cur_file = cur_file->next;

                cur_file->flags &= !FILE_DYN_ALLOC;
                cur_file->file = argv[optind++];
                cur_file->ino = cur_ino;
                args->no_targets++;
           }
        } else {
            /* No string given and no additional arguments remain */
            fprintf(stderr, "Error: no string or file given.\n\n");
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

    if (!(args.flags & ARG_NO_HASH)) {
        do_hash(&args);
        print_hash(&args);
    }

    free_file_list(args.no_targets, args.target);

	return 0;
}
