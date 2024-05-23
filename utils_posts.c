#include "utils_posts.h"
#include <stdlib.h>
#include <string.h>

post_t *init_post(int id, char *title, int user_id, int is_repost)
{
	post_t *post = (post_t *)calloc(1, sizeof(post_t));
	post->id = id;
	if (title) {
		post->title = malloc(TITLE_SIZE * sizeof(char));
		strcpy(post->title, title);
	} else {
		post->title = NULL;
	}
	post->user_id = user_id;
	if (!is_repost)
		post->events = init_tree(post);
	return post;
}

node_t *init_node(post_t *post)
{
	node_t *node = (node_t *)calloc(1, sizeof(node_t));
	node->data = post;
	return node;
}

tree_t *init_tree(post_t *root)
{
	tree_t *tree = (tree_t *)calloc(1, sizeof(tree_t));
	tree->root = init_node(root);
	return tree;
}

tree_t *init_posts(void)
{
	tree_t *posts = init_tree(NULL);
	return posts;
}

void extend_arr(void **arr, int count, int *capacity, int size)
{
	if (!*arr) {
		*arr = (void *)calloc(1, size);
		*capacity = 1;
	}
	if (count == *capacity) {
		*capacity *= 2;
		*arr = (void *)realloc(*arr, *capacity * size);
	}
}

void add_child(node_t *parent, node_t *son)
{
	extend_arr((void **)&parent->sons, parent->sons_count,
			   &parent->sons_capacity, sizeof(node_t *));
	parent->sons[parent->sons_count++] = son;
	son->parent = parent;
}

void free_post(post_t *post)
{
	if (!post)
		return;
	if (post->title)
		free(post->title);
	if (post->likes)
		free(post->likes);
	free(post);
}

void free_node(node_t *node)
{
	if (!node)
		return;
	for (int i = 0; i < node->sons_count; i++) {
		free_node(node->sons[i]);
	}
	if (node->sons)
		free(node->sons);
	free_post((post_t *)node->data);
	free(node);
}

void free_tree(tree_t *tree)
{
	if (!tree)
		return;
	free_node(tree->root);
	free(tree);
}

void free_posts(tree_t *posts)
{
	for (int i = 0; i < posts->root->sons_count; i++) {
		free_tree(((post_t *)(posts->root->sons[i]->data))->events);
		free(posts->root->sons[i]);
	}
	free(posts->root->sons);
	free(posts->root);
	free(posts);
}