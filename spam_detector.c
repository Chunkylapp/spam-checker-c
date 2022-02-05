#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// local header files
#include "task_1.h"
#include "task_2.h"

int main(void)
{
	struct FILES *files = read_filenames("./data/emails");
	sort_files(files);

	struct keywords *key_arr = read_keywords(files->n);

	// task 1
	count(files, key_arr);
	statistics(files, key_arr);
	free_all(files, key_arr);

	// task 2
	files = read_filenames("./data/emails");
	sort_files(files);
	key_arr = read_all_keywords(files->n);

	//getting the average size of an email
	float avg_size = get_avg_size(files);

	FILE *output = fopen("prediction.out", "w");

	//counting the occurences of each key inside the emails
	count(files, key_arr);
	//iterating through each email to calculate its score
	for (int i = 0; i < files->n; i++) {
		//the next block creates the path to the email inside the
		//path string
		char *path = calloc(BUFSIZ, sizeof(char));
		if (!path) {
			printf("Something went wrong\n");
			exit(-1);
		}
		strcpy(path, "./data/emails/");
		strcat(path, files->str[i]);
		path[strlen(path)] = '\0';
		//end of path block

		//the next block calculates the score for the current email
		int keywords_count = keywords_score(i, key_arr);

		int lenght_chars = email_len_chars(path);
		int lenght_words = email_len_words(path);

		//formula for the keywords result
		int keywords_res = (int)(keywords_count *
							((avg_size * 1.0) / (lenght_words * 1.0)));
		int caps_bool = has_caps(path, lenght_chars);

		int spammer = is_spammer(path);
		//final formula
		int score = 20 * keywords_res + 30 * caps_bool + spammer;

		//output
		if (score > 100)
			fprintf(output, "%d\n", 1);
		else
			fprintf(output, "%d\n", 0);
		free(path);
	}
	//clearing the mess we left behind
	fclose(output);
	free_all(files, key_arr);
	return 0;
}
