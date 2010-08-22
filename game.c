/* Main game routine
 * Copyright (C) 2009 Anton Pirogov
 * Distributed under the General Public License version 3
 */
#include "main.h"

void increment_coinmove_counter(); /* Required prototype to init the timer */
void add_score(int score);

/* collision detection of two bitmaps */
int PicCollision(BITMAP *pic1,int x1, int y1, BITMAP *pic2,int x2, int y2) {
    if(x1>=(x2+pic2->w) || y1>=(y2+pic2->h)
			|| x2>=(x1+pic1->w) || y2>=(y1+pic1->h))
		return 0;
    else
		return 1;
}

/* initialize objects with 0 */
void init_objs(struct obj_struct *objs, int number) {
	int i;for(i=0;i<number;i++) {
		objs[i].x = 0;
		objs[i].y = 0;
		objs[i].hidden = 0;
	}
}

/* Set items to random position */
void relocate_objs(struct obj_struct *objs,int number, int x_max, int y_max) {
	int i;for(i=0;i<number;i++) {
		objs[i].x = rand() % x_max;
		objs[i].y = rand() % y_max;
	}
}

/* Dirty (because freezing) ready-set-go countdown hack for a player wish */
void readysetgo() {
	BITMAP *screensave = create_bitmap(screen->w,screen->h);
	blit(screen,screensave,0,0,0,0,screen->w,screen->h);

	textprintf_centre_ex(screensave,myfont,screensave->w/2,screensave->h/2,makecol(255,0,0),-1,"3...");
	blit(screensave,screen,0,0,0,0,screensave->w,screensave->h);
	rest(500);
	textprintf_centre_ex(screensave,myfont,screensave->w/2,screensave->h/2+fontpxsize,makecol(255,0,0),-1,"2...");
	blit(screensave,screen,0,0,0,0,screensave->w,screensave->h);
	rest(500);
	textprintf_centre_ex(screensave,myfont,screensave->w/2,screensave->h/2+fontpxsize*2,makecol(255,0,0),-1,"1...");
	blit(screensave,screen,0,0,0,0,screensave->w,screensave->h);
	rest(500);

	destroy_bitmap(screensave);
}

