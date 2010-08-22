/* Main game routine
 * Copyright (C) 2009 Anton Pirogov
 * Distributed under the General Public License version 3
 */
#include "main.h"

int get_score(int coins, int interval, int time) {
	return coins*1000/interval/(time+1);
}

void read_scores() {
	char buffer[32];
	char *token=NULL;
	int i=0;

	FILE *sp = fopen("highscores","r");

	if (sp!=NULL) {
		for (i=0;i<10;i++) {
			fgets(buffer,32,sp);

			token=strtok(buffer,",");
			strcpy(highscores[i][0],token);
			token=strtok(NULL,",");
			strcpy(highscores[i][1],token);
			/* Get position of \n and overwrite with \0 */
			token=strstr(highscores[i][1],"\n");
			token[0]='\0';
		}
		fclose(sp);
	} else {
		for (i=0;i<10;i++) {
			strcpy(highscores[i][0],"Anton Pirogov");
			sprintf(highscores[i][1],"%d",100-(i*10));
		}
	}
}

void write_scores() {
	int i=0;
	FILE *sp = fopen("highscores","w");

	for (i=0;i<10;i++) {
		fprintf(sp,"%s,%s\n",highscores[i][0],highscores[i][1]);
	}

	fclose(sp);
}

/* From german allegro tutorial, modified by me */
void Eingabe(FONT* myfont, char *Schreib1, int x, int y, int Schriftfarbe, int Hintergrund, char *Ausgabe,int buffsize)
{
  int val, i, anz;
  char Eingabestring[1024]={""};
  char Schreib[1024];

  /* Create copy of visible screen and a buffer for the result */
  BITMAP *screensave = create_bitmap(screen->w,screen->h);
  BITMAP *buffer = create_bitmap(screen->w,screen->h);

  blit(screen,screensave,0,0,0,0,screen->w,screen->h);

  sprintf(Schreib,"%s",Schreib1);

  while( !key[KEY_ENTER])
  {
	blit(screensave,buffer,0,0,0,0,screensave->w,screensave->h);

    val = readkey();
    if ((val >> 8) != KEY_ENTER && (val >> 8) != KEY_ESC)
    {
        anz=strlen(Eingabestring);
        if ((val >> 8) == KEY_BACKSPACE)
        {
            Eingabestring[anz-1]='\0';
        }
        else if ((val >> 8) == KEY_TAB)
        {
             for(i=0;i<9;i++) Eingabestring[anz+i]=' ';
             Eingabestring[anz+i]='\0';
        }
        else
        {
             Eingabestring[anz]=(char)(val & 0xff);
             Eingabestring[anz+1]='\0';
        }
	}

    textout_ex(buffer, myfont, Schreib, x, y, Schriftfarbe, Hintergrund);
	textout_ex(buffer, myfont, Eingabestring, x+text_length(myfont,Schreib), y, Schriftfarbe, Hintergrund);

	blit(buffer,screen,0,0,0,0,buffer->w,buffer->h);
  }

  strncpy(Ausgabe,Eingabestring,buffsize-1);
  Ausgabe[buffsize-1]='\0';
  destroy_bitmap(buffer);
  destroy_bitmap(screensave);
}


/* Asks for player name if a new highscore was broken and inserts it into the array */
void add_if_highscore(int score) {
	char name[16]; /* Buffer for player name if highscore */
	int i=0,j=0;
	int highscore=0; /* boolean */

	for (i=0;i<10;i++)
		if (score > atoi(highscores[i][1])) {
			highscore=1;
			break;
		}

		if(highscore) {
			/* ask for name */
			Eingabe(myfont,"Highscore! Your name: ",32,fontpxsize*6,makecol(128,128,128),-1,name,16);
			key[KEY_ENTER]=0;

			j=i;
			for(i=9;i>j;i--) {
				sprintf(highscores[i][0],"%s",highscores[i-1][0]);
				sprintf(highscores[i][1],"%s",highscores[i-1][1]);
			}

			sprintf(highscores[j][0],(strlen(name)>0) ? name : "<anonymous>");
			sprintf(highscores[j][1],"%d",score);
		}

	write_scores(); /* Save it */
}

