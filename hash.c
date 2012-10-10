/**
 * @file hash.c
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

#ifndef NTHREAD
#include <pthread.h>
#else
/* Define the functions so it compiles */
typedef void pthread_t;
typedef void pthread_mutex_t;
typedef void pthread_mutexattr_t;
pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
pthread_join(pthread_t, void **);
pthread_mutex_init(pthread_mutex_t *restrict, const pthread_mutexattr_t *restrict);
pthread_mutex_lock(pthread_mutex_t *);
pthread_mutex_unlock(pthread_mutex_t *);
#endif

#include "global.h"
#include "file.h"
#include "md5/md5.h"
#include "sha1/sha1.h"
#include "sha2/sha2.h"

struct thread_data_t {
    unsigned int no_threads;
    unsigned int no_files;
    unsigned int cur_file_idx;
    enum target_t target_type;
    struct file_list_t *files;
    pthread_t *hash_threads;
    pthread_mutex_t cur_file_mutex;
};

void *do_md5(void *);

void do_hash(struct args_t *args)
{
    unsigned int i;
    void *(*do_hash_algo)(void *);
    struct thread_data_t *thread_data;
    void *ret_val;

    thread_data = malloc(sizeof(struct thread_data_t));

    thread_data->cur_file_idx = 0;
    thread_data->no_threads = args->no_threads;
    thread_data->no_files = args->no_targets;
    thread_data->target_type = args->target_type;
    thread_data->files = args->target;
    if (args->no_threads > 0) {
        pthread_mutex_init(&thread_data->cur_file_mutex, NULL);
        thread_data->hash_threads = malloc(args->no_threads * sizeof(pthread_t));
    }

    switch (args->hash) {
    case H_MD5:
        do_hash_algo = do_md5;
        break;
    default:
        fprintf(stderr, "Error: hash function not implemented yet.\n");
        exit(1);
    }

    if (args->no_threads <= 0) {
        do_hash_algo((void *) thread_data);
    } else {
        for (i = 0; i < args->no_threads; i++) {
            if (pthread_create(&thread_data->hash_threads[i], NULL, do_hash_algo, (void *) thread_data) != 0) {
                perror("Error: failed to create hash thread");
            }
        }
        for (i = 0; i < args->no_threads; i++) {
            if (pthread_join(thread_data->hash_threads[i], &ret_val) != 0) {
                perror("Error: failed to rejoin hash thread'");
            }
        }
    }
}

void print_hash(struct args_t *args)
{
    unsigned int i;
    char *format_str, *fmt_first_arg, *fmt_second_arg;
    char **file_arg, **hash_arg;
    struct file_list_t *cur_file;
    
    if (args->target_type == T_STRING) {
        format_str = "\"%s\" = %s\n";
        file_arg = &fmt_first_arg;
        hash_arg = &fmt_second_arg;
    } else {
        format_str = "%s  %s\n";
        file_arg = &fmt_second_arg;
        hash_arg = &fmt_first_arg;
    }

    cur_file = args->target;

    for (i = 0; i < args->no_targets; i++) {
        *file_arg = cur_file->file;
        *hash_arg = cur_file->hash;
        printf(format_str, fmt_first_arg, fmt_second_arg);
    }
}

void *do_md5(void *thread_data_v)
{
    char threaded;
    unsigned int i;
    unsigned int cur_file_idx, new_file_idx;
    struct thread_data_t *thread_data;
    struct file_list_t *cur_file;
    struct md5_state *hash_state;
    FILE *fp;

    thread_data = (struct thread_data_t *) thread_data_v;
    cur_file = thread_data->files;
    cur_file_idx = 0;
    threaded = thread_data->no_threads > 0;

    /* TODO: Try removing */
    if (threaded) {
        pthread_mutex_lock(&thread_data->cur_file_mutex);
        new_file_idx = thread_data->cur_file_idx++;
        pthread_mutex_unlock(&thread_data->cur_file_mutex);
    } else {
        new_file_idx = thread_data->cur_file_idx++;
    }

    while (new_file_idx < thread_data->no_files) {
        /* Traverse the file list to get the next file */
        for (i = 0; i < new_file_idx - cur_file_idx; i++) {
            cur_file = cur_file->next;
        }
        
        /* Initialise MD5 hashing */
        hash_state = malloc(sizeof(struct md5_state));
        check_malloc(hash_state);
        if (!md5_init(hash_state)) {
            fprintf(stderr, "Error: couldn't initialise MD5 hashing.\n");
        }

        if (thread_data->target_type == T_FILE) {
            /* Open the file */
            fp = fopen(cur_file->file, "rb");
            if (fp == NULL) {
                fprintf(stderr, "Error: couldn't open file \"%s\": ", cur_file->file);
                perror(NULL);
            }

            /* Hash the file */
            if (!md5_add_file(hash_state, fp)) {
                fprintf(stderr, "Error: couldn't hash the file \"%s\".\n", cur_file->file);
            }
            
            /* Close the file */
            fclose(fp);
        } else if (thread_data->target_type == T_STRING) {
            /* Hash the string */
            if (!md5_add_string(hash_state, cur_file->file)) {
                fprintf(stderr, "Error: couldn't hash the string.\n");
            }
        }

        /* Get the hash */
        cur_file->hash = malloc((MD5_STRING_LEN + 1) * sizeof(char));
        check_malloc(cur_file->hash);
        if (!md5_get_hash_str(hash_state, cur_file->hash)) {
            fprintf(stderr, "Error: couldn't complete hashing.\n");
        }

        free(hash_state);

        /* TODO: Try removing */
        if (threaded) {
            pthread_mutex_lock(&thread_data->cur_file_mutex);
            new_file_idx = thread_data->cur_file_idx++;
            pthread_mutex_unlock(&thread_data->cur_file_mutex);
        } else {
            new_file_idx = thread_data->cur_file_idx++;
        }
    }

    if (threaded)
        pthread_exit(NULL);
    return NULL;
}
