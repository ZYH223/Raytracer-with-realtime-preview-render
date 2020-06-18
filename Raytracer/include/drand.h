#pragma once
#ifndef DRAND48_H
#define DRAND48_H

#include <stdlib.h>
#include <iostream>

#define drand_m 0x100000000LL
#define drand_c 0xB16
#define drand_a 0x5DEECE66DLL

static unsigned long long seed = 1;

double drand48(void)
{
	seed = (drand_a * seed + drand_c) & 0xFFFFFFFFFFFFLL;
	unsigned int x = seed >> 16;
	return 	((double)x / (double)drand_m);

}

void srand48(unsigned int i)
{
	seed = (((long long int)i) << 16) | rand();
}

#endif