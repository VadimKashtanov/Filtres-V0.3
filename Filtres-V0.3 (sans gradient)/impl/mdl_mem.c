#include "mdl.h"

uint* allouer_uint(uint C) {
	return calloc(C, sizeof(uint));
}

static uint * copier_uint(uint * x, uint n) {
	uint * r = malloc(sizeof(uint) * n);
	memcpy(r, x, sizeof(uint)*n);
	return r;
}

Mdl_t * cree_mdl(uint C, uint * y, uint * n, uint * type)
{
	Mdl_t * mdl = malloc(sizeof(Mdl_t));
	//
	mdl->C = C;
	mdl->y = copier_uint(y, C);
	mdl->n = copier_uint(n, C);
	mdl->type = copier_uint(type, C);
	//
	mdl->conste_depart = allouer_uint(C);
	mdl->poid_depart = allouer_uint(C);
	mdl->y_depart = allouer_uint(C);
	mdl->neu_depuis = allouer_uint(C);
	mdl->flt_depuis = allouer_uint(C);
	//
	mdl->constes = n[0]*y[0];
	mdl->poids = 0;
	mdl->vars = y[0];
	mdl->ema = allouer_uint(y[0]); for (uint i=0; i < y[0]; i++) ema[i] = rand() % NB_DIFF_EMA;
	mdl->intervalles = allouer_uint(y[0]); for (uint i=0; i < y[0]; i++) ema[i] = rand() % INTERVALLES;
	//
	//	===== Autres Couches ====
	FOR(1, i, N) {
		if (mdl->type[i] == 2) {
			//	=== Couches Neurones ===
			mdl->neu_depuis[i] = malloc(sizeof(uint*) * y[i]);
			
			FOR(0, j, y[i]) {
				mdl->neu_depuis[i][j] = allouer_uint(n[i]);
				FOR(0, k, n[i]) {
					mdl->neu_depuis[i][j][k] = rand() % y[i-1];
				}
			}
			//
			mdl->poid_depart[i] = mdl->poids;
			mdl->conste_depart[i] = mdl->constantes;
			//
			mdl->poids += y[i] * POIDS_NEU(n[i]);
			mdl->constantes += 0;
		} else {
			// === Couches Filtriques ===
			mdl->flt_depuis[i] = allouer_uint(y[i]);
			FOR(0, j, y[i]) mdl->flt_depuis[i][j] = rand() % (y[i-1] - n[i]);
			//
			mdl->poid_depart[i] = mdl->poids;
			mdl->conste_depart[i] = mdl->constes;
			//
			mdl->poids += 0;
			mdl->constantes += y[i] * CONSTS_FLTR(n[i]);
		}
		//
		mdl->y_depart[i] = mdl->vars;
		mdl->vars += y[i];
	}
	//
	mdl->conste = allouer_flotants(mdl->constes);
	mdl->poid = allouer_flotants(mdl->poids);
	mdl->var = allouer_flotants(mdl->vars);
	//
	FOR(0, i, mdl->poids) mdl->poid[i] = poid_rnd();
	FOR(0, i, mdl->constes) mdl->conste[i] = rnd();
	//
	return mdl;
};

void liberer_mdl(Mdl_t * mdl) {

};

void ecrire_mdl(Mdl_t * mdl, char * file) {

};

Mdl_t * lire_mdl(char * file) {

};

static float ppossibles[] = {-2, -1, 1, 2};
float poid_rnd() {
	return ppossibles[rand() % 4];
};