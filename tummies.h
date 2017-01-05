#ifndef _TUMMIESH_
#define _TUMMIESH_

	#include <stdbool.h>
	#include <math.h>

	#define e 2.718
	#define pi 3.142

	// Neurons
	//IN:  memory, eyeAngle, foodyness, energy, sight[2](, bias)
	//OUT: memory, eyeAngle, move, turn, eat, reproduce, mutate
	#define NUM_LAYERS 3
	#define NUM_SYNAPSES 128
	#define MEM_NEURONS (1)
	const int LAYERS[NUM_LAYERS]={7,9,8};
	const int SYNAPSES_ON_LAYER[2]={56,72};

	// Tumy attributes
	const double METABOLISM=0.005;
	const double TURN_COST=0.1;
	const double ACCEL_COST=0.10;
	const double BACK_COST_FACTOR=1.2;
	const double EAT_COST=0.1;
	const double SPEED_FACTOR=0.30; // gotta go fast
	const double SPEED_DECAY=0.925; // speed reduced by 50% after ln(0.5)/ln(SPEED_DECAY). Takes 9f now.
	const double TURN_SPEED=(2*pi)/256; // takes up to 16 frames to turn completely
	const double FOCAL_LENGTH=1; // how far a tummy sees
	const double AGE_METABOLISM_FACTOR=1/2; // the older a tummy is, the more its metabolism needs

	//Map attributes
	const int MAX_X=48;
	const int MAX_Y=48;
	const int MAP_NOISE_SCALE=2;
	const float MIN_GROWTH=-0.1;  //
	const float MAX_GROWTH=0.25; // replace grass with water between those
	const float GROWTH_DIVIDER=128; // lower is faster
	const float MAX_FOOD=64; // tiles can contain up to MAX_FOOD * localGrowth

	// Weather
	const int DAY_LENGTH=256;
	const float DAY_PERIOD=(2*pi)/DAY_LENGTH;
	const float MAX_TEMP=2;
	const float MIN_TEMP=-1;
	const float AVG_TEMP=(MAX_TEMP+MIN_TEMP)/2;

	// Eating
	const float MAX_FOOD_BITE=1; // max food eaten per tick
	const float FOOD_BITE_PROPORTION=0.85; // percentage of tile food eaten
	const float EAT_WHILE_MOVING_INNEFICIENCY_FACTOR=2; // the faster you move, the less you eat

	// Display
	const int MAP_DISP_SCALE=8;


	const int MAX_TUMMIES=65536;
	const int MIN_TUMMIES=16;
#endif
