#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"
#include "friends.h"
#include "posts.h"
#include "users.h"
#include "utils_feed.h"
#include "utils_posts.h"

void feed(char *name, int feed_size, int **friendship, tree_t *posts_tree)
{
	// obtine ID-ul utilizatorului
	int user_id = get_user_id(name);
	// obtine nodurile postarilor
	node_t **post_nodes = posts_tree->root->sons;

	// parcurge postarile in ordine inversa pentru a afisa cele mai recente
	// postari
	for (int i = posts_tree->root->sons_count - 1; i >= 0 && feed_size; i--) {
		post_t *post = (post_t *)post_nodes[i]->data;
		// verifica daca postarea apartine utilizatorului sau prietenilor lui
		if (post->user_id == user_id || friendship[user_id][post->user_id]) {
			printf("%s: %s\n", get_user_name(post->user_id), post->title);
			// decrementeaza dimensiunea feed-ului
			feed_size--;
		}
	}
}

void view_profile(char *name, tree_t *posts_tree)
{
	// obtine ID-ul utilizatorului
	int user_id = get_user_id(name);
	// obtine nodurile postarilor
	node_t **post_nodes = posts_tree->root->sons;

	// afiseaza toate postarile utilizatorului
	for (int i = 0; i < posts_tree->root->sons_count; i++) {
		post_t *post = (post_t *)post_nodes[i]->data;
		if (post->user_id == user_id)
			printf("Posted: %s\n", post->title);
	}

	// afiseaza toate repostarile utilizatorului
	for (int i = 0; i < posts_tree->root->sons_count; i++) {
		node_t *post_node = ((post_t *)post_nodes[i]->data)->events->root;
		if (check_user_reposted(post_node, user_id))
			printf("Reposted: %s\n", ((post_t *)post_node->data)->title);
	}
}

void friends_repost(char *name, int id, int **friendship, tree_t *posts_tree)
{
	// obtine ID-ul utilizatorului
	int user_id = get_user_id(name);
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;

	// parcurge repostarile si afiseaza prietenii care au dat repost
	for (int i = 0; i < post_node->sons_count; i++) {
		post_t *post = (post_t *)post_node->sons[i]->data;
		if (friendship[user_id][post->user_id])
			printf("%s\n", get_user_name(post->user_id));
	}
}

void common_group(char *name, int **friendship)
{
	// obtine ID-ul utilizatorului
	int user_id = get_user_id(name);

	// aloca memorie pentru gestionarea clicii si a prietenilor
	uint8_t *clique = calloc(MAX_PEOPLE, sizeof(uint8_t));
	uint8_t *max_clique = calloc(MAX_PEOPLE, sizeof(uint8_t));
	int max_clique_size = 1;
	int clique_size = 1;
	clique[user_id] = 1;
	max_clique[user_id] = 1;

	// pentru a optimiza, retin toti prietenii celui care sigur e in grup
	uint16_t *friends = calloc(MAX_PEOPLE, sizeof(uint16_t));
	uint16_t friends_size = 0;
	for (int i = 0; i < MAX_PEOPLE; i++)
		if (friendship[user_id][i])
			friends[friends_size++] = i;

	// gaseste cea mai mare clica de prieteni folosind backtracking
	clique_bktk(friendship, friends, 0, friends_size, clique, max_clique,
				&max_clique_size, &clique_size);

	printf("The closest friend group of %s is:\n", name);
	for (int i = 0; i < MAX_PEOPLE; i++) {
		if (max_clique[i])
			printf("%s\n", get_user_name(i));
	}
	// eliberez memoria
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
