#include "mdl.h"

static float filtre_n(float * x, float * filtre, uint n) {
	float _min=x[0], _max=x[0];
	float _x;
	for (uint i=1; i < n; i++) {
		_x = x[i];
		if (_x > _max) _max = _x;
		if (_x < _min) _min = _x;
	}
	if (_min == _max) ERR("Impossible de normaliser un vecteur uniforme");

	//
	float x_norm[n];

	//
	float _s = 0;
	const float d = _max-_min;
	float tmp, signe;
	for (uint i=0; i < n; i++) {
		x_norm[i] = (x[i]-_min)/d;
		_s += sqrtf(1.0f+fabs(x_norm[i] - filtre[i]));
	}
	_s = _s/n - 1;

	//
	float _d = 0.0;
	for (int i=0; i < n-1; i++) {
		_d += powf(1.0f + fabs((x_norm[i+1]-x_norm[i]) - (filtre[i+1]-filtre[i])),2);
	}
	_d = _d/(n-1) - 1;

	//
	return ___exp(-_s*_s - _d*_d);
};

static float neurone_n(float * x, float * poid, uint n) {
	float _somme = 0.0;
	for (uint i=0; i < n; i++) _somme += x[i]*poid[i];
	return _somme/n;
};

float f(Mdl_t * mdl, uint depart) {
	uint C = mdl->C;
	uint * type = mdl->type;
	uint * y = mdl->y;
	uint * n = mdl->n;

	uint d;

	//	Filtres N
	float * _x = allouer_flotants(mdl->max_n);

	FOR(0, i, y[0]) {
		uint _ema = mdl->ema[i];
		uint interv = intervalles[mdl->intervalles[i]];
		//
		FOR(0, j, n[0]) {
			_x[j] = ema[_ema][depart - j*interv];
		}

		mdl->var[i] = filtre_n(
			_x, mdl->conste + i*CONSTS_FLTR(n[0]), n[0]);
	};
	
	FOR(1, i, C) {
		FOR(0, j, y[i]) {
			if (type[i] == 1) {
				mdl->var[mdl->y_depart[i] + j] = filtre_n(
					mdl->var + mdl->y_depart[i-1] + mdl->flt_depuis[i][j],
					mdl->conste + mdl->conste_depart[i] + mdl->n[i]*j,
					mdl->n[i]
				);
			} else if (mdl->type[i] == 2) {
				d = (C-i);
				//
				FOR(0, k, n[i]) {
					_x[k] = mdl->var[mdl->y_depart[i-1] + mdl->neu_depuis[i][j][k]];
				}
				//
				mdl->var[mdl->y_depart[i] + j] = neurone_n(
					_x, mdl->poid + mdl->poid_depart[i] + j*POIDS_NEU(mdl->n[i]),
					mdl->n[i]
				);
			} else {
				ERR("Pas de couche %i", mdl->type[i]);
			}
		}
	}
	free(_x);
	//
	return mdl->var[mdl->vars-1];
};