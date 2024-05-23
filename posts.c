#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "friends.h"
#include "posts.h"
#include "users.h"
#include "utils_posts.h"

void add_post(char *name, char *title, tree_t *posts, node_t *parent_post,
			  int is_repost)
{
	uint16_t user_id = get_user_id(name);
	posts->post_count++;
	post_t *post = init_post(posts->post_count, title, user_id, is_repost);
	add_child(parent_post, init_node(post));
}

void create_post(char *name, char *title, tree_t *posts)
{
	add_post(name, title, posts, posts->root, 0);
	printf("Created %s for %s\n", title, name);
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
	add_post(name, NULL, posts_tree, post_node, 1);
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

void add_like(char *name, node_t *post_node)
{
	post_t *post = (post_t *)post_node->data;
	if (!post->likes) {
		post->likes = (uint8_t *)calloc(MAX_PEOPLE, sizeof(uint8_t));
	}
	int user_id = get_user_id(name);
	if (post->likes[user_id]) {
		post->likes_count--;
		post->likes[user_id] = 0;
		printf("User %s unliked ", name);
	} else {
		post->likes_count++;
		post->likes[user_id] = 1;
		printf("User %s liked ", name);
	}
}

void like_post(char *name, int id, int repost_id, tree_t *posts_tree)
{
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	char *title = ((post_t *)post_node->data)->title;
	if (repost_id) {
		post_node = search_repost(post_node, repost_id);
	}
	add_like(name, post_node);

	if (repost_id)
		printf("re");
	printf("post %s\n", title);
}

void get_likes(int id, int repost_id, tree_t *posts_tree)
{
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	if (repost_id) {
		post_node = search_repost(post_node, repost_id);
	}
	post_t *post = (post_t *)post_node->data;
	if (repost_id) {
		printf("Repost #%d", repost_id);
	} else {
		printf("Post %s", post->title);
	}
	printf(" has %d likes\n", post->likes_count);
}

void ratio_reposts(node_t *node, int *max, int *repost_id)
{
	if (*max < ((post_t *)node->data)->likes_count ||
		(*max == ((post_t *)node->data)->likes_count &&
		 *repost_id > ((post_t *)node->data)->id)) {
		*max = ((post_t *)node->data)->likes_count;
		*repost_id = ((post_t *)node->data)->id;
	}
	for (int i = 0; i < node->sons_count; i++) {
		ratio_reposts(node->sons[i], max, repost_id);
	}
}

void ratio(int id, tree_t *posts_tree)
{
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	int max = ((post_t *)(post_node->data))->likes_count;
	int repost_id = -1;
	ratio_reposts(post_node, &max, &repost_id);

	if (max == ((post_t *)(post_node->data))->likes_count) {
		printf("The original post is the highest rated\n");
	} else {
		printf("Post %d got ratio'd by repost %d\n", id, repost_id);
	}
}

int search_repost_index(node_t *node, int repost_id)
{
	for (int i = 0; i < node->sons_count; i++) {
		if (((post_t *)node->sons[i]->data)->id == repost_id)
			return i;
		int index = search_repost_index(node->sons[i], repost_id);
		if (index != -1)
			return index;
	}
	return -1;
}

int search_post_index(tree_t *posts, int post_id)
{
	node_t *node = posts->root;
	for (int i = 0; i < node->sons_count; i++) {
		if (((post_t *)node->sons[i]->data)->id == post_id)
			return i;
	}
	return -1;
}

void delete_post(int id, int repost_id, tree_t *posts_tree)
{
	if (repost_id) {
		node_t *post_node =
			((post_t *)(search_post(posts_tree, id)->data))->events->root;
		char *title = ((post_t *)post_node->data)->title;
		int post_node_index = 0;
		for (int i = post_node_index; i < post_node->sons_count - 1; i++) {
			post_node->sons[i] = post_node->sons[i + 1];
		}
		post_node->sons_count--;
		free_node(post_node->sons[post_node->sons_count]);
		printf("Deleted repost #%d of post %s\n", repost_id, title);
	} else {
		int post_node_index = search_post_index(posts_tree, id);
		node_t *post_node = posts_tree->root->sons[post_node_index];
		char *title = ((post_t *)post_node->data)->title;
		for (int i = post_node_index; i < posts_tree->root->sons_count - 1; i++) {
			posts_tree->root->sons[i] = posts_tree->root->sons[i + 1];
		}
		posts_tree->root->sons_count--;
		free_tree(((post_t *)(post_node->data))->events);
		free(post_node);
		printf("Deleted %s\n", title);
	}
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
		char *name = strtok(NULL, " ");
		int id = atoi(strtok(NULL, "\n "));
		char *repost_char_id = strtok(NULL, "\n ");
		int repost_id = 0;
		if (repost_char_id)
			repost_id = atoi(repost_char_id);
		like_post(name, id, repost_id, posts);
	} else if (!strcmp(cmd, "ratio")) {
		int id = atoi(strtok(NULL, "\n "));
		ratio(id, posts);
	} else if (!strcmp(cmd, "delete")) {
		int id = atoi(strtok(NULL, "\n "));
		char *repost_char_id = strtok(NULL, "\n ");
		int repost_id = 0;
		if (repost_char_id)
			repost_id = atoi(repost_char_id);
		delete_post(id, repost_id, posts);
	} else if (!strcmp(cmd, "get-likes")) {
		int id = atoi(strtok(NULL, "\n "));
		char *repost_char_id = strtok(NULL, "\n ");
		int repost_id = 0;
		if (repost_char_id)
			repost_id = atoi(repost_char_id);
		get_likes(id, repost_id, posts);
	} else if (!strcmp(cmd, "get-reposts")) {
		int id = atoi(strtok(NULL, "\n "));
		char *repost_char_id = strtok(NULL, "\n ");
		int repost_id = 0;
		if (repost_char_id)
			repost_id = atoi(repost_char_id);
		get_reposts(id, repost_id, posts);
	}

	free(commands);
}
