#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "tummies.h"
#include "terrain.c"

// Creature thingy
struct tummy{
	float pos[2];
	float speed[2]; // trueSpeed, acceleration
	float angle[2]; // trueAngle, inertia
	double synapses[NUM_SYNAPSES]; // weights of the synapses. Doubles are faster than float for some reason.
	float eyeAngle;
	float focalLength;
	float memory[MEM_NEURONS];

	float energy;
	int age;

	struct tymmy *self;
	struct tummy *offspring;
};


// Generates a random double between 0 and 1
double drand(){
	return ((double)rand()/(double)RAND_MAX);
}
// Generates a random double between -1 and 1
double nrand(){
	return ((2*(double)rand()/(double)RAND_MAX)-1);
}

// Takes a number <x> and normalizes it between -1 and 1
double normalize(double x){
	return 2/(1+pow(e,-2*x))-1;
}

// Returns the scalar product or <a> and <b> of length len
static double dotProduct(double *a, double *b, int len){
	 double result = 0.0;
	 for(int i=0; i<len; ++i)
		  result+=a[i]*b[i];
	 return result;
}

// Runs the neural network <nn> on <input>.
static double * runNetwork(double *input, double *nn){
	static double* hidden;
	hidden=malloc(LAYERS[1]*sizeof(double));
	static double* output;
	output=malloc(LAYERS[2]*sizeof(double));
	double synapses_l12[LAYERS[0]];
	double synapses_l23[LAYERS[1]];

	for(int i=0; i<LAYERS[1]-2; ++i) { // for each hidden neuron (except bias)
		for(int j=0;j<LAYERS[0];++j) // for each input neuron
			synapses_l12[j]=nn[i*(LAYERS[0])+j];
		hidden[i]=normalize(dotProduct(input,synapses_l12,LAYERS[0]));
	}
	hidden[LAYERS[1]-1]=1; // set bias

	for(int i=0; i<LAYERS[2]; ++i) { // for each output
		for(int j=0;j<LAYERS[1];++j) // for each hidden
			synapses_l23[j]=nn[SYNAPSES_ON_LAYER[0]+i*(LAYERS[1])+j];
		output[i]=normalize(dotProduct(hidden,synapses_l23,LAYERS[1]));
	}

	free(hidden);
	return output;
}

// Returns the color of where a tummy at <pos> is looking at.
static double * look(double angle, double focalLength, float *pos, float*** map){
	static int sightCoords[2];
	static double color[3];

	sightCoords[0]=round(focalLength*cos(angle)+pos[0]);
	sightCoords[1]=round(focalLength*sin(angle)+pos[1]);

	if(sightCoords[0]>=MAX_X-1)
		sightCoords[0]=MAX_X-1;
	else if(sightCoords[0]<=0)
		sightCoords[0]=0;
	if(sightCoords[1]>=MAX_Y-1)
		sightCoords[1]=MAX_Y-1;
	else if(sightCoords[1]<=0)
		sightCoords[1]=0;

	color[0]=map[sightCoords[0]][sightCoords[1]][0];
	color[1]=map[sightCoords[0]][sightCoords[1]][1];

	return color;
}

double eat(float* pos, float speed, float*** map){
	float food=
		map[(int)round(pos[0])][(int)round(pos[1])][1]
		* FOOD_BITE_PROPORTION
		/ (1+(fabsf(speed)*EAT_WHILE_MOVING_INNEFICIENCY_FACTOR));
	if(food>MAX_FOOD_BITE)
		food=MAX_FOOD_BITE;
	if(food<0)
		food=0;

	map[(int)round(pos[0])][(int)round(pos[1])][1]-=
		food * (1+(fabsf(speed)*EAT_WHILE_MOVING_INNEFICIENCY_FACTOR));
	return(food);
}

static struct tummy *mate(double mutate){
	return 0;
}

// Creates a new tummy from one, two, or no tummies.
// Takes an arg count and pointers to the parent(s).
struct tummy *newTummy(int argc, ...){
	va_list argv; va_start(argv,argc);
	static struct tummy* child;
	child=malloc(sizeof(struct tummy));

	if(argc==3){ // two parents
		struct tummy *parent1=va_arg(argv, struct tummy*);
		struct tummy *parent2=va_arg(argv, struct tummy*);
		double mutation=va_arg(argv, double);

		for(int i=0; i<NUM_SYNAPSES; ++i)
			child->synapses[i]=
				(1-mutation)*(parent1->synapses[i]+parent2->synapses[i])/2
				+ mutation*nrand();
		child->energy=5;
		child->pos[0]=parent1->pos[0];
		child->pos[1]=parent1->pos[1];
	}else if(argc==2){ // single parent
		struct tummy *parent=va_arg(argv, struct tummy*);
		double mutation=va_arg(argv, double);

		for(int i=0; i<NUM_SYNAPSES; ++i)
			child->synapses[i]=(1-mutation)*parent->synapses[i] + mutation*nrand();
		child->energy=5;
		child->pos[0]=parent->pos[0];
		child->pos[1]=parent->pos[1];
	}else if (argc==0){ // no parents
		child->energy=10;
		for(int i=0; i<NUM_SYNAPSES; ++i)
			child->synapses[i]=nrand();
			child->pos[0]=(rand()%MAX_X);
			child->pos[1]=(rand()%MAX_Y);
	}else{
		printf("PANIC PANIC PANIC PANIC PANIC");
	}

