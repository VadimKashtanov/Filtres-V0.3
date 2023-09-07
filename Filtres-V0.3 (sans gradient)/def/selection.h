#include "mdl.h"

typedef struct {
	//	Selection Naturelle
	float GG = 1.00;	//%gagant -> gagant
	float GP = 1.00;	//%gagant -> perdant
	float MUTG_cst     =.00,  MUTP_cst     =.05;
	float MUTG_p       =.00,  MUTP_p       =.02;
	float MUTG_ema_int =.00,  MUTP_ema_int =.01;
	float MUTG_depuis  =.00,  MUTP_depuis  =.01;

	//	Quelles zones de prixs

	//	Comment faire de l'argent (p[i+1]/p[i] ou p[i+5]/p[i])
} Env_t;	//Environnement

/*	Mixer (gagant, perdant, env, mode)
mode :
	0 = Toutes mixations confondues
	1 = mixer poids
	2 = mixer consts
	3 = mixer ema&intervalles
	4 = mixer _depuis 								*/
void mixer(Mdl_t * G, Mdl_t * P, Env_t env, uint mode);


Mdl_t * selection(Mdl_t * depuis, Env_t env);