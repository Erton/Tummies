#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "tummy.c"
//#include "tummies.c"
#include "tummies.h"
#include "SDL/SDL.h"

#define START_TUMMIES 32
#define MAX_TUMMIES 65536
#define FRAME_EVERY 4

void writePx(SDL_Surface* screen, int x, int y, int r, int g, int b){
	Uint32 colour;
	Uint32 *pixmem32;

	colour = SDL_MapRGB( screen->format, r, g, b);

	pixmem32 = (Uint32*) screen->pixels  + x + y*MAX_X*MAP_DISP_SCALE;
	*pixmem32 = colour;
	return;
}

int main(){
	//srand(1);
	srand((unsigned)time(NULL));
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(
		MAX_X*MAP_DISP_SCALE,
		MAX_Y*MAP_DISP_SCALE,
		32,
		SDL_HWSURFACE | SDL_DOUBLEBUF);
	int time=0;
	float maxTemp=MAX_TEMP;
	float minTemp=MIN_TEMP;
	struct tummy *tummies[MAX_TUMMIES];



	for(int i=0; i<START_TUMMIES; ++i)
		tummies[i]=newTummy(0);
	for(int i=START_TUMMIES; i<MAX_TUMMIES; ++i)
		tummies[i]=0;

	float*** map=genMap((uint32_t)rand());

	int alive=START_TUMMIES;
	bool running=true;

	while(running){
		tickMap(map,time++,maxTemp,minTemp);
		alive=0;

		for(int x=0; x<MAX_X; ++x){
			for(int y=0; y<MAX_Y; ++y){
				float foodiness=sqrt(map[x][y][1]/MAX_FOOD); // make foody tiles easy to spot
				for(int dx=0; dx<MAP_DISP_SCALE; ++dx)
					for(int dy=0; dy<MAP_DISP_SCALE; ++dy)
						writePx(screen, x*MAP_DISP_SCALE+dx, y*MAP_DISP_SCALE+dy,
							(map[x][y][0]+1)*64 * (1-foodiness),
							(map[x][y][0]+1)*127.5*(1+foodiness)/2,
							0);
			}
		}

		for(int i=0; i<MAX_TUMMIES; ++i){
			if(tummies[i]){ // check if tummy exists
				if(tummies[i]->energy>0){
					writePx(screen,
						round(MAP_DISP_SCALE*tummies[i]->pos[0]),
						round(MAP_DISP_SCALE*tummies[i]->pos[1]),
						(normalize(tummies[i]->energy)+1)*255, 255, 255);
					*tummies[i]=doActions(*tummies[i], map);
					alive+=1;

					// birth a new tummy
					if(tummies[i]->offspring!=0){
						int new=0;
						while(tummies[new])
							++new;
						tummies[new]=tummies[i]->offspring;
						tummies[i]->offspring=0;
					}
				}else{ // kill tummy
					tummies[i]=0;
				}
			}
		}

		// replace tummies if they fall below threshold
		while(alive<START_TUMMIES){
			int new=0;
			while(tummies[new])
				++new;
			tummies[new]=newTummy(0);
			++alive;
		}

		// Once per day
		if(time>=DAY_LENGTH){
			time=0;
			// prevent over/under-population by adjusting weather
			if(alive>TARGET_MAX_TUMMIES){
				maxTemp*=TEMP_DECAY_FACTOR;
			}else if(alive<TARGET_MAX_TUMMIES){
				maxTemp+=fabsf(MAX_TEMP-maxTemp)/TEMP_INCREASE_FACTOR;
			}
		}
		SDL_Flip(screen);

		printf("\nmt=%f\n%d alive\n\e[90m",maxTemp,alive);
		usleep(1000*16);
	}

	SDL_Quit();
	return 0;
}


