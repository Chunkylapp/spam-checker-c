#ifndef TASK_1_H_
#define TASK_1_H

#include <stdio.h>
#include <stdlib.h>

//structure that holds all the filenames
struct FILES {
	char **str;
	int n;
};

//structure that holds the data for each key
struct key {
	char *key;
	int *occurences;
	int total;
};

//structure that holds all the keys
struct keywords {
	struct key *keys;
	int n;
};

//function gets the path to a directory, and returns a FILES structure
//holding all the filenames inside the given directory
struct FILES *read_filenames(char *dir_path);

//function receives the number of files inside the directory and
//reads the keywords from the data/keywords file, allocates space
//for the occurences array
struct keywords *read_keywords(int files_nr);

//function does what strstr() does
//gets the haystack string, and creturns the number of
//times we found the needle
int get_substr_count(const char *haystack, const char *needle);

//returns the number of occurences of the key string, inside the file
int count_file(char *key, char *file);

//counts all key occurences inside all the files
void count(struct FILES *files, struct keywords *key_arr);

//calculates the statistics and prints them
void statistics(struct FILES *files, struct keywords *key_arr);

//clears all the memory
void free_all(struct FILES *files, struct keywords *key_arr);

//sorts the files
void sort_files(struct FILES *files);

#endif
