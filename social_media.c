/**
 * The entrypoint of the homework. Every initialization must be done here
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "users.h"
#include "friends.h"
#include "posts.h"
#include "feed.h"

#include "utils_posts.h"

/**
 * Initializez every task based on which task we are running
*/
void init_tasks(void)
{
	#ifdef TASK_1

	#endif

	#ifdef TASK_2

	#endif

	#ifdef TASK_3

	#endif
}

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	init_users();

	init_tasks();
	#ifdef TASK_1
    int **friendship = calloc(MAX_PEOPLE, sizeof(int *));
    for (int i = 0; i < MAX_PEOPLE; i++) {
        friendship[i] = (int *)calloc(MAX_PEOPLE, sizeof(int));
    }
	#endif

	#ifdef TASK_2
	tree_t *posts = init_posts();
	#endif


	char *input = (char *)malloc(MAX_COMMAND_LEN);
	while (1) {
		char *command = fgets(input, MAX_COMMAND_LEN, stdin);

		// If fgets returns null, we reached EOF
		if (!command)
			break;

		#ifdef TASK_1
        handle_input_friends(input, friendship);
		#endif

		#ifdef TASK_2
		handle_input_posts(input, posts);
		#endif

		#ifdef TASK_3
		handle_input_feed(input);
		#endif
	}

	free_users();
	free(input);

	#ifdef TASK_1
    for (int i = 0; i < MAX_PEOPLE; i++)
        free(friendship[i]);
    free(friendship);
	#endif

	return 0;
}
