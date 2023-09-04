#pragma once

#include "marchee.h"

//	Poids = {-2, -1, 1, 2};

typedef struct {
	//	Config dimentions & entree de la pile
	uint C;
	uint *y, *n, *t;
	uint * ema, * intervalles;

	//	Espace de Parametres & Calcules
	uint constes, poids, vars;
	float * conste, * poid, * var;
} Mdl_t;

typedef struct {
	uint 
	float gain_moyen;
	float 
} Resultat;

void 