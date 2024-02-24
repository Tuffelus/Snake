#define _DEFAULT_SOURCE

#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "snake.h"

int food_x, food_y;

int
main()
{
	srand(time(NULL));

	initscr();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	raw();
	keypad(stdscr, TRUE);
	noecho();
	nodelay(stdscr, TRUE);
	curs_set(0);
	init_pair(1, COLOR_BLACK, COLOR_GREEN);
	init_pair(2, COLOR_BLACK, COLOR_RED);

	reposition_food();

	struct snake *head = malloc(sizeof(struct snake));
	head->pos_x = (SIZE_X / 2) + (SIZE_X % 2);
	head->pos_y = (SIZE_Y / 2) + (SIZE_Y % 2);

	enum move_dir direction = UNDEFINED;
	bool has_eaten = false;

	int ch;

	while ((ch = getch()) != 'q') {
		erase();
		for (int i = 0; i < SIZE_X; i++) {
			mvaddch(0, i, '-');
			mvaddch(SIZE_Y - 1, i, '-');
		}

		for (int i = 0; i < SIZE_Y; i++) {
			mvaddch(i, 0, '|');
			mvaddch(i, SIZE_X - 1, '|');
		}
		mvaddch(0, 0, '#');
		mvaddch(0, SIZE_X - 1, '#');
		mvaddch(SIZE_Y - 1, SIZE_X - 1, '#');
		mvaddch(SIZE_Y - 1, 0, '#');

		if (ch == 'w' && direction != DOWN) {
			direction = UP;
		}
		if (ch == 's' && direction != UP) {
			direction = DOWN;
		}
		if (ch == 'd' && direction != LEFT) {
			direction = RIGHT;
		}
		if (ch == 'a' && direction != RIGHT) {
			direction = LEFT;
		}

		int coll = move_snake(head, direction, has_eaten);
		if (coll == COLL_FOOD) {
			has_eaten = true;
		}
		else if (coll == COLL_SNAKE) {
			printf("You lost...\n");
			break;
		}
		else {
			has_eaten = false;
		}

		if (has_eaten) {
			reposition_food();
		}
		else {
			attron(COLOR_PAIR(1));
			mvprintw(food_y, food_x, "  ");
			attroff(COLOR_PAIR(1));
		}

		print_snake(head);
		refresh();

		usleep(100000);
	}

	free_snake(head);
	endwin();
	return 0;
}

void
reposition_food(void)
{
	food_x = rand() % (SIZE_X - 3) + 1;
	food_y = rand() % (SIZE_Y - 3) + 1;

	food_x += food_x % 2;
}

enum collision_type
move_snake(struct snake *head, enum move_dir direction, bool append)
{
	if (direction == UNDEFINED)
		return append;

	struct snake *curser = head;
	while (curser->next != NULL) {
		curser = curser->next;
	}

	struct snake *new_tail = malloc(sizeof(struct snake));
	if (new_tail == NULL) {
		fprintf(stderr, "ERROR: Memory allocation failed!\n");
	}

	if (append) {
		curser->next = new_tail;
		new_tail->prev = curser;
		new_tail->pos_x = curser->pos_x;
		new_tail->pos_y = curser->pos_y;
	}
	else {
		free(new_tail);
	}
	append = false;

	while (curser->prev != NULL) {
		curser->pos_x = curser->prev->pos_x;
		curser->pos_y = curser->prev->pos_y;
		curser = curser->prev;
	}

	switch (direction) {
	case RIGHT:
		if ((head->pos_x += 2) >= SIZE_X - 2) {
			head->pos_x = 2;
		}
		break;
	case LEFT:
		if ((head->pos_x -= 2) <= 1) {
			head->pos_x = SIZE_X - 2;
		}
		break;
	case UP:
		if (head->pos_y-- <= 1) {
			head->pos_y = SIZE_Y - 2;
		}
		break;
	case DOWN:
		if (head->pos_y++ >= SIZE_Y - 2) {
			head->pos_y = 1;
		}
		break;
	default:
		fprintf(stderr, "ERROR: Wrong direction!\n");
		exit(EXIT_FAILURE);
	}

	return collision_detection(head);
}

enum collision_type
collision_detection(struct snake *head)
{
	if (head->pos_x == food_x && head->pos_y == food_y) {
		return COLL_FOOD;
	}
	for (struct snake *curser = head->next; curser != NULL; curser = curser->next) {
		if (curser->pos_x == head->pos_x && curser->pos_y == head->pos_y) {
			return COLL_SNAKE;
		}
	}

	return COLL_NONE;
}

void
free_snake(struct snake *head)
{
	if (head == NULL) {
		return;
	}
	free_snake(head->next);
	free(head);
}

void
print_snake(struct snake *head)
{
	attron(COLOR_PAIR(2));
	mvprintw(head->pos_y, head->pos_x, "  ");

	for (struct snake *curser = head->next; curser != NULL; curser = curser->next) {
		mvprintw(curser->pos_y, curser->pos_x, "  ");
	}
	attroff(COLOR_PAIR(2));
}
