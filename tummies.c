#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "tummies.h"

struct tummy;

struct tummyList{
	int count;
	struct tummy* members[MAX_TUMMIES];
};

int findFreeSlot(struct tummy* t[]){
	int slot=0;
	for(int i=0;i<MAX_TUMMIES; ++i)
		if(t[i]==0)
			rerurn i;
	return -1;
}

struct tummyList live(struct tummyList l){
	for(int i=0; l.members[i]; ++i){
		if(l.members[i]->energy>0){
			alive+=1
			*l.members[i]=doActions(*l.members[i]0;
		}
	}
}
