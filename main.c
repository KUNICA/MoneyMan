/* APCMoneyMan - Anton Pirogov's MoneyMan (C Allegro 4.2 implementation)
 * Copyright (C) 2009 Anton Pirogov
 * Distributed under the General Public License version 3
*/
/* TODO:
 * (Anti aliasing fonts (alfont))
 */

#include "main.h"

/* Prototype */
int game(int mode);

/* Global vars regarding appearance */
FONT *myfont;
int fontpxsize=40;

/* Global vars regarding color */
int	titlecol;
int	infocol;
int	menucol;
int	menuselcol;
int	backcolor;
int	scoretitcol;
int	scorenamcol;
int	scorescrcol;

/* Global vars regarding game */
int players=1;
int coinnumber = 10; /* Number of coins in game */
int relocinterval = 2; /* Interval between coin jumps */
int ttime = 60; /* Time in time mode */
char highscores[10][2][16]; /* Place for scores */


/* my two timers */
volatile long speed_counter = 0; /* The redraw tick */
void increment_speed_counter() {  speed_counter++; }
END_OF_FUNCTION(increment_speed_counter)
volatile long coinmove_counter = 0; /* The interval between coin movement */
void increment_coinmove_counter() {  coinmove_counter++; }
END_OF_FUNCTION(increment_coinmove_counter)

volatile int close_button_pressed = FALSE; /* Closing with X */
void close_button_handler() { close_button_pressed = TRUE; }
END_OF_FUNCTION(close_button_handler)


void renderscores() {
	BITMAP *brand = load_bitmap("gfx/apbrand.bmp",NULL);
	FONT *smallfont = load_font("gfx/Komikaboogie14px.tga",NULL,NULL);
	int i=0;

	rectfill(screen,0,0,screen->w,screen->h,backcolor);

	textprintf_centre_ex(screen,myfont,screen->w/2,6*fontpxsize,scoretitcol,-1,"Highscores");
	for(i=0;i<10;i++) {
		textprintf_ex(screen,smallfont,screen->w/2-200,7*fontpxsize+25*i,scorenamcol,-1,
			highscores[i][0]);
			textprintf_ex(screen,smallfont,screen->w/2+150,7*fontpxsize+25*i,scorescrcol,-1,
			highscores[i][1]);
	}

	draw_sprite(screen,brand,screen->w-brand->w,screen->h-brand->h);

	destroy_bitmap(brand);
}

int settings_event(int currindex, int key, char *title) {
	if (key==0 && currindex==3)
		return 1;

	switch (currindex) {
		case 0: /* A speed mode */
			if (coinnumber<100 && key==1 || coinnumber>1 && key==-1)
				coinnumber += key; /* in/decrease (left or right = -1 or 1) */
			sprintf(title,"Coin number: %d",coinnumber);
			break;
		case 1: /* A time mode */
			if (relocinterval<30 && key==1 || relocinterval>1 && key==-1)
				relocinterval += key;
			sprintf(title,"Jump interval: %d sec",relocinterval);
			break;
		case 2: /* Options */
			if (ttime<180 && key==1 || ttime>10 && key==-1)
				ttime += key;
			sprintf(title,"Time (Time mode): %d sec",ttime);
			break;
	}

	return 0;
}

/* callback function - currindex = menu item, key = pressed key (0=enter,-1=left,1=right) */
int mainmenu_event(int currindex, int key, char *title) {
	int i;
	char **settingstext;

	/* Allocate space for menu texts and write them there */
	settingstext = malloc(4*sizeof(*settingstext));
	for(i=0;i<4;i++)
		settingstext[i] = malloc(64*sizeof(**settingstext));

	sprintf(settingstext[0],"Coin number: %d",coinnumber);
	sprintf(settingstext[1],"Jump interval: %d sec",relocinterval);
	sprintf(settingstext[2],"Time (Time mode): %d sec",ttime);
	sprintf(settingstext[3],"Back");

	if (key==0)
		switch (currindex) {
			case 0: /* speed mode */
				game(0);
				break;
			case 1: /* time mode */
				game(1);
				break;
			case 3: /* Settings */
				clear_bitmap(screen);
				rectfill(screen,0,0,screen->w,screen->h,backcolor);

				generic_menu(settingstext,4,settings_event,backcolor,menucol,menuselcol,fontpxsize,0,1,NULL);
				break;
			case 4: return 1; /* Set quit flag */
				break;
	}
	if (currindex==2) {
		players = (players==1) ? 2 : 1 ;
		sprintf(title,"Players: %d",players);
	}

	for (i=0;i<4;i++)
		free(settingstext[i]);
	free(settingstext);
	return 0;
}


int main(int argc, char *argv[]) {
	int i;
	char **menutext;

	/* Allocate space for menu texts and write them there */

	menutext = malloc(5*sizeof(*menutext));
	for(i=0;i<5;i++)
		menutext[i] = malloc(64*sizeof(**menutext));

	sprintf(menutext[0],"Play speed mode");
	sprintf(menutext[1],"Play time mode");
	sprintf(menutext[2],"Players: 1");
	sprintf(menutext[3],"Settings");
	sprintf(menutext[4],"Quit");

	/* Init allegro and stuff I need */
	allegro_init();
	install_keyboard();
	/* install_mouse(); */

	/* Init gfx mode */
	set_color_depth(24);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800,600,0,0);
	set_window_title("Anton Pirogov's MoneyMan v1.0");

	/* Init logic tick timer */
	install_timer();
	LOCK_VARIABLE(speed_counter);
	LOCK_FUNCTION(increment_speed_counter);
	install_int_ex(increment_speed_counter, BPS_TO_TIMER(60));
	/* Lock coin movement timer */
	LOCK_VARIABLE(coinmove_counter);
	LOCK_FUNCTION(increment_coinmove_counter);

	set_close_button_callback( close_button_handler ); /* Close with X */

	/* Set colors */
	titlecol=makecol(255,192,0);
	infocol=makecol(255,128,192);
	menucol=makecol(0,0,255);
	menuselcol=makecol(171,171,255);
	backcolor=makecol(245, 245, 220); /* beige */
	scoretitcol=makecol(0,128,128);
	scorenamcol=makecol(128,64,255);
	scorescrcol=makecol(255,128,255);

	/* Load font */
	myfont = load_font("gfx/Komikaboogie18px.tga",NULL,NULL);

	/* Show main menu */
	read_scores();
	generic_menu(menutext,5,mainmenu_event,backcolor,menucol,menuselcol,fontpxsize,0,0,renderscores);

	/* Finish clean up */
	for (i=0;i<4;i++)
		free(menutext[i]);
	free(menutext);
	allegro_exit();
	return 0;
}
END_OF_MAIN()
