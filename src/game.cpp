
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include "vec.h"
#include "game.h"

#define GRAVITY 3.8
#define THRUST (GRAVITY * 2)
float velocity = 30.0;

void cave_gen(Cave *cave, Ship *digger)
{
	int i = (cave->i-1+CAVE_DEPTH)%CAVE_DEPTH;
	//printf("[%d/%d] d(%f), c(%f)\n", cave->i, i, digger->pos[2], cave->segs[i][0][2] );
	if( digger->pos[2] > 1 && digger->pos[2] - SEGMENT_LEN < cave->segs[i][0][2] )
		return;
	//puts("gen");
	for( i = 0; i < N_SEGS; ++i ) {
		float a = i*M_PI*2/N_SEGS;
		float r = digger->radius;
		SET(cave->segs[cave->i][i],
			digger->pos[0] + r*cos(a),
			digger->pos[1] + r*sin(a),
			((int)(digger->pos[2]/SEGMENT_LEN))*SEGMENT_LEN
		);
	}

	cave->i ++;
	if( cave->i >= CAVE_DEPTH )
		cave->i = 0;
}

void cave_init(Cave *cave, Ship *digger)
{
	cave->i=0;
	do {
		digger_control(digger);
		ship_move(digger, 1./FPS);
		cave_gen(cave, digger);
	}
	while(cave->i != 0);
}

void ship_init(Ship* ship, float radius)
{
	SET(ship->pos,0,0,0);
	SET(ship->vel,0,0,velocity);
	ship->radius = radius;
}

void ship_move(Ship *ship, float dt)
{
#if 1
	float a = THRUST*dt;


	if(ship->lefton) {
		Vec3 leftup = {-a,a/2,0};
		ADD(ship->vel, leftup);
	}

	if(ship->righton) {
		Vec3 rightup = {+a,a/2,0};
		ADD(ship->vel, rightup);
	}

	ship->vel[1] -= GRAVITY*dt;

	ADDSCALE(ship->pos, ship->vel, dt);
#else
	if(ship->lefton)
		ship->vel[1] += THRUST*dt;

	ship->vel[1] -= GRAVITY*dt;

	ship->pos[1] += ship->vel[1]*dt;
#endif
}

void digger_control(Ship *ship)
{
#if 0
	ship->lefton  = rand()%2 ? true : false;
	ship->righton = rand()%2 ? true : false;
#else
	static bool cave_change = true;
	static float repeat = 0;

	float skill = log((ship->pos[2]+200)/200);
	if(repeat * ship->vel[1] < 0)
		repeat = 0;
	repeat += ship->vel[1]/1000.;

	int dir = ship->lefton || ship->righton ? -1: 1;
	float velocity_change_rate = 1./3.;
	if(
		(RAND < fabs(velocity_change_rate*ship->vel[1]/skill) && ship->vel[1]*dir < 0)
		|| (RAND < fabs(repeat * skill) && repeat*dir < 0)
	) {
		ship->lefton  = !ship->lefton;
		ship->righton = !ship->righton;
	}

	float max_cave_height = 10+30/(1+skill);
	float min_cave_height = max_cave_height/skill;

	float cave_change_rate = .2;
	if(RAND > cave_change_rate/skill)
		 cave_change = !cave_change;

	if(ship->radius < min_cave_height) {
		cave_change = !cave_change;
		ship->radius = min_cave_height;
	}

	if(ship->radius > max_cave_height) {
		cave_change = !cave_change;
		ship->radius = max_cave_height;
	}

	if(cave_change)
		ship->radius += RAND>.5?-1:1;
#endif
	//printf("l(%d), r(%d)\n", ship->lefton, ship->righton);
}

float collision(Cave *cave, Ship *ship)
{
	int j = cave->i;
	float min = FLT_MAX;
	int i;
	for( i = 0; i < N_SEGS; ++i ) {
		int i0 = (i+0)%N_SEGS;
		int i1 = (i+1)%N_SEGS;
		Vec3 seg;
		SUB2(seg, cave->segs[j][i1], cave->segs[j][i0]);
		Vec3 front = {0,0,1};
		Vec3 normal;
		CROSS(normal, front, seg);
		Vec3 dist;
		SUB2(dist, ship->pos, cave->segs[j][i0]);
		float dir = DOT(normal, dist);
		float len = LEN(dist);
		if(dir < 0) len = -len;
		if(len < min) min = len;
	}

	assert(min != FLT_MAX);

#if 0
	int j0 = (cave->i-1+CAVE_DEPTH)%CAVE_DEPTH;
	int j1 = (cave->i);
	int j2 = (cave->i + 1)%CAVE_DEPTH;
#endif
	ship->dist = min;

	return min;
}

// vim600:fdm=syntax:fdn=1:

