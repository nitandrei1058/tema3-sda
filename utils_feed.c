#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"
#include "friends.h"
#include "posts.h"
#include "users.h"
#include "utils_posts.h"

int check_user_reposted(node_t *node, int user_id)
{
	for (int i = 0; i < node->sons_count; i++) {
		post_t *post = (post_t *)node->sons[i]->data;
		if (post->user_id == user_id)
			return 1;
		if (check_user_reposted(node->sons[i], user_id))
			return 1;
	}
	return 0;
}

void clique_bktk(int **friendship, uint16_t *friends, uint16_t level,
				 uint16_t friends_size, uint8_t *clique, uint8_t *max_clique,
				 int *max_clique_size, int *clique_size)
{
	if (*clique_size > *max_clique_size) {
		*max_clique_size = *clique_size;
		memcpy(max_clique, clique, MAX_PEOPLE * sizeof(uint8_t));
	}

	for (int i = level; i < friends_size; i++) {
		uint8_t good = 1;
		for (int j = 0; j < MAX_PEOPLE; j++) {
			if (clique[j] && !friendship[friends[i]][j]) {
				good = 0;
				break;
			}
		}
		if (good) {
			clique[friends[i]] = 1;
			(*clique_size)++;
			clique_bktk(friendship, friends, i + 1, friends_size, clique,
						max_clique, max_clique_size, clique_size);
			clique[friends[i]] = 0;
			(*clique_size)--;
		}
	}
}
