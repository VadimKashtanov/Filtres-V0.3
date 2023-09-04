#include "mdl.h"

uint C = 3;
uint y[] = {3,2,1};
uint n[] = {4,2,2};
uint type[] = {0,2,2};

/*
				90%
 G -------- G
    =     =     10%
       =
    =     =     75%
 P -------- P
                25%
*/

float GG=.9, GP=.75, PG=.1, PP=.25;

void mixer(Mdl_t * G, Mdl_t * P) {

};

//	=======================================

void calcule_score(Mdl_t * mdl, float * _gain, float * _prediction) {
	float p0, p1;

	*_gain = USDT;
	*_prediction = 0;

	float _f, p0, p1;
	for (uint i=DEPART; i < PRIXS-1; i++) {
		_f = signe( f(mdl, i) );

		_gain += _gain * _f * 0.20 * LEVIER * (p1/p0 - 1.0);	//20%
		if (signe(p1/p0-1.0) == _f)
			_prediction += 1.0;
	};

	*_gain -= USDT;
	*_prediction /= (float)(PRIXS-1-DEPART);
};

//	=======================================

int main() {
	//	La population
	uint n = 10;
	uint N = 2*n;
	Mdl_t * mdl[N];
	for (uint i=0; i < N; i++) mdl[i] = cree_mdl(C, y, n, type);
			
	//	Les points de score
	float gains[N]; 		uint rang_gains[N];			//gain exacte depuis le depart avec un pret
	float prediction[N]; 	uint rang_prediction[N];	//% reussite de prediction de tendance

	//	Points Finaux et 
	uint points[N];
	uint cintasat[N];	//#0, #1, #...

	//
#define T 100
	for (uint t=0; t < T; t++) {
		//	Scores
		for (uint i=0; i < N; i++)
			gain(mdl[i], gains+i, prediction+i);

		//	Cintasar `gains` & `predictions`
		uint c;
		for (uint i=0; i < N; i++) {
			for (uint j=i + 1; j < N; j++) {
				if (gains[i] < gains[j]) {

				}
			}
		}
	}
};