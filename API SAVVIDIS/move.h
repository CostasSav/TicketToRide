#ifndef T2R_TEST_MOVE_H
#define T2R_TEST_MOVE_H

#include "TicketToRideAPI.h"

/* ask for a move */
void askMove(t_move* move);

/* plays the move given as a parameter (send to the server)
 * returns the return code */
t_return_code playOurMove(t_move* move, t_color* lastCard);

/* tell if we need to replay */
int needReplay(t_move* move, t_color lastCard);

#endif //T2R_TEST_MOVE_H