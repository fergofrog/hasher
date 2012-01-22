/**
 * @file global.h
 * Header for global.c
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
#ifndef FILE_H_
#define FILE_H_

/** File list struct */
struct file_list_t {
    char dyn_alloc;
    char *file;
    ino_t ino;
    struct file_list_t *next;
};

ino_t is_file(char *);
ino_t is_dir(char *);
ino_t is_filedir(char *);

unsigned int expand_files(unsigned int, struct file_list_t **);

char file_listed(ino_t, unsigned int, struct file_list_t *);
void free_file_list(unsigned int, struct file_list_t *);


#endif /* FILE_H_ */
