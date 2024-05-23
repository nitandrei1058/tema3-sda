#include "utils_posts.h"
#include <stdlib.h>
#include <string.h>

post_t *init_post(int id, char *title, int user_id, int is_repost)
{
	post_t *post = (post_t *)calloc(1, sizeof(post_t));
	post->id = id;
	if (title) {
		post->title = malloc(TITLE_SIZE * sizeof(char));
		strcpy(post->title, title); // copiaza titlul postarii
	} else {
		post->title = NULL;
	}
	post->user_id = user_id;
	if (!is_repost)
		// initializeaza arborele de eveniment
		post->events = init_tree(post);
	return post;
}

node_t *init_node(post_t *post)
{
	node_t *node = (node_t *)calloc(1, sizeof(node_t));
	node->data = post; // asociaza postarea cu nodul
	return node;
}

tree_t *init_tree(post_t *root)
{
	tree_t *tree = (tree_t *)calloc(1, sizeof(tree_t));
	tree->root = init_node(root); // seteaza radacina arborelui
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
		// realoca memoria pentru array
		*arr = (void *)realloc(*arr, *capacity * size);
	}
}

void add_child(node_t *parent, node_t *son)
{
	extend_arr((void **)&parent->sons, parent->sons_count,
			   &parent->sons_capacity, sizeof(node_t *));
	parent->sons[parent->sons_count++] = son; // adauga fiul in lista
	son->parent = parent; // seteaza parintele fiului
}

void free_post(post_t *post)
{
	if (!post)
		return;
	if (post->title)
		free(post->title); // elibereaza memoria pentru titlu
	if (post->likes)
		free(post->likes); // elibereaza memoria pentru like-uri
	free(post); // elibereaza structura postarii
}

void free_node(node_t *node)
{
	if (!node)
		return;
	for (int i = 0; i < node->sons_count; i++)
		free_node(node->sons[i]); // elibereaza recursiv memoria pentru fii
	if (node->sons)
		free(node->sons); // elibereaza memoria pentru lista de fii
	free_post((post_t *)node->data);
	free(node); // elibereaza structura nodului
}

void free_tree(tree_t *tree)
{
	if (!tree)
		return;
	free_node(tree->root); // elibereaza radacina arborelui
	free(tree); // elibereaza structura arborelui
}

void free_posts(tree_t *posts)
{
	// functia elibereaza memoria pentru fiecare sub-arbore, nod, lista de fii
	// radacina si structura arborelui de radacini
	for (int i = 0; i < posts->root->sons_count; i++) {
		free_tree(((post_t *)(posts->root->sons[i]->data))->events);
		free(posts->root->sons[i]);
	}
	free(posts->root->sons);
	free(posts->root);
	free(posts);
}

node_t *search_repost(node_t *node, int repost_id)
{
	// functia cauta un repost in arbore
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
	// functia cauta o postare in arbore
	node_t *node = posts->root;
	for (int i = 0; i < node->sons_count; i++) {
		if (((post_t *)node->sons[i]->data)->id == post_id)
			return node->sons[i];
	}
	return NULL;
}
