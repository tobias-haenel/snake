#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

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
    int running;
    int game_won;
};

void
read_inputs(struct game_state_t *state);

void
update_state(struct game_state_t *state);

void
render_state(struct game_state_t *state);

struct game_state_t *
init_game_state();

void
deinit_game_state(struct game_state_t *state);

struct snake_body_part_t *
init_snake(int fieldWidth, int fieldHeight);

void
move_snake(struct game_state_t *state);

void
deinit_snake(struct snake_body_part_t *snake);

struct food_t *
init_food();

void
spawn_food(struct game_state_t *state);

void
deinit_food(struct food_t *food);

int
positions_equal(struct position_t *p1, struct position_t *p2);

void
read_inputs(struct game_state_t *state) {
    // see wgetch()
    (void) state;
}

void
update_state(struct game_state_t *state) {
    move_snake(state);
}

void
render_state(struct game_state_t *state) {
    // see box(), mvwaddch(), wrefresh(), has_colors()
    // TODO render box around field
    // TODO render snake
    // TODO render food
    // TODO show window content
}

struct game_state_t *
init_game_state() {
    struct game_state_t *state = malloc(sizeof(*state));
    state->fieldWidth = 32;
    state->fieldHeight = 16;
    state->snake = init_snake(state->fieldWidth, state->fieldHeight);
    state->food = init_food();
    state->direction = LEFT;
    state->running = 1;
    state->game_won = 0;
    spawn_food(state);

    // see initscr(), create_newwin(), (no)echo(), keypad(), cbreak(), nodelay(), has_colors(), start_color(), init_pair(), curs_set(), (no)nl(), intrflush()
    // TODO initialise ncurses
    // TODO set ncurses options for interactive usage
    // TODO create a window for the field
    return state;
}

void
deinit_game_state(struct game_state_t *state) {
    deinit_snake(state->snake);
    deinit_food(state->food);

    // delwin(), endwin()
    // TODO delete the window for the field
    // TODO deinitialise ncurses
    free(state);
}

struct snake_body_part_t *
init_snake(int fieldWidth, int fieldHeight) {
    struct snake_body_part_t *current_part;
    struct snake_body_part_t *previous_part = NULL;
    struct snake_body_part_t *first_part;
    int snake_length = 4;
    assert(snake_length <= fieldWidth);
    for (int i = 0; i < 4; ++i) {
        current_part = malloc(sizeof(*current_part));
        current_part->next = NULL;
        current_part->position.x = (fieldWidth - snake_length) / 2 + i;
        current_part->position.y = fieldHeight / 2;
        if (previous_part != NULL) {
            previous_part->next = current_part;
        } else {
            first_part = current_part;
        }

        previous_part = current_part;
    }

    return first_part;
}

void
move_snake(struct game_state_t *state) {
    // calculate the new position for the head of the snake
    struct position_t new_head_pos;
    new_head_pos = state->snake->position;
    switch (state->direction) {
        case LEFT : new_head_pos.x -= 1; break;
        case UP : new_head_pos.y += 1; break;
        case RIGHT : new_head_pos.x += 1; break;
        case DOWN : new_head_pos.y -= 1; break;
    }
    if (new_head_pos.x >= state->fieldWidth) new_head_pos.x = 0;
    if (new_head_pos.x < 0) new_head_pos.x = state->fieldWidth - 1;
    if (new_head_pos.y >= state->fieldHeight) new_head_pos.y = 0;
    if (new_head_pos.y < 0) new_head_pos.y = state->fieldHeight - 1;

    // check if the game is finished, if the snake moves
    struct snake_body_part_t *part = state->snake;
    int snake_length = 0;
    while (part) {
        if (positions_equal(&new_head_pos, &(part->position))) {
            state->running = 0;
        }
        part = part->next;
        ++snake_length;
    }

    // decide if THE game was won or lost, if it is finished
    if (!state->running) {
        if (snake_length == state->fieldWidth * state->fieldHeight) {
            state->game_won = 1;
        }

        return;
    }

    // check if the snake consumes the food
    if (positions_equal(&new_head_pos, &(state->snake->position))) {
        // move the snakes head to the new (food) position, keep the tail
        struct snake_body_part_t *new_part = malloc(sizeof(*new_part));
        new_part->position = new_head_pos;
        new_part->next = state->snake;
        state->snake = new_part;
        // spawn new food
        spawn_food(state);
    } else {
        // move the snakes head to the new position and the last body part from its tail
        struct snake_body_part_t *current_part = state->snake;
        struct snake_body_part_t *previous_part = NULL;
        while (current_part->next) {
            previous_part = current_part;
            current_part = current_part->next;
        }
        current_part->position = new_head_pos;
        current_part->next = state->snake;
        if (previous_part) {
            previous_part->next = NULL;
        }
        state->snake = current_part;
    }
}

void
deinit_snake(struct snake_body_part_t *snake) {
    struct snake_body_part_t *current_part = snake;
    struct snake_body_part_t *next_part;
    while (current_part) {
        next_part = current_part->next;
        free(current_part);
        current_part = next_part;
    }
}

struct food_t *
init_food() {
    return malloc(sizeof(struct food_t));
}

void
spawn_food(struct game_state_t *state) {
    // generate a field buffer that indicates if a field is blocked by a snake body part
    int fields = state->fieldWidth * state->fieldHeight;
    int *field_buffer = calloc(sizeof(*field_buffer), fields);
    struct snake_body_part_t *part = state->snake;
    int snake_length = 0;
    while (part) {
        int field_index = part->position.y * state->fieldWidth + part->position.x;
        field_buffer[field_index] = 1;
        part = part->next;
        ++snake_length;
    }

    // generate a random index for the food position (fields with snake body parts aren't indexed)
    int random_food_idx = rand() % (fields - snake_length); // not evenly distributed
    int current_food_idx = 0;
    int searching = 1;
    for (int x = 0; x < state->fieldWidth && searching; ++x) {
        for (int y = 0; y < state->fieldHeight && searching; ++y) {
            int current_field_index = y * state->fieldWidth + x;
            int is_snake_body_part = field_buffer[current_field_index];
            if (!is_snake_body_part) {
                if (current_food_idx == random_food_idx) {
                    searching = 0;
                    state->food->position.x = x;
                    state->food->position.y = y;
                }
                ++current_food_idx;
            }
        }
    }
    free(field_buffer);
}

void
deinit_food(struct food_t *food) {
    free(food);
}

int
positions_equal(struct position_t *p1, struct position_t *p2) {
    return p1->x == p2->x && p1->y == p2->y;
}

double get_time() {
    return clock() * 1.0 / CLOCKS_PER_SEC;
}

int main() {
    // init
    srand(time(0));

    struct game_state_t *state = init_game_state();

    double time_per_update = 1;
    double previous_time = get_time();
    double update_lag = 0;
    while (state->running) {
        double current_time = get_time();
        double elapsed_time = current_time - previous_time;
        previous_time = current_time;
        update_lag += elapsed_time;

        read_inputs(state);
        while (update_lag >= time_per_update) {
            update_state(state);
            update_lag -= time_per_update;
        }
        render_state(state);
    }
    // deinit
    deinit_game_state(state);
    return 0;
}
