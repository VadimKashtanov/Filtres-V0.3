#pragma once

#include "marchee.h"

//	Poids = {-2, -1, 1, 2};

typedef struct {
	//	Config dimentions & entree de la pile
	uint C;
	uint * y, * n, * type;
	uint * ema, * intervalles;

	uint *** neu_depuis;	//[couche][neurone][connection] pris dans {0..y[-1]}
	uint **  flt_depuis;	//[couche][filtre] pris dans {0..(y[-1] - n)}

	//	Espace de Parametres & Calcules
	uint constes, poids, vars;
	float * conste, * poid, * var;

	//	Depart de chaque couche dans les espaces
	uint * conste_depart, * poid_depart, * y_depart;
} Mdl_t;

//	Allocation Memoire
Mdl_t * cree_mdl();
void liberer_mdl();


//	Qlqs fonctions directes
float poid_rnd();


//	Calcule f(x)
float f(Mdl_t * mdl, uint depart);