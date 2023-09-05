#pragma once

#include "marchee.h"

//	Poids = {-2, -1, +1, +2};

#define POIDS_NEU(n) n
#define CONSTS_FLTR(n) n

typedef struct {
	//	Config dimentions & entree de la pile
	uint C;
	uint * y, * n, * type;
	uint * ema, * intervalles;

	uint max_n;

	uint *** neu_depuis;	//[couche][neurone][connection] pris dans {0..y[-1]}
	uint **  flt_depuis;	//[couche][filtre] pris dans {0..(y[-1] - n)}

	//	Espace de Parametres & Calcules
	uint constes, poids, vars;
	float * conste, * poid, * var;

	//	Depart de chaque couche dans les espaces
	uint * conste_depart, * poid_depart, * y_depart;
} Mdl_t;

//	Allocation Memoire
Mdl_t * cree_mdl(uint C, uint * y, uint * n, uint * type);
void liberer_mdl(Mdl_t * mdl);

//	Memoire dure
void ecrire_mdl(Mdl_t * mdl, char * file);
Mdl_t * lire_mdl(char * file);

//	Plume
void plume_mdl(Mdl_t * mdl);
void comportement(Mdl_t * mdl);

//	Qlqs fonctions directes
float poid_rnd();


//	Calcule f(x)
float f(Mdl_t * mdl, uint depart);