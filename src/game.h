#ifndef game_h_included
#define game_h_included

#include "vec.h"

#define FPS 30
#define N_SEGS 30
#define CAVE_DEPTH 100

typedef struct {
	// triangle strip
	float radius;
	Vec3 pos, vel;
	bool lefton, righton;
	float dist;
} Ship;

typedef struct {
	Vec3 segs[CAVE_DEPTH][N_SEGS];
	int i; // indice do array circular
} Cave;

void cave_gen(Cave *cave, Ship *digger);
void cave_init(Cave *cave, Ship *digger);
void ship_init(Ship* ship, float radius);
void ship_move(Ship *ship, float dt);
void digger_control();
float collision(Cave *, Ship *);

#endif
