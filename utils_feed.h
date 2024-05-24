#ifndef UTILS_FEED_H
#include "utils_posts.h"

int check_user_reposted(node_t *node, int user_id);
void clique_bktk(int **friendship, uint16_t *friends, uint16_t level,
				 uint16_t friends_size, uint8_t *clique, uint8_t *max_clique,
				 int *max_clique_size, int *clique_size);

#define UTILS_FEED_H
#endif // UTILS_FEED_H
