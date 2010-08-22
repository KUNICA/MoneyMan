/* Global header of moneyman
 * Copyright (C) 2009 Anton Pirogov
 * Distributed under the General Public License version 3
 */

#ifndef _MAIN_H
#define	_MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <allegro.h>

/* Struct for items */
struct obj_struct {
	int x,
	y,
	hidden;
};

/* Important global vars */
extern int players; /* No of players (1 or 2) */
extern int coinnumber; /* Number of coins in game */
extern int relocinterval; /* Interval between coin jumps */
extern int ttime; /* Time in time mode */
extern char highscores[10][2][16];

extern FONT* myfont;
extern int fontpxsize;
extern int menucol;
extern int menuselcol;
extern int titlecol;
extern int infocol;
extern int backcolor;

/* Timers globally visible */
extern volatile long speed_counter;
extern volatile long coinmove_counter;
extern volatile int close_button_pressed; /* Close with X */

#ifdef	__cplusplus
}
#endif

#endif	/* _MAIN_H */

