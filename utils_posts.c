#include "utils_posts.h"
#include <stdlib.h>
#include <string.h>

post_t *init_post(int id, char *title, int user_id)
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
	extend_arr((void **)&parent->sons, parent->sons_count, &parent->sons_capacity,
			   sizeof(node_t *));
	parent->sons[parent->sons_count++] = son;
}