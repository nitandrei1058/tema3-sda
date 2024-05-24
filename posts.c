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
	posts->post_count++; // incrementeaza contorul de postari
	post_t *post = init_post(posts->post_count, title, user_id, is_repost);
	// adauga postarea ca fiu al nodului parinte
	add_child(parent_post, init_node(post));
}

void create_post(char *name, char *title, tree_t *posts)
{
	// adauga postarea la radacina arborelui
	add_post(name, title, posts, posts->root, 0); 
	printf("Created %s for %s\n", title, name);
}

void repost(char *name, int id, int repost_id, tree_t *posts_tree)
{
	// cauta nodul postarii originale
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	// cauta repostarea
	if (repost_id)
		post_node = search_repost(post_node, repost_id);
	// creeaza repostarea
	add_post(name, NULL, posts_tree, post_node, 1);
	printf("Created repost #%d for %s\n", posts_tree->post_count, name);
}

void dfs_reposts(node_t *node)
{
	printf("Repost #%d by %s\n", ((post_t *)node->data)->id,
		   get_user_name(((post_t *)node->data)->user_id));
	for (int i = 0; i < node->sons_count; i++)
		// apeleaza recursiv pentru fiecare fiu
		dfs_reposts(node->sons[i]);
}

void get_reposts(int id, int repost_id, tree_t *posts_tree)
{
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	if (!repost_id) {
		printf("%s - Post by %s\n", ((post_t *)post_node->data)->title,
			   get_user_name(((post_t *)post_node->data)->user_id));
		for (int i = 0; i < post_node->sons_count; i++)
			// afiseaza recursiv reposturile
			dfs_reposts(post_node->sons[i]);
	} else {
		post_node = search_repost(post_node, repost_id);
		// afiseaza reposturile pentru repostul specificat
		dfs_reposts(post_node);
	}
}

node_t *find_lca(node_t *node, int id_1, int id_2)
{
	// cauta recursiv cel mai apropiat stramos comun
	if (((post_t *)node->data)->id == id_1 ||
		((post_t *)node->data)->id == id_2)
		return node;

	int match = 0;
	node_t *lca = NULL;

	for (int i = 0; i < node->sons_count; i++) {
		node_t *temp = find_lca(node->sons[i], id_1, id_2);
		if (temp) {
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
	// gaseste LCA al celor doua reposturi
	node_t *lca = find_lca(post_node, id_1, id_2);
	printf("The first common repost of %d and %d is %d\n", id_1, id_2,
		   ((post_t *)lca->data)->id);
}

void add_like(char *name, node_t *post_node)
{
	post_t *post = (post_t *)post_node->data;
	if (!post->likes)
		// aloca memorie pentru like-uri
		post->likes = (uint8_t *)calloc(MAX_PEOPLE, sizeof(uint8_t));
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
	// caita nodul postarii originale in arbore
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	char *title = ((post_t *)post_node->data)->title;
	// daca exist repost_id, cauta repostul specifical in arbore
	if (repost_id)
		post_node = search_repost(post_node, repost_id);
	add_like(name, post_node); // adauga sau elimina like-ul

	if (repost_id)
		printf("re");
	printf("post %s\n", title);
}

void get_likes(int id, int repost_id, tree_t *posts_tree)
{
	// cauta nodul postarii originale in arbore
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	// daca exista repost_id, cauta repostul specificat
	if (repost_id)
		post_node = search_repost(post_node, repost_id);
	post_t *post = (post_t *)post_node->data;
	if (repost_id)
		printf("Repost #%d", repost_id);
	else
		printf("Post %s", post->title);
	printf(" has %d likes\n", post->likes_count);
}

void ratio_reposts(node_t *node, int *max, int *repost_id)
{
	// actualizeaza max si repost_id daca gaseste un repost cu mai multe likes
	if (*max < ((post_t *)node->data)->likes_count ||
		(*max == ((post_t *)node->data)->likes_count &&
		 *repost_id > ((post_t *)node->data)->id)) {
		*max = ((post_t *)node->data)->likes_count;
		*repost_id = ((post_t *)node->data)->id;
	}
	for (int i = 0; i < node->sons_count; i++)
		ratio_reposts(node->sons[i], max, repost_id); // parcurge recursiv
}

void ratio(int id, tree_t *posts_tree)
{
	// cauta nodul postarii originale in arbore
	node_t *post_node =
		((post_t *)(search_post(posts_tree, id)->data))->events->root;
	int max = ((post_t *)(post_node->data))->likes_count;
	int repost_id = -1;

	// calculeaza repostul cu cele mai multe like-uri
	ratio_reposts(post_node, &max, &repost_id);
	if (max == ((post_t *)post_node->data)->likes_count)
		printf("The original post is the highest rated\n");
	else
		printf("Post %d got ratio'd by repost %d\n", id, repost_id);
}

int search_repost_index(node_t *node, int repost_id)
{
	// parcurge lista de fii si ai nodului curent
	for (int i = 0; i < node->sons_count; i++) {
		if (((post_t *)node->sons[i]->data)->id == repost_id)
			return i;

		// cauta recursiv in subarborii nodului curent
		int index = search_repost_index(node->sons[i], repost_id);
		if (index != -1)
			return index;
	}
	return -1;
}

int search_post_index(tree_t *posts, int post_id)
{
	node_t *node = posts->root;

	// parcurge lista de fii ai radacinii arborelui
	for (int i = 0; i < node->sons_count; i++) {
		if (((post_t *)node->sons[i]->data)->id == post_id)
			return i;
	}
	return -1;
}

void delete_post(int id, int repost_id, tree_t *posts_tree)
{
	if (repost_id) {
		// cauta nodul postarii originale in arbore
		node_t *post_node =
			((post_t *)(search_post(posts_tree, id)->data))->events->root;
		char *title = ((post_t *)post_node->data)->title;

		// cauta si sterge repostul specificat
		int post_node_index = search_repost_index(post_node, repost_id);
		node_t *repost_node = post_node->sons[post_node_index];
		for (int i = post_node_index; i < post_node->sons_count - 1; i++)
			post_node->sons[i] = post_node->sons[i + 1];
		post_node->sons_count--;
		// elibereaza memoria repostului
		free_node(repost_node);
		printf("Deleted repost #%d of post %s\n", repost_id, title);
	} else {
		// cauta si sterhe postarea specificata
		int post_node_index = search_post_index(posts_tree, id);
		node_t *post_node = posts_tree->root->sons[post_node_index];
		char *title = ((post_t *)post_node->data)->title;
		
		// elibereaza memoria pentru postare si toate reposturile asociate
		for (int i = post_node_index; i < posts_tree->root->sons_count - 1;
			 i++) {
			posts_tree->root->sons[i] = posts_tree->root->sons[i + 1];
		}
		posts_tree->root->sons_count--;
		// elibereaza memoria subarborelui
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
