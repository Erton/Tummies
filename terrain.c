#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <png.h>

#include "tummies.h"
#include "simplex/open-simplex-noise.c"

float*** genMap(int64_t seed){
	int x, y;
	double value;
	double v0, v1, v2;
	static float*** map;
	map=(float***) malloc(sizeof(float **)*MAX_Y);
	uint32_t rgb;
	uint32_t img[MAX_X][MAX_Y];
	struct osn_context *ctx;

	open_simplex_noise(seed, &ctx);

	for (x=0; x<MAX_X; ++x) {
		map[x]=(float**) malloc(sizeof(float **)*MAX_Y);
		for (y=0; y<MAX_Y; ++y) {
			map[x][y]=(float*) malloc(sizeof(float *)*2);
			// Use three waves for smoother terrain
			v0 = open_simplex_noise4(ctx, (double) x / MAP_NOISE_SCALE / 4,
						(double) y / MAP_NOISE_SCALE / 4, 0.0, 0.0);
			v1 = open_simplex_noise4(ctx, (double) x / MAP_NOISE_SCALE / 2,
						(double) y / MAP_NOISE_SCALE / 2, 0.0, 0.0);
			v2 = open_simplex_noise4(ctx, (double) x / MAP_NOISE_SCALE / 1,
						(double) y / MAP_NOISE_SCALE / 1, 0.0, 0.0);
			value = v0 * 4 / 7.0 + v1 * 2 / 7.0 + v2 * 1 / 7.0;

			// Replace tiles that are too rich or too poor with water.
			if(value>MAX_GROWTH || value<MIN_GROWTH)
				value=-1;
			else
				value-=MIN_GROWTH;

			map[x][y][0]=value; // set growth value
			map[x][y][1]=0; // food starts at 0

			rgb = 0x000100 * (uint32_t) (pow(((value+1) * 127.5), 2) / 127.5) ;
			img[y][x] = (0x0ff << 24) | (rgb);
			//printf("%d	%f\n",(int)(127.5*(1+value)),value);
		}
	}

	// Turn borders into water.
	for(y=0; y<MAX_Y; ++y){
		map[0][y][0]=-1;
		map[MAX_X-1][y][0]=-1;
	}
	for(x=1; x<MAX_X-1; ++x){
		map[x][0][0]=-1;
		map[x][MAX_Y-1][0]=-1;
	}

	open_simplex_noise_free(ctx);
	return map;
}

float sineGrowth(int time, float maxTemp, float minTemp){
	float f=(maxTemp-MIN_TEMP)/2;
	float growth=f * sin(time*DAY_PERIOD) + f + MIN_TEMP;
	return (growth);
}

float*** tickMap(float ***map, int time, float maxTemp, float minTemp){
	int x,y;
	float growthWeatherFactor=sineGrowth(time,maxTemp,minTemp);
	for (x=0; x<MAX_X; ++x) {
		for (y=0; y<MAX_Y; ++y) {
			if(map[x][y][0]!=-1){
				float max=MAX_FOOD * pow(map[x][y][0],1.1);
				float growth=(max - map[x][y][1])/GROWTH_DIVIDER;
				growth*=growthWeatherFactor;
				map[x][y][1] += growth;
				if(map[x][y][1]>=max)
					map[x][y][1]=max;
				if(map[x][y][1]<=0)
					map[x][y][1]=0;
			}else{
				map[x][y][1]=0;
			}
		}
	}
	return map;
}

/*
float** genMap(int seed){
	static float** map;
	map=(float**) malloc(sizeof(double)*MAX_X*MAX_Y);

	for(int x=0; x<MAX_X; ++x)
		for(int y=0; y<MAX_Y; ++y)
			map[x][y]=noise(x,y);

	return map;
}*/

// Gets the ammount of food on x,y
double foodOnTile(float x, float y){
	return (1 + (sin(x)*cos(y)) );
}

