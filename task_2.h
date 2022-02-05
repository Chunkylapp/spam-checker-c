#ifndef TASK_2_H_
#define TASK_2_H

struct keywords *read_all_keywords(int files_nr);

//gets a files structure and returns the average lenght
//of each one's body
float get_avg_size(struct FILES *files);

//callculates the keyword score for the file
int keywords_score(int f_nr, struct keywords *key_arr);

//returns the number of chars inside the path email
int email_len_chars(char *path);

//returns the number of words inside the path email
int email_len_words(char *path);

//returns the number of capitalized characters inside the path email
int caps_count(char *path);

//returns 1 if more than half of the email is capitalized
//0 if not
int has_caps(char *path, int lenght_chars);

//returns the sender of the email inside the path
char *get_email(char *path);

//returns the spammer score inside the /data/spammers file
//for the given email or 0 if not found
int is_spammer(char *path);

#endif
