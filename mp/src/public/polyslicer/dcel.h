#pragma once
#include "vector2.h"

enum vertexType
{
	startv = 1, endv = 2, mergev = 4, splitv = 8, regularv = 16, unassigned = 32
};
enum edgeDirection
{
	undefined = 1, cW = 2, cCW = 4
};
struct Dcel
{
	Vector2f orig;
	struct Dcel* twin;
	struct Dcel* next;
	struct Dcel* prev;
	struct Dcel* helper = nullptr;
	struct Dcel* unionHelper = nullptr;
	bool hasTwin = false;
	bool holeEdge = false;
	bool triangulated = false;
	bool visited = false;
	bool inside = false;
	bool marked = false;
	bool deleteMe = false;
	bool drawtext = false;
	vertexType type = unassigned;
	edgeDirection direction = undefined;
};