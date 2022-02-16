#ifndef T2R_TEST_GAME_H
#define T2R_TEST_GAME_H


#include "TicketToRideAPI.h"

/* CECI EST JUSTE UNE INDICATION */


typedef struct{
	int city1, city2;				/* id of the cities */
	int length;						/* length of the track */
	t_color color1, color2;			/* colors */
	int taken;						/* tell if taken */
} t_track;

typedef struct{
	char name[20];					/* name of the player */
	int nbWagons;					/* number of wagons */
	int nbCards;					/* number of cards */
	int cards[10];					/* number of cards of each color, ie cards[YELLOW] is the number of YELLOW cards */
	int nbObjectives;				/* number of objectives */
	t_objective objectives[20];		/* objectives, only objectives with index < nbObjectives are available */
} t_player;


typedef struct{
	char name[20];					/* name of the game */
	t_color faceUp[5];				/* face up cards */
	int player;						/* player who plays: 0 == US, 1 == OPPONNENT */
	t_player players[2];
} t_game;


#endif //T2R_TEST_GAME_H