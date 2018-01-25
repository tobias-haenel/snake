#include <stdlib.h>

struct position_t {
    int x;
    int y;
};

struct snake_body_part_t {
    struct position_t position;
    struct snake_body_part_t *next;
};

enum direction_t {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

struct food_t {
    struct position_t position;
    int value;
};

struct game_state_t {
    int fieldWidth;
    int fieldHeight;
    struct snake_body_part_t *snake;
    struct food_t *food;
    enum direction_t direction;
};

struct game_state_t *
init_game_state();

void
deinit_game_state(struct game_state_t *game_state);

struct snake_body_part_t *
init_snake(int fieldWidth, int fieldHeight);

void
move_snake(struct game_state_t *game_state);

void
deinit_snake(struct snake_body_part_t *snake);

struct food_t *
init_food();

void
spawn_food(struct game_state_t *game_state);

void
deinit_food(struct food_t *food);

void
read_inputs(struct game_state_t *game_state);

void
update_state(struct game_state_t *game_state);

void
render_state(struct game_state_t *game_state);

int main() {
    // init
    struct game_state_t *game_state = init_game_state();
    for(;;) {
        read_inputs(game_state);
        update_state(game_state);
        render_state(game_state);
    }
    // deinit
    deinit_game_state(game_state);
    return 0;
}
