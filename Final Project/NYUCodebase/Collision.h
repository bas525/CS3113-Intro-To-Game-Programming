#pragma once
#include <vector>
#include <algorithm>
#include "Vect.h"


bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<Vect> &points1, const std::vector<Vect> &points2, Vect &penetration);

bool penetrationSort(const Vect &p1, const Vect &p2);

bool checkSATCollision(const std::vector<Vect> &e1Points, const std::vector<Vect> &e2Points, Vect &penetration);

