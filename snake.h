#define SIZE_X 40
#define SIZE_Y 20

enum move_dir {
	// UNDEFINED stops the snake from moving
	UNDEFINED = -1,
	RIGHT,
	LEFT,
	UP,
	DOWN
};

enum collision_type {
	COLL_NONE,
	COLL_FOOD,
	COLL_SNAKE
};

struct snake {
	int pos_x;
	int pos_y;
	struct snake *next;
	struct snake *prev;
};

enum collision_type move_snake(struct snake *head, enum move_dir direction, bool append);
void free_snake(struct snake *head);
void print_snake(struct snake *head);
void reposition_food(void);
enum collision_type collision_detection(struct snake *head);
void new_game();
