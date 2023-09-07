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
	assert(C > 0);
	assert(type[0] == 0);
	assert(y[C-1] == 1);
	assert(n[0] <= MAX_N_PREMIERE_COUCHE);
	//
	mdl->C = C;
	mdl->y = copier_uint(y, C);
	mdl->n = copier_uint(n, C);
	mdl->type = copier_uint(type, C);
	//
	mdl->max_n = n[0];
	for (uint i=1; i < C; i++) if (n[i] > mdl->max_n) mdl->max_n = n[i];
	//
	mdl->conste_depart = allouer_uint(C);
	mdl->poid_depart = allouer_uint(C);
	mdl->y_depart = allouer_uint(C);
	mdl->neu_depuis = (uint***)calloc(C, sizeof(uint**));
	mdl->flt_depuis = (uint**)calloc(C, sizeof(uint*));
	//
	mdl->constes = n[0]*y[0];
	mdl->poids = 0;
	mdl->vars = y[0];
	mdl->ema = allouer_uint(y[0]); for (uint i=0; i < y[0]; i++) mdl->ema[i] = 0;//rand() % NB_DIFF_EMA;
	mdl->intervalles = allouer_uint(y[0]); for (uint i=0; i < y[0]; i++) mdl->intervalles[i] = 0;//rand() % INTERVALLES;
	//
	//	===== Autres Couches ====
	FOR(1, i, C) {
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
			mdl->conste_depart[i] = mdl->constes;
			//
			mdl->poids += y[i] * POIDS_NEU(n[i]);
			mdl->constes += 0;
		} else {
			// === Couches Filtriques ===
			mdl->flt_depuis[i] = allouer_uint(y[i]);
			FOR(0, j, y[i]) mdl->flt_depuis[i][j] = rand() % (y[i-1] - n[i]);
			//
			mdl->poid_depart[i] = mdl->poids;
			mdl->conste_depart[i] = mdl->constes;
			//
			mdl->poids += 0;
			mdl->constes += y[i] * CONSTS_FLTR(n[i]);
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
	FOR(0, c, mdl->C) {
		if (mdl->type[c] < 2) {
			FOR(0, i, mdl->y[c]) {
				uint d = mdl->conste_depart[c] + mdl->n[c]*i;
				float _max=mdl->conste[d], _min=mdl->conste[d];
				FOR(1, k, mdl->n[c]) {
					if (_max < mdl->conste[d+k]) _max = mdl->conste[d+k];
					if (_min > mdl->conste[d+k]) _min = mdl->conste[d+k];
				}
				//
				FOR(0, k, mdl->n[c]) {
					mdl->conste[d+k] = (mdl->conste[d+k]-_min)/(_max-_min);
				};
			}
		};
	};
	//
	return mdl;
};

void liberer_mdl(Mdl_t * mdl) {
	//
	for (uint i=1; i < mdl->C; i++) {
		if (mdl->type[i] == 2) {
			FOR(0, j, mdl->y[i]) {
				free(mdl->neu_depuis[i][j]);
			}
			free(mdl->neu_depuis[i]);
		} else {
			free(mdl->flt_depuis[i]);

		}
	}
	free(mdl->y);
	free(mdl->n);
	free(mdl->type);
	free(mdl->ema);
	free(mdl->intervalles);
	free(mdl->conste);
	free(mdl->poid);
	free(mdl->var);
	free(mdl->conste_depart);
	free(mdl->poid_depart);
	free(mdl->y_depart);
};

Mdl_t * copier_mdl(Mdl_t * mdl) {
	Mdl_t * ret = cree_mdl(mdl->C, mdl->y, mdl->n, mdl->type);
	memcpy(ret->ema, mdl->ema, sizeof(float)*mdl->y[0]);
	memcpy(ret->intervalles, mdl->intervalles, sizeof(float)*mdl->y[0]);
	memcpy(ret->conste, mdl->conste, sizeof(float)*mdl->constes);
	memcpy(ret->poid, mdl->poid, sizeof(float)*mdl->poids);
	FOR(0, i, mdl->C) {
		if (mdl->type[i] == 2) {
			FOR(0, j, mdl->y[i]) {
				memcpy(ret->neu_depuis[i][j], mdl->neu_depuis[i][j], sizeof(uint)*mdl->n[i]);
			}
		} else if (mdl->type[i] == 1) {
			memcpy(ret->flt_depuis[i], mdl->flt_depuis[i], sizeof(uint)*mdl->y[i]);
		}
	}
	return ret;
};

static uint comparer_uints(uint * A, uint * B, uint l) {
	FOR(0, i, l) if (A[i] != B[i]) return 1;
	return 0;
}

static uint comparer_floats(float * A, float * B, uint l) {
	FOR(0, i, l) if (A[i] != B[i]) return 1;
	return 0;
}

