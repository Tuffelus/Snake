#define _DEFAULT_SOURCE
#include "snake.h"
#include "util.h"

int food_x, food_y;

int
main()
{
	srand(time(NULL));

	char *filename = "snake.conf";
	init_tui();

	int score = 0;
	int highscore = 0;
	char *highscore_str = "highscore";
	if ((highscore = get_token_value(filename, highscore_str)) == FAILURE) {
		add_token(filename, highscore_str, 0);
		highscore = 0;
	}
	struct snake *head = malloc(sizeof(struct snake));
	enum move_dir direction = UNDEFINED;
	bool has_eaten = false;

	new_game(head, &score);

	int ch;

	while ((ch = getch()) != 'q') {
		bool end_game = false;
		erase();
		print_border();

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

		enum collision_type coll = move_snake(head, direction, has_eaten);
		switch (coll) {
		case COLL_FOOD:
			has_eaten = true;
			break;
		case COLL_SNAKE:
			end_game = true;
			break;
		default:
			has_eaten = false;
			break;
		}

		if (has_eaten) {
			reposition_food();
			score++;
		}
		else {
			attron(COLOR_PAIR(1));
			mvprintw(food_y, food_x, "  ");
			attroff(COLOR_PAIR(1));
		}

		mvprintw(2, SIZE_X + 3, "Score: %d", score);
		mvprintw(3, SIZE_X + 3, "(Highscore: %d)", highscore);

		print_snake(head);
		refresh();

		usleep(100000);

		if (end_game) {
			if (score > highscore) {
				highscore = score;
				if (write_token_value(filename, highscore_str, highscore) == FAILURE) {
					fprintf(stderr, "ERROR: Failed writing to file\n");
					add_token(filename, highscore_str, highscore);
				}
			}

			char *msg = "You lost...";
			mvprintw(SIZE_Y, SIZE_X / 2 - strlen(msg) / 2, "%s", msg);
			msg = "Press any key to reset the snake";
			mvprintw(SIZE_Y + 1, SIZE_X / 2 - strlen(msg) / 2, "%s", msg);

			direction = UNDEFINED;

			// pauses game until a key is pressed
			nodelay(stdscr, 0);
			if (getch() != -1) {
				new_game(head, &score);
			}
			nodelay(stdscr, 1);
		}
	}

	free_snake(head);
	endwin();
	return 0;
}

void
new_game(struct snake *head, int *score)
{
	free_snake(head);
	*score = 0;
	reposition_food();

	head->pos_x = (SIZE_X / 2) + (SIZE_X % 2);
	head->pos_y = (SIZE_Y / 2) + (SIZE_Y % 2);
}

void
reposition_food(void)
{
	food_x = rand() % (SIZE_X - 4) + 1;
	food_y = rand() % (SIZE_Y - 4) + 1;

	food_x += food_x % 2;
}

enum collision_type
move_snake(struct snake *head, enum move_dir direction, bool append)
{
	if (direction == UNDEFINED) {
		return COLL_NONE;
	}

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
			head->pos_x = SIZE_X - 4;
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
	head->next = NULL;
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

void
print_border(void)
{
	for (int i = 1; i < SIZE_X - 1; i++) {
		mvaddch(0, i, '-');
		mvaddch(SIZE_Y - 1, i, '-');
	}

	for (int i = 1; i < SIZE_Y - 1; i++) {
		mvaddch(i, 1, '|');
		mvaddch(i, SIZE_X - 2, '|');
	}
	mvaddch(0, 1, '#');
	mvaddch(0, SIZE_X - 2, '#');
	mvaddch(SIZE_Y - 1, SIZE_X - 2, '#');
	mvaddch(SIZE_Y - 1, 1, '#');
}

void
init_tui(void)
{
	initscr();
	start_color();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	nodelay(stdscr, TRUE);
	curs_set(0);
	init_pair(1, COLOR_BLACK, COLOR_GREEN);
	init_pair(2, COLOR_BLACK, COLOR_RED);
}
