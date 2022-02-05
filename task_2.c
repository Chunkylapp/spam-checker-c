#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// local header files
#include "task_1.h"
#include "task_2.h"

struct keywords *read_all_keywords(int files_nr)
{
	//opening both keywords files
	FILE *input_base = fopen("data/keywords", "r");
	FILE *input_ext = fopen("additional_keywords", "r");

	if (!input_base) {
		printf("Something went wrong\n");
		exit(-1);
	}
	if (!input_ext) {
		printf("Something went wrong\n");
		exit(-1);
	}
	struct keywords *key_arr = calloc(1, sizeof(struct keywords));
	int n = 0;

	//getting the 1st lenght
	fscanf(input_base, "%d", &n);
	key_arr->n = n;

	//getting the additional keywords lenght
	fscanf(input_ext, "%d", &n);
	key_arr->n += n;

	key_arr->keys = calloc(key_arr->n, sizeof(struct key));
	//reading the standard keywords
	for (int i = 0; i < key_arr->n - n; i++) {
		key_arr->keys[i].key = calloc(BUFSIZ, sizeof(char));
		key_arr->keys[i].occurences = calloc(files_nr, sizeof(int));
		fscanf(input_base, "%s", key_arr->keys[i].key);
	}
	fclose(input_base);
	//using the aux to move the reading cursor
	char aux[4];
	fgets(aux, 4 * sizeof(char), input_ext);
	//reading the additional keywords
	for (int i = key_arr->n - n; i < key_arr->n; i++) {
		key_arr->keys[i].key = calloc(BUFSIZ, sizeof(char));
		key_arr->keys[i].occurences = calloc(files_nr, sizeof(int));
		fgets(key_arr->keys[i].key, BUFSIZ * sizeof(char), input_ext);
	}
	fclose(input_ext);

	//removing the '\n' from the additional keywords
	for (int i = key_arr->n - n; i < key_arr->n - 1; i++)
		key_arr->keys[i].key[strlen(key_arr->keys[i].key) - 1] = '\0';
	return key_arr;
}

float get_avg_size(struct FILES *files)
{
	float size = 0;

	char *path = calloc(BUFSIZ, sizeof(char));
	if (!path) {
		printf("Something went wrong\n");
		exit(-1);
	}

	for (int i = 0; i < files->n; i++) {
		//creating the path for each email
		strcpy(path, "./data/emails/");
		strcat(path, files->str[i]);
		path[strlen(path)] = '\0';
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
		int markdown = 0;
		while (fgets(line, BUFSIZ * sizeof(char), input)) {
			//looking for the body
			if (strstr(line, "Body:"))
				markdown = 1;
			if (markdown == 1) {
				//counting each word
				char *tmp = strtok(line, "\n\t?.:,!");
				while (tmp) {
					size = size + 1;
					tmp = strtok(NULL, "\n\t?.:,!");
				}
			}
		}
		fclose(input);
		free(line);
	}
	free(path);
	//returning the average
	size = size / files->n;
	return size;
}

int keywords_score(int f_nr, struct keywords *key_arr)
{
	int score = 0;
	for (int i = 0; i < key_arr->n; i++)
		score += key_arr->keys[i].occurences[f_nr];
	return score;
}

int email_len_chars(char *path)
{
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

	int len = 0, markdown = 0;
	while (fgets(line, BUFSIZ * sizeof(char), input)) {
		//looking for the body
		if (strstr(line, "Body:"))
			markdown = 1;
		if (markdown == 1) {
			//counting the chars
			for (int i = 0; line[i]; i++)
				if (!strchr("\n\t", line[i]))
					len++;
		}
	}
	fclose(input);
	free(line);
	return len;
}

int email_len_words(char *path)
{
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

	int len = 0, markdown = 0;
	while (fgets(line, BUFSIZ * sizeof(char), input)) {
		//looking for the body
		if (strstr(line, "Body:"))
			markdown = 1;
		if (markdown == 1) {
			//counting the words
			char *p = strtok(line, " .?,!:\n\t");
			while (p) {
				len++;
				p = strtok(NULL, " .?,!:\n\t");
			}
		}
	}
	fclose(input);
	free(line);
	return len;
}

int caps_count(char *path)
{
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

	int caps = 0, markdown = 0;
	while (fgets(line, BUFSIZ * sizeof(char), input)) {
		//looking for the body
		if (strstr(line, "Body:"))
			markdown = 1;
		if (markdown == 1) {
			//counting the capitalized chars
			for (int i = 0; line[i]; i++) {
				if (line[i] >= 'A' && line[i] <= 'Z')
					caps++;
			}
		}
	}
	fclose(input);
	free(line);
	return caps;
}

int has_caps(char *path, int lenght_chars)
{
	if (caps_count(path) > (lenght_chars / 2))
		return 1;
	return 0;
}

char *get_email(char *path)
{
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
	char *email = calloc(BUFSIZ, sizeof(char));
	if (!email) {
		printf("Something went wrong\n");
		exit(-1);
	}
	char stuff[] = "\n\t()::\"<>~,;][";
	while (fgets(line, BUFSIZ * sizeof(char), input)) {
		//looking for the sender line
		char *tmp = strstr(line, "From:");
		if (tmp) {
			tmp = strtok(tmp, stuff);
			//getting the sender
			while (tmp) {
				if (strchr(tmp, '@'))
					strcpy(email, tmp);
				tmp = strtok(NULL, stuff);
			}
		}
	}
	free(line);
	fclose(input);
	return email;
}

int is_spammer(char *path)
{
	FILE *input = fopen("./data/spammers", "r");
	if (!input) {
		printf("Something went wrong\n");
		exit(-1);
	}

	char *line = calloc(BUFSIZ, sizeof(char));
	if (!line) {
		printf("Something went wrong\n");
		exit(-1);
	}
	int val = 0;
	char *email = get_email(path);
	while (fgets(line, BUFSIZ * sizeof(char), input)) {
		//checking to see if the current line holds the email we received
		if (strstr(line, email)) {
			char *tmp = strstr(line, " ");
			tmp += 1;
			val = atoi(tmp);
		}
	}
	fclose(input);
	free(email);
	free(line);
	return val;
}