uint meme_mdl(Mdl_t * A, Mdl_t * B) {
	if (A->C != B->C) return 1;
	if (A->max_n != B->max_n) return 1;
	if (A->constes != B->constes) return 1;
	if (A->poids != B->poids) return 1;
	if (A->vars != B->vars) return 1;
	//
	if (comparer_uints(A->y, B->y, A->C) == 1) return 1;
	if (comparer_uints(A->n, B->n, A->C) == 1) return 1;
	if (comparer_uints(A->type, B->type, A->C) == 1) return 1;
	if (comparer_uints(A->ema, B->ema, A->y[0]) == 1) return 1;
	if (comparer_uints(A->intervalles, B->intervalles, A->y[0]) == 1) return 1;
	//
	if (comparer_floats(A->poid, B->poid, A->poids) == 1) return 1;
	if (comparer_floats(A->conste, B->conste, A->constes) == 1) return 1;
	//
	FOR(0, i, A->C) {
		if (A->type[i] == 2) {
			FOR(0, j, A->y[i]) {
				if (comparer_uints(A->neu_depuis[i][j], B->neu_depuis[i][j], A->n[i])==1)
					return 1;
			}
		} else if (A->type[i] == 1) {
			if (comparer_uints(A->flt_depuis[i], B->flt_depuis[i], A->y[i])==1)
				return 1;
		}
	};
	return 0;
};

#define HASH(h, val) (697*((h+134*(val+1) + 1291)%15734)+87) % 10000;

uint hash_mdl(Mdl_t * mdl) {
	uint h = 0;
	FOR(0, i, mdl->C) {
		if (mdl->type[i] == 2) {
			FOR(0, j, mdl->y[i]) {
				FOR(0, k, mdl->n[i]) {
					h = HASH(h, mdl->neu_depuis[i][j][k]);
				}
			}
		} else if (mdl->type[i] == 1) {
			FOR(0, k, mdl->y[i]) {
				h = HASH(h, mdl->flt_depuis[i][k]);
			}
		}
		h = HASH(h, mdl->ema[i]);
		h = HASH(h, mdl->intervalles[i]);
		h = HASH(h, mdl->y[i]);
		h = HASH(h, mdl->n[i]);
		h = HASH(h, mdl->type[i]);
	};
	FOR(0, i, mdl->poids) h = HASH(h, (uint)roundf(mdl->poid[i]*10000));
	FOR(0, i, mdl->constes) h = HASH(h, (uint)roundf(mdl->conste[i]*10000));
	return h;
};

void ecrire_mdl(Mdl_t * mdl, char * fichier) {
	FILE * fp = fopen(fichier, "wb");
	fwrite(&mdl->C, sizeof(uint), 1, fp);
	uint C = mdl->C;
	//
	fwrite(mdl->type, sizeof(uint), C, fp);
	fwrite(mdl->y, sizeof(uint), C, fp);
	fwrite(mdl->n, sizeof(uint), C, fp);
	//
	fwrite(mdl->intervalles, sizeof(uint), mdl->y[0], fp);
	fwrite(mdl->ema, sizeof(uint), mdl->y[0], fp);
	//
	FOR(1, i, C) {
		if (mdl->type[i] == 2) {
			FOR(0, j, mdl->y[i]) {
				fwrite(mdl->neu_depuis[i][j], sizeof(uint), mdl->n[i], fp);
			}
		} else if (mdl->type[i] == 1) {
			fwrite(mdl->flt_depuis[i], sizeof(uint), mdl->y[i], fp);
		} else {
			ERR("couches %i n'existe pas", mdl->type[i]);
		}
	}
	//
	fwrite(mdl->conste, sizeof(float), mdl->constes, fp);
	fwrite(mdl->poid, sizeof(float), mdl->poids, fp);
	//
	fclose(fp);
};

Mdl_t * lire_mdl(char * fichier) {
	FILE * fp = fopen(fichier, "rb");
	//
	uint C;
	fread(&C, sizeof(uint), 1, fp);
	//
	uint y[C], n[C], type[C];
	//
	fread(type, sizeof(uint), C, fp);
	fread(y, sizeof(uint), C, fp);
	fread(n, sizeof(uint), C, fp);
	//
	uint intervalles[C], ema[C];
	fread(intervalles, sizeof(uint), y[0], fp);
	fread(ema, sizeof(uint), y[0], fp);
	//
	uint ** neu_depuis[C];
	uint * flt_depuis[C];
	for (uint i=1; i < C; i++) {
		if (type[i] == 2) {
			neu_depuis[i] = malloc(sizeof(uint*) * y[i]);
			
			FOR(0, j, y[i]) {
				neu_depuis[i][j] = malloc(sizeof(uint) * n[i]);
				fread(neu_depuis[i][j], sizeof(uint), n[i], fp);
			}

		} else {
			flt_depuis[i] = malloc(sizeof(uint) * y[i]);
			fread(flt_depuis[i], sizeof(uint), y[i], fp);
		}
	}
	//
	Mdl_t * mdl = cree_mdl(C, y, n, type);
	//
	fread(mdl->conste, sizeof(float), mdl->constes, fp);
	fread(mdl->poid, sizeof(float), mdl->poids, fp);
	memcpy(mdl->ema, ema, sizeof(uint)*y[0]);
	memcpy(mdl->intervalles, intervalles, sizeof(uint)*y[0]);
	//
	fclose(fp);
	//
	for (uint i=1; i < C; i++) {
		if (type[i] == 2) {
			FOR(0, j, mdl->y[i]) {
				free(mdl->neu_depuis[i][j]);
			}
			free(mdl->neu_depuis[i]);
		} else {
			free(mdl->flt_depuis[i]);
		}
	}
	free(mdl->neu_depuis);
	free(mdl->flt_depuis);
	//
	mdl->neu_depuis = neu_depuis;
	mdl->flt_depuis = flt_depuis;
	//
	return mdl;
};

static float ppossibles[] = {-2, -1, 0, +1, +2};
float poid_rnd() {
	return ppossibles[rand() % 5];
};