/* Mode: 0 - Speed mode, 1 - time mode (other settings set globally) */
int game(int mode) {
		/* Vars */
	int i=0,j=0,k=0; /* Multi use variables */
	int x_max=64,y_max=64; /* Max coordinates */
	int step = 8; /* Moneyman pixel per step */
	int padding = 16; /* Field's distance from window edge */

	int starttime=0; /* Start time (saved there later) */
	int currtime=0;

	int p1collectedcoins = 0; /* Number of collected coins */
	int p2collectedcoins = 0;
	int score = 0; /* Score if P1 (in 2P mode no scores) */
	int p1wins=0;
	int p2wins=0;
	int finished = 0; /* Finished (0=none, 1 =p1, 2=p2) */

	/* Moneyman coordinates */
	int p1_x=x_max/2, p2_x=p1_x;
	int p1_y=y_max/2, p2_y=p1_y;


	struct obj_struct *objs=0; /* Dyn. array with coins + exit door */

	BITMAP *buffer=0,*p1pic=0,*p2pic=0,*coinpic=0,*exitpic=0; /* all bitmaps */

	/* Init movement timer */
	install_int_ex(increment_coinmove_counter, SECS_TO_TIMER(relocinterval));

	/* Load bitmaps */
	buffer = create_bitmap(800,600);
	p1pic = load_bitmap("gfx/MoneyMan.bmp",NULL);
	p2pic = load_bitmap("gfx/Player2.bmp",NULL);
	coinpic = load_bitmap("gfx/Coin.bmp",NULL);
	exitpic = load_bitmap("gfx/Exit.bmp",NULL);

	/* Random seed */
	srand(time(NULL));

	/* Init objects */
	objs = malloc(sizeof(*objs)*(coinnumber+1)); /* Allocate */
	if (objs==0) { /* Allocation failed */
		fprintf(stderr,"Memory couldn't be allocated, sorry :( free your RAM!\n");
		return 1;
	}
	init_objs(objs,coinnumber+1); /* Init with 0 */
	objs[0].hidden = 1; /* Door invisible */
	relocate_objs(objs,coinnumber+1,x_max,y_max); /* Random position */

	readysetgo();
	starttime = time(NULL); /* Here it's fair to start time counting */
	while (!key[KEY_ESC] && !close_button_pressed) { /* ESC quits game */
		clear_bitmap(buffer);
		rectfill(buffer,0,0,buffer->w,buffer->h,backcolor);

		while (speed_counter > 0) { /* logical tick event happened */
			/* Collision check */
			for (i=1; i<coinnumber+1; i++) {
			/* For coins - if visible, hide, increase counter */
				if (PicCollision(p1pic,p1_x*step,p1_y*step,
						coinpic,objs[i].x*step,objs[i].y*step)
						&& objs[i].hidden==0) {
					objs[i].hidden=1;
					p1collectedcoins++;
				}
				if (players==2)
					if (PicCollision(p2pic,p2_x*step,p2_y*step,
							coinpic,objs[i].x*step,objs[i].y*step)
							&& objs[i].hidden==0) {
						objs[i].hidden=1;
						p2collectedcoins++;
					}
			}

			/* All coins collected - show door (speed mode) */
			if (mode==0 && p1collectedcoins+p2collectedcoins == coinnumber) {
				objs[0].hidden=0;
			}

			currtime = time(NULL)-starttime;

			/* Calculate current score */
			if (players==1) {
				if (mode==0)
					score = get_score(p1collectedcoins,relocinterval,currtime);
				else if (mode==1)
					score = get_score(p1collectedcoins,relocinterval,ttime);
			}


			if ((mode==0 && PicCollision(p1pic,p1_x*step,p1_y*step,
							exitpic,objs[0].x*step,objs[0].y*step)
							&& objs[0].hidden==0) ||
							(mode==1 && currtime>=ttime)) {
				finished = 1;
			} else if (players == 2)
				if (mode==0 && PicCollision(p2pic,p2_x*step,p2_y*step,
							exitpic,objs[0].x*step,objs[0].y*step)
							&& objs[0].hidden==0)
					finished = 2;

			/* Movement of MoneyMan */
			if(key[KEY_LEFT] && p1_x > 0)
				p1_x--;
			else if(key[KEY_RIGHT] && p1_x < x_max)
				p1_x++;
			if(key[KEY_UP] && p1_y > 0)
				p1_y--;
			else if(key[KEY_DOWN] && p1_y < y_max)
				p1_y++;
			/* Movement of P2 */
			if(key[KEY_A] && p2_x > 0)
				p2_x--;
			else if(key[KEY_D] && p2_x < x_max)
				p2_x++;
			if(key[KEY_W] && p2_y > 0)
				p2_y--;
			else if(key[KEY_S] && p2_y < y_max)
				p2_y++;

			/*Reset tick counter*/
			speed_counter=0;
		}

		/* win count (must be here) */
		if (players==2 && mode==0) { /* More coins + reaches door -> win, else -> draw */
			if (finished==1 && p1collectedcoins>p2collectedcoins)
				p1wins++;
			else if (finished==2 && p2collectedcoins>p1collectedcoins)
				p2wins++;
		} else if (players==2 && mode==1 && finished) { /* more coins -> win, else -> draw */
			if (p1collectedcoins>p2collectedcoins)
				p1wins++;
			else if (p2collectedcoins>p1collectedcoins)
				p2wins++;
		}

		/* Move coins every some secs */
		if (coinmove_counter > 0) {
			relocate_objs(objs,coinnumber+1,x_max,y_max);
			if (mode==1) /* Time mode */
				for(i=1;i<coinnumber+1;i++)  /* Make visible again */
					objs[i].hidden=0;
			coinmove_counter = 0; /* reset interval tick */
		}

		/* Fill buffer */
		if (objs[0].hidden==0) /* index 0 = exit door */
			draw_sprite(buffer,exitpic,step*objs[0].x+padding,step*objs[0].y+padding);
		for(i=1;i<=coinnumber;i++) /* the coins */
			if (objs[i].hidden==0)
				draw_sprite(buffer,coinpic,step*objs[i].x+padding,step*objs[i].y+padding);

		/* Draw money man */
		draw_sprite(buffer,p1pic,step*p1_x+padding,step*p1_y+padding);
		if (players==2)
			draw_sprite(buffer,p2pic,step*p2_x+padding,step*p2_y+padding);

		/* Borders of game field */
		i=(x_max+p1pic->w/step)*step+padding;
		j=(y_max+p1pic->h/step)*step+padding;

		/* Draw borders */
		for (k=0;k<5;k++)
			rect(buffer,padding-k,padding-k,i+k,j+k,makecol(0,0,255-k*20));


		/* Print info */
		textprintf_ex(buffer,myfont,i+padding,32,titlecol,-1,"MoneyMan");
		textprintf_ex(buffer,myfont,i+padding,fontpxsize * 2,
			menuselcol,-1,
			(mode==0) ? "Time: %d" : "Time left: %d",
			(mode==0) ? currtime : ttime-currtime );
		textprintf_ex(buffer,myfont,i+padding,fontpxsize * 3,
				menuselcol,-1,
				(mode==0) ? "Coins left: %d" : "P1 Coins: %d" ,
				(mode==0) ? coinnumber-(p1collectedcoins+p2collectedcoins) : p1collectedcoins );
		if (players==2 && mode==1)
			textprintf_ex(buffer,myfont,i+padding,fontpxsize * 4,
				menuselcol,-1, "P2 Coins: %d" , p2collectedcoins );
		if (players==1)
			textprintf_ex(buffer,myfont,
						(x_max+p1pic->w/step)*step+2*padding,fontpxsize*5,
						menuselcol,-1,
						"Score: %d",score);
		/* Draw wins info */
		if (players==2) {
			textprintf_ex(buffer,myfont,
				(x_max+p1pic->w/step)*step+2*padding,fontpxsize*11,
				infocol,backcolor,
				"P1 wins: %d",p1wins);
			textprintf_ex(buffer,myfont,
				(x_max+p1pic->w/step)*step+2*padding,fontpxsize*12,
				infocol,backcolor,
				"P2 wins: %d",p2wins);
		}

		/* Apply on screen */
		blit(buffer,screen,0,0,0,0,800,600); /*Copy buffer to screen*/

		/* If win */
		if (finished) {
			if (mode==0)
				textprintf_ex(screen,myfont,
					(x_max+p1pic->w/step)*step+2*padding,fontpxsize*6,
					infocol,-1,
					"Finished!");
			else if (mode==1)
				textprintf_ex(screen,myfont,
					(x_max+p1pic->w/step)*step+2*padding,fontpxsize*6,
					infocol,-1,
					"Time is over!");
				if (players==1)
				add_if_highscore(score);
				textprintf_ex(screen,myfont,
				(x_max+p1pic->w/step)*step+2*padding,fontpxsize*8,
				infocol,-1,
				"ENTER=retry");
			textprintf_ex(screen,myfont,
				(x_max+p1pic->w/step)*step+2*padding,fontpxsize*9,
				infocol,-1,
				"ESC=quit");
			while(!(key[KEY_ENTER] || key[KEY_ESC]));
			if (key[KEY_ENTER]) {
				readysetgo();
				finished = 0;
				p1collectedcoins = 0;
				p2collectedcoins = 0;
				init_objs(objs,coinnumber+1);
				relocate_objs(objs,coinnumber+1,x_max,y_max);
				objs[0].hidden = 1; /* hide door again */
				remove_int(increment_coinmove_counter);
				install_int_ex(increment_coinmove_counter, SECS_TO_TIMER(relocinterval));
				starttime = time(NULL);
			}
		}
	}

	/* clean up on finish */
	remove_int(increment_coinmove_counter);
	destroy_bitmap(exitpic);
	destroy_bitmap(coinpic);
	destroy_bitmap(p1pic);
	destroy_bitmap(p2pic);
	destroy_bitmap(buffer);
	free(objs); objs=0;
	clear_bitmap(screen);
	rectfill(screen,0,0,screen->w,screen->h,backcolor);
	return 0;
}
