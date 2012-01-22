/**
 * @file file.c
 * Various file operations
 * @author	FergoFrog <fergofrog@fergofrog.com>
 * @version 0.4
 *
 * @section LICENSE
 * Copyright (C) 2012 FergoFrog
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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "file.h"

/**
 * Check whether filename given is a file
 *
 * @param filename Path to the test file
 * @return The file's inode number, otherwise 0
 */
ino_t is_file(char *filename)
{
    struct stat buf;
    if (stat(filename, &buf) == 0 && S_ISREG(buf.st_mode))
        return buf.st_ino;
    else
        return 0;
}

/**
 * Check whether dirname given is a directory
 *
 * @param dirname Path to the test directory
 * @return The directory's inode number, otherwise 0
 */
ino_t is_dir(char *dirname)
{
    struct stat buf;
    if (stat(dirname, &buf) == 0 && S_ISDIR(buf.st_mode))
        return buf.st_ino;
    else
        return 0;
}

/**
 * Check whether path given exists and is a file or directory
 *
 * @param path Path to test
 * @return The file or directory's inode number, otherwise 0
 */
ino_t is_filedir(char *filename)
{
    struct stat buf;
    if (stat(filename, &buf) == 0 && (S_ISDIR(buf.st_mode) | S_ISREG(buf.st_mode)))
        return buf.st_ino;
    else
        return 0;
}

/**
 * Frees a list of files
 *
 * @param n_files Number of files in the list
 * @param cur First file in the list
 */
void free_file_list(unsigned int n_files, struct file_list_t *cur)
{
    struct file_list_t *prev;
    int i;

    for (i = 0; i < n_files; i++) {
        prev = cur;
        cur = cur->next;
        if (prev->dyn_alloc)
            free(prev->file);
        free(prev);
    }
}

char *make_file_name(char *, char *);

/**
 * Expand all directories in the file list into their child files.
 * The files are added as a depth-first search (and sorted)
 *
 * @param n_files The number of elements (files and dirs) in the list
 * @param head A pointer to the first element of the file list
 * @returns The new number of elements in the list
 */
