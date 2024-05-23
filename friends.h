#ifndef FRIENDS_H
#define FRIENDS_H

#include "users.h"

#define MAX_COMMAND_LEN 500
#define MAX_PEOPLE 550

void add_friendship(char *user1, char *user2, int **friendship);
void remove_friendship(char *user1, char *user2, int **friendship);
void suggest_friends(char *user, int **friendship);
void find_distance(char *user1, char *user2, int **friendship);
void common_friends(char *user1, char *user2, int **friendship);
void count_friends(char *user, int **friendship);
void most_popular(char *user, int **friendship);

/**
 * Function that handles the calling of every command from task 1
 *
 * Please add any necessary parameters to the functions
 */
void handle_input_friends(char *input, int **friendship);

#endif // FRIENDS_H/**
