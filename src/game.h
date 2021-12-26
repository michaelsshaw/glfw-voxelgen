#ifndef PO_GAME_H_
#define PO_GAME_H_


int
game_begin(struct state *state, GLFWwindow *window);

int
game_state(struct state *state, enum gamestate i_gamestate);

#endif // PO_GAME_H_
