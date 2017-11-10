#include "rand.h"

extern size_t R;

void randGenerator(size_t* i, size_t* j, size_t* k, size_t commit_id){
	srand(commit_id);
	
	*i = rand() % R;
	
	do{
		*j = rand() % R;
	}while(*j==*i);
	
	do{
		*k = rand() % R;
	}while(*k == *i || *k == *j);

	return;
}

