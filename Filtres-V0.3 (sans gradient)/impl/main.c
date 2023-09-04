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

float GG=.90, GP=.75, PG=.10, PP=.25;
float MUTG=.10, MUTP=.20;

void mixer(Mdl_t * G, Mdl_t * P) {
	assert(G->consets == P->constes);
	assert(G->poids == P->poids);

	float tmp;
	uint c;

	//	Mixage des Constantes
	for (uint i=0; i < G->constes; i++) {
		tmp = G->conste[i];
		if (rnd() < GP) G->conste[i] = P->conste[i];
		if (rnd() < PG) P->conste[i] = tmp;

		if (rnd() < MUTG) G->conste[i] = (2*G->conste[i] + rnd())/3;
		if (rnd() < MUTP) P->conste[i] = (2*G->conste[i] + rnd())/3;
	};

	//	Mixage des poids
	for (uint i=0; i < G->poids; i++) {
		tmp = G->poid[i];
		if (rnd() < GP) G->poid[i] = P->poid[i];
		if (rnd() < PG) P->poid[i] = tmp;

		if (rnd() < MUTG) G->poid[i] = poid_rnd();
		if (rnd() < MUTP) P->poid[i] = poid_rnd();
	};

	//	Mixage des Ema & Intervalles
	for (uint i=0; i < G->y[0]; i++) {
		//	Moyennes mouvantes
		c = G->ema[i];
		if (rnd() < GP) G->ema[i] = P->ema[i];
		if (rnd() < PG) P->ema[i] = c;

		if (rnd() < MUTG) G->ema[i] = rand() % NB_DIFF_EMA;
		if (rnd() < MUTP) P->ema[i] = rand() % NB_DIFF_EMA;

		//	Intervalles
		c = G->intervalles[i];
		if (rnd() < GP) G->intervalles[i] = P->intervalles[i];
		if (rnd() < PG) P->intervalles[i] = c;

		if (rnd() < MUTG) G->intervalles[i] = rand() % INTERVALLES;
		if (rnd() < MUTP) P->intervalles[i] = rand() % INTERVALLES;
	}

	//	Mixage des `neu_depuis` & `flt_depuis`
	for (uint i=0; i < G->couches; i++) {
		if (G->neu_depuis[i] != 0) {
			for (uint j=0; j < G->y[i]; j++) {
				for (uint k=0; k < G->n[i]; k++) {
					c = G->neu_depuis[i][j][k];
					if (rnd() < GP) G->neu_depuis[i][j][k] = P->neu_depuis[i][j][k];
					if (rnd() < PG) P->eneu_depuis[i][j][k] = c;

					if (rnd() < MUTG) G->neu_depuis[i][j][k] = rand() % G->y[i-1];
					if (rnd() < MUTP) P->neu_depuis[i][j][k] = rand() % G->y[i-1];
				};
			};
		} else if (G->flt_depuis[i] != 0) {
			for (uint j=0; j < G->y[i]; j++) {
				c = G->flt_depuis[i][j];
				if (rnd() < GP) G->flt_depuis[i][j] = P->flt_depuis[i][j];
				if (rnd() < PG) P->flt_depuis[i][j] = c;

				if (rnd() < MUTG) G->flt_depuis[i][j] = rand() % (G->y[i-1] - G->n[i]);
				if (rnd() < MUTP) P->flt_depuis[i][j] = rand() % (G->y[i-1] - G->n[i]);
			}
		};
	};
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
#define K 1 		// K : combien de perdants pour 1 gagant
	uint N = (1+K)*n;
	Mdl_t * mdl[N];
	for (uint i=0; i < N; i++) mdl[i] = cree_mdl(C, y, n, type);
			
	//	Les points de score
	float gains[N]; 		uint rang_gains[N];			//gain exacte depuis le depart avec un pret
	float prediction[N]; 	uint rang_prediction[N];	//% reussite de prediction de tendance

	//	Points Finaux et 
	uint points[N];
	uint cintasat[N];	//#0, #1, #...  le 0-eme (1er) correspond a quel mdl, le 1-er ...

	//
#define T 10
	for (uint t=0; t < T; t++) {
		//	Scores
		for (uint i=0; i < N; i++) {
			gain(mdl[i], gains+i, prediction+i);
			rang_gains[i] = i;
			rang_prediction[i] = i;
			points[i] = 0;
			cintasat[i] = i;
		}

		//	Cintasar `gains` & `predictions`
		uint c; float tmp;
		for (uint i=0; i < N; i++) {
			for (uint j=i + 1; j < N; j++) {
				if (gains[i] < gains[j]) {
					tmp = gains[i];
					gains[i] = gains[j];
					gains[j] = tmp;
					//
					c = rang_gains[i];
					rang_gains[i] = rang_gains[j];
					rang_gains[j] = c;
				}

				if (prediction[i] < prediction[j]) {
					tmp = prediction[i];
					prediction[i] = prediction[j];
					prediction[j] = tmp;
					//
					c = rang_prediction[i];
					rang_prediction[i] = rang_prediction[j];
					rang_prediction[j] = c;
				}
			}
		}

		//	Sommes des Points
		for (uint i=0; i < N; i++) {
			points[rang_gains[i]] += N-i;
			points[rang_prediction[i]] += N-i;
		}

		//	Hierarchie des Points
		for (uint i=0; i < N; i++) {
			for (uint j=i + 1; j < N; j++) {
				if (points[i] < points[j]) {
					c = points[i];
					points[i] = points[j];
					points[j] = c;
					//
					c = cintasat[i];
					cintasat[i] = cintasat[j];
					cintasat[j] = c;
				}
			}
		}

		//	Plumage d'avancement
#define TOUT_LES 2
		if (t % TOUT_LES == 0)
			printf("%i/%i %i%%   [gain=%f, prediction=%f]\n",
				t, T, (float)(t+1)/(T)*100,
				gains[cintasat[0]], prediction[cintasat[0]]);

		//	Si pas derniere iteration
		if (t != T-1) {
			//	Mixage
			for (uint i=0; i < n; i++) {	//	N / (K+1)
				for (uint k=0; k < K; k++)
					mixer(mdl[cintasat[i]], mdl[cintasat[n + i*K + k]]);
			}
		}
	}
};