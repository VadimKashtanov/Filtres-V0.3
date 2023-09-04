#include "mdl.h"

static float filtre_n(float * x, float * filtre, uint n) {
	float _min=x[0], _max=x[0];
	float _x;
	for (uint i=1; i < n; i++) {
		_x = arr[i];
		if (_x > _max) _max = _x;
		if (_x < _min) _min = _x;
	}

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
	uint C = mdl->N;
	uint * type = mdl->type;
	uint * y = mdl->y;
	uint * n = mdl->n;

};