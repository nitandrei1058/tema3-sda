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

int check_user_reposted(node_t *node, int user_id)
{
	for (int i = 0; i < node->sons_count; i++) {
		post_t *post = (post_t *)node->sons[i]->data;
		if (post->user_id == user_id)
			return 1;
		if (check_user_reposted(node->sons[i], user_id))
			return 1;
	}
	return 0;
}

void view_profile(char *name, tree_t *posts_tree)
{
	int user_id = get_user_id(name);
	node_t **post_nodes = posts_tree->root->sons;

	for (int i = 0; i < posts_tree->root->sons_count; i++) {
		post_t *post = (post_t *)post_nodes[i]->data;
		if (post->user_id == user_id)
			printf("Posted: %s\n", post->title);
	}

	for (int i = 0; i < posts_tree->root->sons_count; i++) {
		node_t *post_node = ((post_t *)post_nodes[i]->data)->events->root;
		if (check_user_reposted(post_node, user_id))
			printf("Reposted: %s\n", ((post_t *)post_node->data)->title);
	}
}

void friends_repost(char *name, int id, int **friendship, tree_t *posts_tree)
{
	int user_id = get_user_id(name);
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	for (int i = 0; i < post_node->sons_count; i++) {
		post_t *post = (post_t *)post_node->sons[i]->data;
		if (friendship[user_id][post->user_id])
			printf("%s\n", get_user_name(post->user_id));
	}
}

void clique_bktk(int **friendship, uint16_t *friends, uint16_t level,
				 uint16_t friends_size, uint8_t *clique, uint8_t *max_clique,
				 int *max_clique_size, int *clique_size)
{
	if (*clique_size > *max_clique_size) {
		*max_clique_size = *clique_size;
		memcpy(max_clique, clique, MAX_PEOPLE * sizeof(uint8_t));
	}

	for (int i = level; i < friends_size; i++) {
		uint8_t good = 1;
		for (int j = 0; j < MAX_PEOPLE; j++) {
			if (clique[j] && !friendship[friends[i]][j]) {
				good = 0;
				break;
			}
		}
		if (good) {
			clique[friends[i]] = 1;
			(*clique_size)++;
			clique_bktk(friendship, friends, i + 1, friends_size, clique,
						max_clique, max_clique_size, clique_size);
			clique[friends[i]] = 0;
			(*clique_size)--;
		}
	}
}

void common_group(char *name, int **friendship)
{
	int user_id = get_user_id(name);

	uint8_t *clique = calloc(MAX_PEOPLE, sizeof(uint8_t));
	uint8_t *max_clique = calloc(MAX_PEOPLE, sizeof(uint8_t));
	int max_clique_size = 1;
	int clique_size = 1;
	clique[user_id] = 1;
	max_clique[user_id] = 1;

	// trebuie sa fac un vector cu toti ratatii aia si sa iau size-ul gen

	uint16_t *friends = calloc(MAX_PEOPLE, sizeof(uint16_t));
	uint16_t friends_size = 0;
	for (int i = 0; i < MAX_PEOPLE; i++)
		if (friendship[user_id][i])
			friends[friends_size++] = i;

	clique_bktk(friendship, friends, 0, friends_size, clique, max_clique,
				&max_clique_size, &clique_size);

	printf("The closest friend group of %s is:\n", name);
	for (int i = 0; i < MAX_PEOPLE; i++) {
		if (max_clique[i])
			printf("%s\n", get_user_name(i));
	}
	free(clique);
	free(max_clique);
	free(friends);
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
	} else if (!strcmp(cmd, "view-profile")) {
		char *name = strtok(NULL, "\n ");
		view_profile(name, posts);
	} else if (!strcmp(cmd, "friends-repost")) {
		char *name = strtok(NULL, "\n ");
		int id = atoi(strtok(NULL, "\n "));
		friends_repost(name, id, friendship, posts);
	} else if (!strcmp(cmd, "common-group")) {
		char *name = strtok(NULL, "\n ");
		common_group(name, friendship);
	}

	free(commands);
}
