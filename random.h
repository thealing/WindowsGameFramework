#pragma once

#include <stdlib.h>

#define RANDOM_MAX INT_MAX

int random();

int random_below(int x);

int random_in_range(int l, int h);

double random_fraction();

double random_fraction_below(double x);

double random_fraction_in_range(double l, double h);

