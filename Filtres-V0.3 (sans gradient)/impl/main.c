#include "mdl.h"


uint __C = 5;
uint __y[]    = {20,9,4,2,1};
uint __n[]    = { 5,4,3,3,2};
uint __type[] = { 0,2,2,2,2};


/*
					80%
 G -------- G
    =     =    20%
       =
    =     =    40%
 P -------- P
               60%
*/

#define GG 1.00
#define GP 1.00
#define PG (1.00-GG)
#define PP (1.00-GP)
float MUTG_cst     =.00,  MUTP_cst     =.10;
float MUTG_p       =.00,  MUTP_p       =.10;
float MUTG_ema_int =.00,  MUTP_ema_int =.00;
float MUTG_depuis  =.00,  MUTP_depuis  =.00;

void mixer(Mdl_t * G, Mdl_t * P) {
	assert(G->constes == P->constes);
	assert(G->poids == P->poids);

	float tmp;
	uint c;

	//	Mixage des Constantes
	for (uint i=0; i < G->constes; i++) {
		tmp = G->conste[i];
		if (rnd() < PG) G->conste[i] = P->conste[i];
		if (rnd() < GP) P->conste[i] = tmp;

		if (rnd() < MUTG_cst) G->conste[i] = (3*G->conste[i] + rnd())/4;
		if (rnd() < MUTP_cst) P->conste[i] = (3*G->conste[i] + rnd())/4;
	};

	float _maxG, _maxP, _minG, _minP;
	float _P, _G;
	uint p;
	for (uint c=0; c < G->C; c++) {
		if (G->type[c] != 2) {
			for (uint j=0; j < G->y[c]; j++) {
				_G = G->conste[G->conste_depart[c] + j*G->n[c] + 0];
				_P = P->conste[G->conste_depart[c] + j*G->n[c] + 0];
				_maxG = _G;
				_maxP = _P;
				_minG = _G;
				_minP = _P;
				for (uint k=1; k < G->n[c]; k++) {
					_G = G->conste[G->conste_depart[c] + j*G->n[c] + k];
					_P = P->conste[G->conste_depart[c] + j*G->n[c] + k];
					if (_G >= _maxG) _maxG = _G;
					if (_G <= _minG) _minG = _G;
					if (_P >= _maxP) _maxP = _P;
					if (_P <= _minP) _minP = _P;
				}
				//
				if (_minG == _maxG) ERR("%f == %f\n", _minG, _maxG);
				if (_maxP == _minP) ERR("%f == %f\n", _minP, _maxP);
				//
				for (uint k=0; k < G->n[c]; k++) {
					p = G->conste_depart[c] + j*G->n[c] + k;
					G->conste[p] = (G->conste[p]-_minG)/(_maxG-_minG);
					P->conste[p] = (P->conste[p]-_minP)/(_maxP-_minP);
				}
			}
		}
	}

	//	Mixage des poids
	for (uint i=0; i < G->poids; i++) {
		tmp = G->poid[i];
		if (rnd() < PG) G->poid[i] = P->poid[i];
		if (rnd() < GP) P->poid[i] = tmp;

		if (rnd() < MUTG_p) G->poid[i] = poid_rnd();
		if (rnd() < MUTP_p) P->poid[i] = poid_rnd();
	};

	//	Mixage des Ema & Intervalles
	for (uint i=0; i < G->y[0]; i++) {
		//	Moyennes mouvantes
		c = G->ema[i];
		if (rnd() < PG) G->ema[i] = P->ema[i];
		if (rnd() < GP) P->ema[i] = c;

		if (rnd() <= MUTG_ema_int) G->ema[i] = rand() % NB_DIFF_EMA;
		if (rnd() <= MUTP_ema_int) P->ema[i] = rand() % NB_DIFF_EMA;

		//	Intervalles
		c = G->intervalles[i];
		if (rnd() < PG) G->intervalles[i] = P->intervalles[i];
		if (rnd() < GP) P->intervalles[i] = c;

		if (rnd() <= MUTG_ema_int) G->intervalles[i] = rand() % INTERVALLES;
		if (rnd() <= MUTP_ema_int) P->intervalles[i] = rand() % INTERVALLES;
	}

	//	Mixage des `neu_depuis` & `flt_depuis`
	for (uint i=0; i < G->C; i++) {
		if (G->neu_depuis[i] != 0) {
			for (uint j=0; j < G->y[i]; j++) {
				for (uint k=0; k < G->n[i]; k++) {
					c = G->neu_depuis[i][j][k];
					if (rnd() < PG) G->neu_depuis[i][j][k] = P->neu_depuis[i][j][k];
					if (rnd() < GP) P->neu_depuis[i][j][k] = c;

					if (rnd() < MUTG_depuis) G->neu_depuis[i][j][k] = rand() % G->y[i-1];
					if (rnd() < MUTP_depuis) P->neu_depuis[i][j][k] = rand() % G->y[i-1];
				};
			};
		} else if (G->flt_depuis[i] != 0) {
			for (uint j=0; j < G->y[i]; j++) {
				c = G->flt_depuis[i][j];
				if (rnd() < PG) G->flt_depuis[i][j] = P->flt_depuis[i][j];
				if (rnd() < GP) P->flt_depuis[i][j] = c;

				if (rnd() < MUTG_depuis) G->flt_depuis[i][j] = rand() % (G->y[i-1] - G->n[i]);
				if (rnd() < MUTP_depuis) P->flt_depuis[i][j] = rand() % (G->y[i-1] - G->n[i]);
			}
		};
	};
};

//	=======================================