unsigned int expand_files(unsigned int n_files, struct file_list_t **head)
{
    struct file_list_t *prev, *cur, *next, *new_entry;
    struct file_list_t *sort_cur, *sort_prev;
    struct file_list_t *dirs_expanded_head, *cur_dir_expanded;
    DIR *dir;
    struct dirent *dir_entry;
    char *temp_file, *temp_dir;
    ino_t cur_ino;
    unsigned int i, n_expanded, n_inserted, sort_i;

    prev = NULL;
    cur = *head;
    next = cur->next;

    /* Loop through the files, expanding the directories as encountered */
    i = n_expanded = 0;
    while (i < n_files) {
        /* Check whether the current file is a directory */
        if ((cur_ino = is_dir(cur->file)) != 0) {
            /* Save some work by ensuring we haven't already expanded */
            if (n_expanded) {
                if (file_listed(cur_ino, n_expanded, dirs_expanded_head)) {
                    /* Already expanded, remove and continue */
                    if (cur->dyn_alloc)
                        free(cur->file);
                    free(cur);

                    if (prev)
                        prev->next = next;
                    cur = next;
                    if (cur)
                        next = cur->next;

                    n_files--;

                    continue;
                } else {
                    /* Haven't expanded, add it to the list */
                    cur_dir_expanded->next =
                        malloc(sizeof(struct file_list_t));
                    cur_dir_expanded = cur_dir_expanded->next;

                    cur_dir_expanded->dyn_alloc = 0;
                    cur_dir_expanded->file = NULL;
                    cur_dir_expanded->ino = cur_ino;
                    cur_dir_expanded->next = NULL;
                    n_expanded++;
                }
            } else {
                /* This is the first expansion, init the list */
                dirs_expanded_head = cur_dir_expanded =
                    malloc(sizeof(struct file_list_t));

                cur_dir_expanded->dyn_alloc = 0;
                cur_dir_expanded->file = NULL;
                cur_dir_expanded->ino = cur_ino;
                cur_dir_expanded->next = NULL;
                n_expanded++;
            }

            /* Copy the directory's name */
            temp_dir = malloc((strlen(cur->file) + 1) * sizeof(char));
            strcpy(temp_dir, cur->file);

            /* Open the directory */
            if ((dir = opendir(temp_dir)) != NULL) {
                n_inserted = 0;

                /* Loop through the directory entries, 
                 * adding other dirs and files (but omitting '.' and '..')
                 */
                while ((dir_entry = readdir(dir)) != NULL) {
                    /* Make the complete file name by prepending
                     * the directory path to the file name
                     */
                    temp_file = make_file_name(temp_dir, dir_entry->d_name);
                    
                    if ((cur_ino = is_filedir(temp_file)) &&
                            strcmp(dir_entry->d_name, ".") &&
                            strcmp(dir_entry->d_name, "..") &&
                            !file_listed(cur_ino, n_files, *head)) {
                        /* Create the new list entry */
                        new_entry = malloc(sizeof(struct file_list_t));
                        new_entry->dyn_alloc = 1;
                        new_entry->file = temp_file;
                        new_entry->ino = cur_ino;
                        
                        /* Insert the entry */
                        if (n_inserted > 0) {
                            /* Sort (insertion sort) the entry in */
                            sort_prev = prev;
                            sort_cur = cur;
                            sort_i = 0;

                            /* Find the position */
                            while (sort_i < n_inserted &&
                                    strcmp(sort_cur->file, temp_file) < 0) {
                                sort_prev = sort_cur;
                                sort_cur = sort_cur->next;
                                sort_i++;
                            }

                            /* Insert the new entry in */
                            new_entry->next = sort_cur;
                            if (sort_prev != NULL)
                                sort_prev->next = new_entry;

                            /* Boundry cases (if inserted as 1st or 2nd) */
                            if (sort_i == 0) {
                                next = cur;
                                cur = new_entry;
                                if (i == 0)
                                    *head = new_entry;
                            } else if (sort_i == 1) {
                                next = new_entry;
                            }

                            /* Update the counters */
                            n_files++;
                            n_inserted++;
                        } else {
                            /* None inserted prior,
                             *  replace the directry entry with this one
                             */

                            /* Free the filename, if allocated by malloc */
                            if (cur->dyn_alloc)
                                free(cur->file);
                            /* Free the node */
                            free(cur);

                            /* Update the previous node's reference */
                            if (prev != NULL)
                                prev->next = new_entry;

                            /* Update the next reference in the new node */
                            new_entry->next = next;

                            /* Update the global entries */
                            cur = new_entry;
                            if (i == 0)
                                *head = new_entry;

                            n_inserted++;
                        }
                    } else {
                        free(temp_file);
                    }
                }
                closedir(dir);
            }

            free(temp_dir);
        } else {
            /* Is a file - move along */
            prev = cur;
            cur = next;
            if (i + 1 < n_files)
                next = cur->next;
            i++;
        }
    }

    /* Free the dirs expanded list */
    free_file_list(n_expanded, dirs_expanded_head);
    
    return n_files;
}

/**
 * Check whether the file that is described by target_ino
 * already exists in the file list
 *
 * @param target_ino Target inode number
 * @param n_files Number of files in the list
 * @param flist_head First element of the file list
 * @return 1 if found, 0 if not
 */
char file_listed(ino_t target_ino, unsigned int n_files,
    struct file_list_t *flist_head)
{
    struct file_list_t *cur = flist_head;
    unsigned int i;
    
    for (i = 0; i < n_files; i++) {
        if (cur->ino == target_ino)
            return 1;
        
        cur = cur->next;
    }

    return 0;
}

char *make_file_name(char *dir, char *name)
{
    unsigned int dir_len, name_len, i, rel_i;
    char *new_name;
    char adding_fslash;

    /* Get the string lengths */
    dir_len = strlen(dir);
    name_len = strlen(name);

    /* Check whether a forward slash (/) is already in dir */
    adding_fslash = !(dir[dir_len - 1] == '/' || dir[dir_len - 1] == '\\');

    /* Allocate the space for the name,
     * plus 1 for separator (if needed) and 1 for null terminator
     */
    new_name = malloc((dir_len + adding_fslash + name_len + 1) * sizeof(char));

    i = 0;
    /* Copy the directory over */
    while (i < dir_len) {
        new_name[i] = dir[i];
        i++;
    }

    /* Add directory separator (if needed) */
    if (adding_fslash)
        new_name[i++] = '/';

    /* Keep track of the position within the name (saves maths) */
    rel_i = 0;
    while (rel_i < name_len) {
        new_name[i++] = name[rel_i++];
    }

    /* Null terminator */
    new_name[i] = '\0';

    return new_name;
}
