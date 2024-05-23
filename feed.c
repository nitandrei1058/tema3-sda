#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"
#include "friends.h"
#include "posts.h"
#include "users.h"
#include "utils_posts.h"

void feed(char *name, int feed_size, int **friendship, tree_t *posts_tree)
{
	int user_id = get_user_id(name);
	node_t **post_nodes = posts_tree->root->sons;

	for (int i = posts_tree->root->sons_count - 1; i >= 0 && feed_size; i--) {
		post_t *post = (post_t *)post_nodes[i]->data;
		if (post->user_id == user_id || friendship[user_id][post->user_id]) {
			printf("%s: %s\n", get_user_name(post->user_id), post->title);
			feed_size--;
		}
	}
}

void handle_input_feed(char *input, int **friendship, tree_t *posts)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "feed")) {
		char *name = strtok(NULL, "\n ");
		int feed_size = atoi(strtok(NULL, "\n "));
		feed(name, feed_size, friendship, posts);
	} else if (!strcmp(cmd, "view-profile"))
		(void)cmd;
	// TODO: Add function
	else if (!strcmp(cmd, "friends-repost"))
		(void)cmd;
	// TODO: Add function
	else if (!strcmp(cmd, "common-groups"))
		(void)cmd;
	// TODO: Add function

	free(commands);
}
