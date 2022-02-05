#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// local header files
#include "task_1.h"

// MACROS
#define KEY_FILE "data/keywords"

struct FILES *read_filenames(char *dir_path)
{
	DIR *d = NULL;
	struct dirent *dir = NULL;
	d = opendir(dir_path);

	//couting the filenames
	//yes I know I could've used realloc but I'm short of time
	//and this thing is faster than python anyways
	struct FILES *files = calloc(1, sizeof(struct FILES));
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			//skipping the '.' and '..' directories
			if (strcmp(dir->d_name, ".") == 0)
				continue;
			if (strcmp(dir->d_name, "..") == 0)
				continue;
			(*files).n++;
		}
		closedir(d);
	}
	d = opendir(dir_path);

	//allocating space
	(*files).str = (char **)calloc((*files).n, sizeof(char *));
	for (int i = 0; i < (*files).n; i++)
		(*files).str[i] = (char *)calloc(BUFSIZ, sizeof(char));

	//reading the filenames
	if (d) {
		int i = 0;
		while ((dir = readdir(d)) != NULL) {
			if (strcmp(dir->d_name, ".") == 0)
				continue;
			if (strcmp(dir->d_name, "..") == 0)
				continue;
			strcpy((*files).str[i], dir->d_name);
			i++;
		}
		closedir(d);
	}
	return files;
}

struct keywords *read_keywords(int files_nr)
{
	FILE *input = fopen("data/keywords", "r");

	if (!input) {
		printf("Something went wrong\n");
		exit(-1);
	}

	int n = 0;
	//aux used to read the number of lines inside the key
	char *aux = calloc(4, sizeof(char));
	fgets(aux, 4 * sizeof(char), input);
	n = atoi(aux);
	free(aux);
	struct keywords *key_arr = calloc(1, sizeof(struct keywords));
	key_arr->n = n;
	//allocating space
	key_arr->keys = calloc(n, sizeof(struct key));
	//reading the keys
	for (int i = 0; i < n; i++) {
		key_arr->keys[i].key = calloc(BUFSIZ, sizeof(char));
		key_arr->keys[i].occurences = calloc(files_nr, sizeof(int));
		fgets(key_arr->keys[i].key, BUFSIZ * sizeof(char), input);
		//making them lowercase
		for (int j = 0; key_arr->keys[i].key[j]; j++)
			key_arr->keys[i].key[j] = tolower(key_arr->keys[i].key[j]);
	}
	//removing the '\n' from the end
	for (int i = 0; i < n - 1; i++)
		key_arr->keys[i].key[strlen(key_arr->keys[i].key) - 1] = '\0';
	fclose(input);
	return key_arr;
}

int count_file(char *key, char *file)
{
	//the next block creates the path to the file
	char *path = calloc(BUFSIZ, sizeof(char));
	if (!path) {
		printf("Something went wrong\n");
		exit(-1);
	}
	strcpy(path, "./data/emails/");
	strcat(path, file);
	path[strlen(path)] = '\0';
	//end of path block

	FILE *input = fopen(path, "r");

	if (!input) {
		printf("Something went wrong\n");
		exit(-1);
	}

	char *line = calloc(BUFSIZ, sizeof(char));
	if (!line) {
		printf("Something went wrong\n");
		exit(-1);
	}

	int occ = 0, markup = 0;
	while (fgets(line, BUFSIZ * sizeof(char), input)) {
		//looking for the body
		if (strstr(line, "Body:"))
			markup = 1;
		if (markup == 1) {
			//making it all lowercase
			for (int i = 0; line[i]; i++)
				line[i] = tolower(line[i]);
			//counting the occurences of the key
			char *tmp = strstr(line, key);
			while (tmp) {
				occ++;
				tmp += strlen(key);
				tmp = strstr(tmp, key);
			}
		}
	}
	fclose(input);
	free(path);
	free(line);
	return occ;
}

void count(struct FILES *files, struct keywords *key_arr)
{
	//for each file and each key we put the ammount of times
	//we encountered it in the occurences array
	for (int i = 0; i < files->n; i++)
		for (int j = 0; j < key_arr->n; j++)
			key_arr->keys[j].occurences[i] = count_file(key_arr->keys[j].key,
														files->str[i]);
}

void statistics(struct FILES *files, struct keywords *key_arr)
{
	FILE *output = fopen("statistics.out", "w");
	for (int i = 0; i < key_arr->n; i++) {
		key_arr->keys[i].total = 0;
		double ma = 0;

		//calculating the total occurences for each key
		for (int j = 0; j < files->n; j++) {
			key_arr->keys[i].total += key_arr->keys[i].occurences[j];
			ma += key_arr->keys[i].occurences[j];
		}

		//doing the formula thingy
		ma = ma / files->n;
		double spread = 0;

		for (int j = 0; j < files->n; j++) {
			spread += (key_arr->keys[i].occurences[j] - ma) *
					  (key_arr->keys[i].occurences[j] - ma);
		}
		//getting the final spread
		spread = spread / files->n;
		spread = sqrt(spread);
		//printing the data out
		fprintf(output, "%s %d %0.6f\n", key_arr->keys[i].key,
				key_arr->keys[i].total, spread);
	}
	fclose(output);
}

void free_all(struct FILES *files, struct keywords *key_arr)
{
	//freeing the keys
	for (int i = 0; i < key_arr->n; i++) {
		free(key_arr->keys[i].key);
		free(key_arr->keys[i].occurences);
	}
	free(key_arr->keys);
	free(key_arr);
	//freeing the files
	for (int i = 0; i < (*files).n; i++)
		free((*files).str[i]);
	free((*files).str);
	free(files);
}

void sort_files(struct FILES *files)
{
	//temporary integer array for filename sorting
	int *tmp = calloc(files->n, sizeof(int));
	//converting filenames to integers
	for (int i = 0; i < files->n; i++)
		tmp[i] = atoi(files->str[i]);
	if (!tmp) {
		printf("Something went wrong\n");
		exit(-1);
	}
	//sorting
	for (int i = 0; i < files->n; i++) {
		for (int j = 0; j < files->n; j++) {
			if (tmp[i] < tmp[j]) {
				int aux = tmp[i];
				tmp[i] = tmp[j];
				tmp[j] = aux;
			}
		}
	}
	//converting integers back to filenames
	for (int i = 0; i < files->n; i++)
		snprintf(files->str[i], BUFSIZ * sizeof(char), "%d", tmp[i]);
	free(tmp);
}
