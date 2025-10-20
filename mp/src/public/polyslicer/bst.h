#pragma once
#ifndef BST_H
#define BST_H
#include "dcel.h"
#include "globals.h"
struct Bstd
{
	Dcel* data;
	Bstd* left;
	Bstd* right;
	Bstd()
	{
		data = nullptr;
		left = nullptr;
		right = nullptr;
	}
	Bstd(Dcel* edge)
	{
		data = edge;
		left = nullptr;
		right = nullptr;
	}
};

Bstd popBst(Bstd* &root);
Bstd* findMin(Bstd* root);
Bstd* findPrev(Bstd* root, Dcel* edge, Vector2f* sweepLoc);
Bstd* findNext(Bstd* root, Dcel* edge, Vector2f* sweepLoc);
Bstd* delBstX(Bstd* &root, Dcel* edge, Vector2f* sweepLoc);
Bstd* delBstY(Bstd*& root, Dcel* edge, Vector2f* sweepLoc);
Bstd* insertBstX(Bstd* &root, Dcel* edge, Vector2f* sweepLoc);
Bstd* insertBstY(Bstd*& root, Dcel* edge);
void printLeftRight(Bstd* root);
int countAll(Bstd* root, Dcel* edge, Vector2f* sweepLoc);

#endif