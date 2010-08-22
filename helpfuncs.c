/* Useful functions to use with the Allegro library
 * Copyright (C) 2009 Anton Pirogov
 * Distributed under the General Public License version 3
 */
#include "main.h"

/* Makes kinda checksum of whole keyboard state to make possible
 * to detect changes between accesses */
int general_key_state() {
	int i, sum=0;
	for(i=0;i<255;i++)
		sum+=key[i]*i;
	return sum;
}

/* Generic allegro menu by Anton Pirogov
 * menutext: Array of strings of the menu items
 * number: number of menu items
 * callback: callback function (it gets the item index, the pressed
 * key ( enter, right arrow or left arrow ) and title text of activated item
 * it shall return 1 if that is the item to leave the menu
 * backcolor: behind menu color
 * menucolor/menusel.color: Colors of the items
 * fontsize: pixel fontsize
 * pos: the position (0 = center, -1 = left, 1 = right)
 * extra: extra function to call each loop
 */
int generic_menu(char **menutext,int number, int (*callback)(int,int,char*),
						int backcolor, int menucolor, int menuselectedcolor, int fontsize, int pos,
						int clearbefore, void (*extrarender)(void)) {
	int i,max=0;
	int pad = 32;
	int boxheight = fontsize*number+fontsize/4;
	int boxwidth;
	int boxleft;

	for (i=0;i<number;i++)
		if (max<strlen(menutext[i]))
			max = strlen(menutext[i]);
	boxwidth = fontsize*max*0.6; /* width in average 0.6 of height */

	BITMAP *scrcopy=create_bitmap(screen->w,screen->h), 
		*mergebuff=create_bitmap(screen->w,screen->h),
		*buffer= create_bitmap(boxwidth+4,boxheight+4);

	int currindex=0;
	int keys=0; /* Saves pressed down and up keys to determine changes */


	if (clearbefore) rectfill(scrcopy,0,0,screen->w,screen->h,backcolor);
	if (extrarender != NULL) (*extrarender)();
	blit(screen,scrcopy,0,0,0,0,screen->w,screen->h); /* Copy screen pic */

	while (!close_button_pressed) {

		switch (pos) {
		case -1: /* left */
			boxleft = pad;
			break;
		case 0: /* center */
			boxleft = screen->w/2-boxwidth/2-fontsize/4;
			break;
		case 1: /* right */
			boxleft = screen->w-pad-boxwidth;
			break;
		}

		/* Print it */
		for (i=0;i<5;i++)
			rect(buffer,5-i,fontsize/8-i,
				boxwidth+i,boxheight-5+i,
				makecol(getr(menuselectedcolor)-i*10,getg(menuselectedcolor)-i*10,getb(menuselectedcolor)-i*10));
		for (i=0; i<number;i++) {
			textprintf_centre_ex(buffer,myfont,boxwidth/2,i*fontsize,
				(i==currindex)?menuselectedcolor:menucolor,
				-1,menutext[i]);
		}

		/* Apply on screen */
		blit(scrcopy,mergebuff,0,0,0,0,scrcopy->w,scrcopy->h);
		blit(buffer,mergebuff,0,0,boxleft,pad,buffer->w,buffer->h); /*Copy buffer to screen*/
		blit(mergebuff,screen,0,0,0,0,mergebuff->w,mergebuff->h);
		clear_bitmap(buffer); /*For next loop*/
		rectfill(buffer,0,0,buffer->w,buffer->h,backcolor);


		if (keys != general_key_state()) { /* execute only on changes */
			keys = general_key_state();
				/* Menu scroll */
			if(key[KEY_UP]) {
				currindex--;
				if (currindex<0)
					currindex = number-1;
			}
			else if(key[KEY_DOWN]) {
				currindex++;
				if (currindex > number-1)
					currindex=0;
			}

			i = 0; /* i = exit */

			if(key[KEY_ENTER]) {
				i = (*callback)(currindex,0,menutext[currindex]);
				/* Refresh extra rendering and clearing */
				if (clearbefore) rectfill(scrcopy,0,0,screen->w,screen->h,backcolor);
				if (extrarender != NULL) (*extrarender)();
				blit(screen,scrcopy,0,0,0,0,screen->w,screen->h); /* Copy screen pic */
			}
			else if (key[KEY_LEFT]) {
				i = (*callback)(currindex,-1,menutext[currindex]);
				/* Refresh extra rendering and clearing */
				if (clearbefore) rectfill(scrcopy,0,0,screen->w,screen->h,backcolor);
				if (extrarender != NULL) (*extrarender)();
				blit(screen,scrcopy,0,0,0,0,screen->w,screen->h); /* Copy screen pic */
			}
			else if (key[KEY_RIGHT]) {
				i = (*callback)(currindex,1,menutext[currindex]);
				/* Refresh extra rendering and clearing */
				if (clearbefore) rectfill(scrcopy,0,0,screen->w,screen->h,backcolor);
				if (extrarender != NULL) (*extrarender)();
				blit(screen,scrcopy,0,0,0,0,screen->w,screen->h); /* Copy screen pic */
			}

			if (i==1) /* the exit item was chosen */
				break;
		}

	}

	destroy_bitmap(buffer);
	destroy_bitmap(scrcopy);
	destroy_bitmap(mergebuff);

	return 0;
}