void gain(Mdl_t * mdl, float * _gain, float * _prediction) {
	_gain[0] = USDT;
	_prediction[0] = 0.0;

	float _f, p0, p1;
	for (uint i=DEPART; i < PRIXS-1; i++) {
		p1 = prixs[i+1];
		p0 = prixs[i];
		//
		_f = signe( f(mdl, i) );
		
		if (_gain[0] > 0) _gain[0] += _gain[0] * _f * 0.20 * LEVIER * (p1/p0 - 1.0);	//20%
		//
		if (signe(p1/p0-1.0) == _f)
			_prediction[0] += 1.0;
	};
	if (_gain[0] < 0) _gain[0] = 0;

	_gain[0] -= USDT;
	_prediction[0] /= (float)(PRIXS-1-DEPART);
};

//	=======================================


int main() {
	srand(0);
	charger_les_prixs();

	//	Strategie : sng dernier changement 	= 0.539
	//	Strategie : 100% achat 					= 0.476
	//	Strategie : 100% vente 					= 0.523
	/*float __pred = 0;
	float p1,p0,_f;
	p1 = prixs[1];
	p0 = prixs[0];
	for (uint i=2; i < PRIXS-1; i++) {
		_f = -1;//signe(p1/p0 - 1); 
		p1 = prixs[i+1];
		p0 = prixs[i];
		__pred += 1.0*(signe(p1/p0 - 1) == _f);
	};
	printf("%f\n", __pred/(PRIXS-1-1)); //*/

	//	La population
	uint n = 2;
#define K 3 		// K : combien de perdants pour 1 gagant
	uint N = (1+K)*n;
	Mdl_t * mdl[N];
	for (uint i=0; i < N; i++) mdl[i] = cree_mdl(__C, __y, __n, __type);
			
	//	Les points de score
	float gains[N]; 		uint rang_gains[N];				//gain exacte depuis le depart avec un pret
	float prediction[N]; 	uint rang_prediction[N];	//% reussite de prediction de tendance

	//	Points Finaux et 
	uint points[N];
	uint cintasat[N];	//#0, #1, #...  le 0-eme (1er) correspond a quel mdl, le 1-er ...

	for (uint i=0; i < n; i++) {
		for (uint k=0; k < K; k++)
			mixer(mdl[rand()%N], mdl[rand()%N]);
	}

	printf("==============================================================\n");
	printf("==============================================================\n");
	printf("==============================================================\n");
	printf("==============================================================\n");
	printf("==============================================================\n");
	//
#define T 100
	for (uint t=0; t < T; t++) {
		//	Scores
		for (uint i=0; i < N; i++) {
			float _g=prediction[cintasat[0]];
			gain(mdl[i], gains+i, prediction+i);
			if (t > 0) assert(prediction[cintasat[0]]==_g);
			rang_gains[i] = i;
			rang_prediction[i] = i;
			points[i] = 0;
			cintasat[i] = i;
			printf("%f,", prediction[i]);
		}
		printf("\n");

		//
		//		!!!!!!!! toujours pas regler le probleme du cintastat[0]
		//

		//	Sommes des Points
		//printf("::::");
		for (uint i=0; i < N; i++) {
			//printf("%i:%.3g,  ", rang_prediction[i], prediction[rang_prediction[i]]);
		}
		//printf(" <<< \n");

		//	Cintasar `gains` & `predictions`
		uint c; float tmp;
		for (uint i=0; i < N; i++) {
			for (uint j=i+1; j < N; j++) {
				if (gains[rang_gains[i]] < gains[rang_gains[j]]) {
					c = rang_gains[i];
					rang_gains[i] = rang_gains[j];
					rang_gains[j] = c;
				}

				if (prediction[rang_prediction[i]] < prediction[rang_prediction[j]]) {
					c = rang_prediction[i];
					rang_prediction[i] = rang_prediction[j];
					rang_prediction[j] = c;
				}
			}
		}

		//	Sommes des Points
		for (uint i=0; i < N; i++) {
			points[rang_gains[i]] += 0*(N-i);
			points[rang_prediction[i]] += (N-i);
		//	printf("%i:%.3g,  ", rang_prediction[i], prediction[rang_prediction[i]]);
		}
		//printf(" <<< \n");

		//	Hierarchie des Points
		for (uint i=0; i < N; i++) {
			for (uint j=i; j < N; j++) {
				if (points[cintasat[i]] < points[cintasat[j]]) {
					c = cintasat[i];
					cintasat[i] = cintasat[j];
					cintasat[j] = c;
				}
			}
		}
		//for (uint i=0; i < N; i++) printf("%i:%i, ", i, cintasat[i]);
		//printf("\n");

		//	Plumage d'avancement
#define TOUT_LES 10
		if (t % TOUT_LES == 0 || t==(T-1))
			printf("%i/%i %.3g%%   [gain=%f, prediction=%f]\n",
				t+1, T, (float)(t+1)/(T)*100,
				gains[cintasat[0]], prediction[cintasat[0]]);

		plume_mdl(mdl[cintasat[0]]);
		//	Si pas derniere iteration
		if (t != T-1) {
			//	Mixage
			for (uint i=0; i < n; i++) {	//	n = N / (K+1)
				for (uint k=0; k < K; k++) {
					mixer(mdl[cintasat[i]], mdl[cintasat[n + i*K + k]]);
				}
			}
		}
		plume_mdl(mdl[cintasat[0]]);
	}

	plume_mdl(mdl[cintasat[0]]);
	
	srand(0);
	comportement(mdl[cintasat[0]]);
};