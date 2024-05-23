#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posts.h"
#include "users.h"
#include "utils_posts.h"

void add_post(char *name, char *title, tree_t *posts, node_t *parent_post)
{
	uint16_t user_id = get_user_id(name);
	posts->post_count++;
	post_t *post = init_post(posts->post_count, title, user_id);
	add_child(parent_post, init_node(post));
}

void create_post(char *name, char *title, tree_t *posts)
{
	add_post(name, title, posts, posts->root);
}

node_t *search_repost(node_t *node, int repost_id)
{
	if (((post_t *)node->data)->id == repost_id)
		return node;
	for (int i = 0; i < node->sons_count; i++) {
		node_t *repost = search_repost(node->sons[i], repost_id);
		if (repost)
			return repost;
	}
	return NULL;
}

node_t *search_post(tree_t *posts, int post_id)
{
	node_t *node = posts->root;
	for (int i = 0; i < node->sons_count; i++) {
		if (((post_t *)node->sons[i]->data)->id == post_id)
			return node->sons[i];
	}
	return NULL;
}

void repost(char *name, int id, int repost_id, tree_t *posts_tree)
{
	// Caut postarea
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	// Caut repostarea
	if (repost_id) {
		post_node = search_repost(post_node, repost_id);
	}
	// creez repostarea
	add_post(name, NULL, posts_tree, post_node);
	printf("Created repost #%d for %s\n", posts_tree->post_count, name);
}

void dfs_reposts(node_t *node)
{
	printf("Repost #%d by %s\n", ((post_t *)node->data)->id,
		   get_user_name(((post_t *)node->data)->user_id));
	for (int i = 0; i < node->sons_count; i++) {
		dfs_reposts(node->sons[i]);
	}
}

void get_reposts(int id, int repost_id, tree_t *posts_tree)
{
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	if (!repost_id) {
		printf("%s - Post by %s\n", ((post_t *)post_node->data)->title,
			   get_user_name(((post_t *)post_node->data)->user_id));
		for (int i = 0; i < post_node->sons_count; i++) {
			dfs_reposts(post_node->sons[i]);
		}
	} else {
		post_node = search_repost(post_node, repost_id);
		dfs_reposts(post_node);
	}
}

node_t *find_lca(node_t *node, int id_1, int id_2)
{
	if (((post_t *)node->data)->id == id_1 ||
		((post_t *)node->data)->id == id_2)
		return node;

	int match = 0;
	node_t *lca = NULL;

	for (int i = 0; i < node->sons_count; i++) {
		node_t *temp = find_lca(node->sons[i], id_1, id_2);
		if (temp != NULL) {
			lca = temp;
			match++;
		}
	}

	if (match == 2)
		return node;
	else
		return lca;
}

void common_repost(int id, int id_1, int id_2, tree_t *posts_tree)
{
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	node_t *lca = find_lca(post_node, id_1, id_2);
	printf("The first common repost of %d and %d is %d\n", id_1, id_2,
		   ((post_t *)lca->data)->id);
}

void handle_input_posts(char *input, tree_t *posts)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "create")) {
		char *name = strtok(NULL, " ");
		char *title = strtok(NULL, "\n");
		create_post(name, title, posts);
		printf("Created %s for %s\n", title, name);
	} else if (!strcmp(cmd, "repost")) {
		char *name = strtok(NULL, " ");
		int id = atoi(strtok(NULL, "\n "));
		char *repost_char_id = strtok(NULL, "\n ");
		int repost_id = 0;
		if (repost_char_id)
			repost_id = atoi(repost_char_id);
		repost(name, id, repost_id, posts);
	} else if (!strcmp(cmd, "common-repost")) {
		int post_id = atoi(strtok(NULL, " "));
		int id_1 = atoi(strtok(NULL, " "));
		int id_2 = atoi(strtok(NULL, "\n "));
		common_repost(post_id, id_1, id_2, posts);
	} else if (!strcmp(cmd, "like")) {
		(void)cmd;
		// TODO: Add function
	} else if (!strcmp(cmd, "ratio"))
		(void)cmd;
	// TODO: Add function
	else if (!strcmp(cmd, "delete"))
		(void)cmd;
	// TODO: Add function
	else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
	// TODO: Add function
	else if (!strcmp(cmd, "get-reposts")) {
		int id = atoi(strtok(NULL, "\n "));
		char *repost_char_id = strtok(NULL, "\n ");
		int repost_id = 0;
		if (repost_char_id)
			repost_id = atoi(repost_char_id);
		get_reposts(id, repost_id, posts);
	} else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
	// TODO: Add function

	free(commands);
}
