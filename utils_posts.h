#ifndef UTILS_POSTS_H
#define UTILS_POSTS_H

#define TITLE_SIZE 300

typedef struct node_t node_t;
typedef struct tree_t tree_t;
typedef struct post_t post_t;
struct node_t {
    void *data;
    node_t **sons;
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
};

tree_t *init_tree(post_t *root);
tree_t *init_posts(void);
node_t *init_node(post_t *post);
post_t *init_post(int id, char *title, int user_id);
void add_child(node_t *parent, node_t *son);

#endif // UTILS_POSTS_H