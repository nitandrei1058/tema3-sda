#ifndef UTILS_POSTS_H
#define UTILS_POSTS_H
#include <stdint.h>

#define TITLE_SIZE 300

typedef struct node_t node_t;
typedef struct tree_t tree_t;
typedef struct post_t post_t;
struct node_t {
	void *data;
	node_t **sons;
	node_t *parent;
	int sons_count;
	int sons_capacity;
};

struct tree_t {
	node_t *root;
	int post_count;
};

struct post_t {
	char *title;
	int user_id, id;
	tree_t *events;
	uint8_t *likes;
	int likes_count;
};

tree_t *init_tree(post_t *root);
tree_t *init_posts(void);
node_t *init_node(post_t *post);
post_t *init_post(int id, char *title, int user_id, int is_repost);
void add_child(node_t *parent, node_t *son);
void free_post(post_t *post);
void free_node(node_t *node);
void free_tree(tree_t *tree);
void free_posts(tree_t *posts);
node_t *search_post(tree_t *posts, int post_id);
node_t *search_repost(node_t *node, int repost_id);
node_t *find_lca(node_t *node, int id_1, int id_2);
int search_repost_index(node_t *node, int repost_id);
int search_post_index(tree_t *posts, int post_id);

#endif // UTILS_POSTS_H
