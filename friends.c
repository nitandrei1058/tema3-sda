#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "friends.h"
#include "users.h"

void add_friendship(char *user1, char *user2, int **friendship)
{
	uint16_t user1_id = get_user_id(user1);
	uint16_t user2_id = get_user_id(user2);

	friendship[user1_id][user2_id] = true;
	friendship[user2_id][user1_id] = true;
	printf("Added connection %s - %s\n", user1, user2);
}

void remove_friendship(char *user1, char *user2, int **friendship)
{
	uint16_t user1_id = get_user_id(user1);
	uint16_t user2_id = get_user_id(user2);

	friendship[user1_id][user2_id] = false;
	friendship[user2_id][user1_id] = false;
	printf("Removed connection %s - %s\n", user1, user2);
}

void suggest_friends(char *user, int **friendship)
{
	uint16_t user_id = get_user_id(user);
	if (user_id == (uint16_t)-1) {
		printf("User not found.\n");
		return;
	}

	bool suggestions_found = false;
	printf("Suggestions for %s:\n", user);
	for (uint16_t i = 0; i < MAX_PEOPLE; ++i) {
		if (i == user_id || friendship[user_id][i]) {
			continue;
		}

		for (uint16_t j = 0; j < MAX_PEOPLE; ++j) {
			if (friendship[user_id][j] && friendship[j][i]) {
				printf("%s\n", get_user_name(i));
				suggestions_found = true;
				break;
			}
		}
	}

	if (!suggestions_found) {
		printf("There are no suggestions for %s\n", user);
	}
}

void find_distance(char *user1, char *user2, int **friendship)
{
	uint16_t user1_id = get_user_id(user1);
	uint16_t user2_id = get_user_id(user2);
	if (user1_id == (uint16_t)-1 || user2_id == (uint16_t)-1) {
		printf("User not found.\n");
		return;
	}

	int distances[MAX_PEOPLE];
	bool visited[MAX_PEOPLE];
	for (uint16_t i = 0; i < MAX_PEOPLE; i++) {
		distances[i] = INT_MAX;
		visited[i] = false;
	}

	// BFS
	uint16_t queue[MAX_PEOPLE];
	int front = 0, rear = 0;

	distances[user1_id] = 0;
	visited[user1_id] = true;
	queue[rear++] = user1_id;

	while (front < rear) {
		uint16_t current = queue[front++];

		for (uint16_t i = 0; i < MAX_PEOPLE; i++) {
			if (friendship[current][i] && !visited[i]) {
				visited[i] = true;
				distances[i] = distances[current] + 1;
				queue[rear++] = i;

				if (i == user2_id) {
					printf("The distance between %s - %s is %d\n", user1, user2,
						   distances[i]);
					return;
				}
			}
		}
	}

	printf("There is no way to get from %s to %s\n", user1, user2);
}

void common_friends(char *user1, char *user2, int **friendship)
{
	uint16_t user1_id = get_user_id(user1);
	uint16_t user2_id = get_user_id(user2);

	if (user1_id == (uint16_t)-1 || user2_id == (uint16_t)-1) {
		printf("One or both users not found.\n");
		return;
	}

	bool common_found = false;

	for (uint16_t i = 0; i < MAX_PEOPLE; i++) {
		if (friendship[user1_id][i] && friendship[user2_id][i]) {
			common_found = true;
			break;
		}
	}

	if (!common_found) {
		printf("No common friends for %s and %s\n", user1, user2);
	} else {
		printf("The common friends between %s and %s are:\n", user1, user2);
		for (uint16_t i = 0; i < MAX_PEOPLE; i++) {
			if (friendship[user1_id][i] && friendship[user2_id][i]) {
				printf("%s\n", get_user_name(i));
			}
		}
	}
}

void count_friends(char *user, int **friendship)
{
	uint16_t user_id = get_user_id(user);

	if (user_id == (uint16_t)-1) {
		printf("User not found.\n");
		return;
	}

	int friend_count = 0;
	for (uint16_t i = 0; i < MAX_PEOPLE; i++) {
		if (friendship[user_id][i]) {
			friend_count++;
		}
	}

	printf("%s has %d friends\n", user, friend_count);
}

void most_popular(char *user, int **friendship)
{
	uint16_t user_id = get_user_id(user);

	if (user_id == (uint16_t)-1) {
		printf("User not found.\n");
		return;
	}

	int max_connections = -1;
	uint16_t most_popular_id = user_id;

	int own = 0;
	for (uint16_t i = 0; i < MAX_PEOPLE; i++) {
		if (friendship[user_id][i]) {
			int friend_connections = 0;
			own++;
			for (uint16_t j = 0; j < MAX_PEOPLE; j++) {
				if (friendship[i][j]) {
					friend_connections++;
				}
			}
			if (friend_connections > max_connections ||
				(friend_connections == max_connections &&
				 i < most_popular_id)) {
				max_connections = friend_connections;
				most_popular_id = i;
			}
		}
	}
	if (own >= max_connections) {
		max_connections = own;
		most_popular_id = user_id;
	}
	char *most_popular_name = get_user_name(most_popular_id);
	if (most_popular_id == user_id) {
		printf("%s is the most popular\n", most_popular_name);
	} else {
		printf("%s is the most popular friend of %s\n", most_popular_name,
			   user);
	}
}

void handle_input_friends(char *input, int **friendship)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "add")) {
		// TODO: Add function
		char *user1 = strtok(NULL, " ");
		char *user2 = strtok(NULL, "\n ");
		if (user1 && user2)
			add_friendship(user1, user2, friendship);
	} else if (!strcmp(cmd, "remove")) {
		// TODO: Add function
		char *user1 = strtok(NULL, " ");
		char *user2 = strtok(NULL, "\n ");
		if (user1 && user2)
			remove_friendship(user1, user2, friendship);
	} else if (!strcmp(cmd, "suggestions")) {
		// TODO: Add function
		char *user = strtok(NULL, "\n ");
		if (user) {
			suggest_friends(user, friendship);
		}
	} else if (!strcmp(cmd, "distance")) {
		// TODO: Add function
		char *user1 = strtok(NULL, " ");
		char *user2 = strtok(NULL, "\n ");
		if (user1 && user2) {
			find_distance(user1, user2, friendship);
		}
	} else if (!strcmp(cmd, "common")) {
		// TODO: Add function
		char *user1 = strtok(NULL, " ");
		char *user2 = strtok(NULL, "\n ");
		if (user1 && user2) {
			common_friends(user1, user2, friendship);
		}
	} else if (!strcmp(cmd, "friends")) {
		// TODO: Add function
		char *user = strtok(NULL, "\n ");
		if (user) {
			count_friends(user, friendship);
		}
	} else if (!strcmp(cmd, "popular")) {
		// TODO: Add function
		char *user = strtok(NULL, "\n ");
		if (user) {
			most_popular(user, friendship);
		}
	}

	free(commands);
}