	va_end(argv);
	child->age=0;
	child->speed[0]=0; child->speed[1]=0;
	child->angle[0]=0; child->angle[1]=0;
	child->eyeAngle=2*pi*drand();
	child->offspring=0;
	child->self=child;

	return child;
}

// Lets a tummy live for one tick.
struct tummy doActions(struct tummy tummy, float*** map){
	double input[8];
	double *nnOutput;
	double *sight=look(tummy.eyeAngle+tummy.angle[0],tummy.focalLength+1,tummy.pos,map);

	// Set input neurons.
	for(int i=0; i<MEM_NEURONS; ++i) // memory
		input[i]=tummy.memory[i];
	input[1]=normalize(tummy.eyeAngle); // eye angle
	input[2]=map[(int)round(tummy.pos[0])][(int)round(tummy.pos[1])][1]; // food on tile
	input[3]=normalize(tummy.energy); // energy
	for(int i=0; i<2; ++i) // sight
		input[4+i]=sight[i];
	input[LAYERS[0]-1]=1; //bias

	// Let the tummy think.
	nnOutput=runNetwork(input,tummy.synapses);

	// Preordain harmony to let thoughts affect the material world.
	for(int i=0; i<MEM_NEURONS; ++i) // memory
		tummy.memory[i]=nnOutput[i];
	tummy.eyeAngle=nnOutput[2]; // eye angle
	tummy.speed[1]=nnOutput[3]; // acceleration
	tummy.angle[1]=nnOutput[4]; // turning speed
	if(nnOutput[4]>0) // eat
		tummy.energy+=eat(tummy.pos, tummy.speed[1], map);
	if(nnOutput[5]>0.5 && !tummy.offspring && tummy.energy>20){ // mate
		tummy.offspring=newTummy(2,&tummy,pow((nnOutput[6]+1)/2,2));
		tummy.energy-=10;
	}
	tummy.focalLength=2*(nnOutput[7]+1);
	free(nnOutput);

	tummy.energy-=METABOLISM*AGE_METABOLISM_FACTOR*tummy.age;
	// Turn
	tummy.angle[0]+=tummy.angle[1]*TURN_SPEED;
	tummy.energy-=pow(fabs(tummy.angle[1]),2)*TURN_COST;
	// Accelerate (sanic)
	tummy.speed[0]*=SPEED_DECAY;
	tummy.speed[0]+=tummy.speed[1]*SPEED_FACTOR;
	tummy.energy-=fabs(tummy.speed[1])*ACCEL_COST*(1+(tummy.speed[1]<0)*BACK_COST_FACTOR);
	// Move
	tummy.pos[0]+=tummy.speed[0]*cos(tummy.angle[0]);
	tummy.pos[1]+=tummy.speed[0]*sin(tummy.angle[0]);

	if(round(tummy.pos[0])>=(MAX_X-1)){
		tummy.pos[0]=MAX_X-1;
		tummy.speed[0]*=-1;
	}else if(round(tummy.pos[0])<=0){
		tummy.pos[0]=0;
		tummy.speed[0]*=-1;
	}
	if(round(tummy.pos[1])>=(MAX_Y-1)){
		tummy.pos[1]=MAX_Y-1;
		tummy.speed[0]*=-1;
	}else if(round(tummy.pos[1])<=0){
		tummy.pos[1]=0;
		tummy.speed[0]*=-1;
	}

	if(isnan(tummy.pos[0]) || isnan(tummy.pos[1])){
		tummy.pos[0]=0;
		tummy.pos[1]=0;
		tummy.speed[0]=0;
		tummy.angle[0]=0;
		tummy.angle[1]=0;
		printf("NAN ERROR!\nspeed: %f, %f\nangle: %f, %f\n\n\n", tummy.speed[0], tummy.speed[1], tummy.angle[0], tummy.angle[1]);
	}

	//printf("%d,%d,%f,%f\n",(int)tummy.pos[0],(int)tummy.pos[1],tummy.pos[0],tummy.pos[1]);
	if(map[(int)tummy.pos[0]][(int)tummy.pos[1]][0]==-1)
		tummy.energy-=1;


	tummy.age++;

	return tummy;
}
