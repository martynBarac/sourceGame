#pragma once
#include <iostream>
#include <vector>
#include "vector2.h"
#include "dcel.h"
#include "bst.h"


/*struct Bstd
{
	Dcel* data;
	Bstd* left;
	Bstd* right;
};*/

struct ise
{
	Dcel* edge1;
	Dcel* edge2;
	Vector2f orig;
};

class Polygon
{
public:
	Polygon()
	{

	}
	virtual ~Polygon() = default;

	void load(std::string path); // O(n)
	void duplicate(Polygon p);
	void generateDcel(); // O(n)
	std::vector<Vector2f> mergeSortHeight(std::vector<Vector2f> * pts); // O(nlogn)

	Dcel * start_edge;
	std::vector<Dcel*> start_edges;
	std::vector<Dcel*> start_holes;
	std::vector<Dcel*> edges;
	std::vector<Vector2f> points;
	Bstd * Q;
	void traverseDcel(Dcel* edge);
	void translate(Vector2f trans);
	void mergeSort(std::vector<Vector2f>& A, std::vector<Vector2f>& B); // O(nlogn)
	void mergeSort(std::vector<Dcel*> pts, std::vector<Dcel*>& sortedPts); // O(nlogn)
	void sortEdgeList(); // O(nlogn)
	void generateBst();
	void generateBst(int start, int end);
	void makeMonotone();
	void addHole(Polygon &hole, float xoffset, float yoffset, bool ignoreHoles = true);
	void detectHole();
	void findIntersections();
	void findIntersectionsHoles();
	void resetStartEdges();
	void resetAllEdges();
	void markAsHole();
	bool shouldDelete() { return deleted; }
	bool checkBoundingBoxIntersection(Polygon other);
	
	std::vector< std::vector<Vector2f>> triangulateMonotone(Dcel* startEdge, Polygon*& newPoly);
	
	Vector2f sweepLocation = Vector2f(INFINITY,INFINITY);
	Vector2f boundingBox[2];
private:
	int n_points = 0;
	void traverseDcelList(std::vector<Dcel*> e);
	void copyArray(std::vector<Vector2f>& A, int start, int end, std::vector<Vector2f>& B);
	void SplitMerge(std::vector<Vector2f>& A, int start, int end, std::vector<Vector2f>& B);
	void merge(std::vector<Vector2f>& A, int start, int middle, int end, std::vector<Vector2f>& B);

	void copyArray(std::vector<Dcel*> A, int start, int end, std::vector<Dcel*>& B);
	void SplitMerge(std::vector<Dcel*> A, int start, int end, std::vector<Dcel*>& B);
	void merge(std::vector<Dcel*> A, int start, int middle, int end, std::vector<Dcel*>& B);
	void connectEdges(Dcel* edge1, Dcel* edge2, Dcel* newEdge1, Dcel* newEdge2);
	
	vertexType determineType(Dcel edge, bool debug = false);
	//Bstd* popBst(Bstd* root, Bstd& popped);
	//Bstd* findMin(Bstd* root);
	//Bstd* findPrev(Bstd* root, Dcel * edge);
	//Bstd* delBstX(Bstd* root, Dcel* edge);
	//Bstd* insertBstX(Bstd* root, Dcel* edge);
	void handleSplit(Dcel * edge, Bstd* &T);
	void handleStart(Dcel* edge, Bstd* &T);
	void handleEnd(Dcel* edge, Bstd* &T);
	void handleRegular(Dcel* edge, Bstd* &T);
	void handleMerge(Dcel* edge, Bstd* &T);

	void handleSplitL(Dcel* edge, Bstd*& T, std::vector<ise>& events);
	void handleStartL(Dcel* edge, Bstd*& T, std::vector<ise>& events);
	void handleEndL(Dcel* edge, Bstd*& T, std::vector<ise>& events);
	void handleRegularL(Dcel* edge, Bstd*& T, std::vector<ise>& events);
	void handleMergeL(Dcel* edge, Bstd*& T, std::vector<ise>& events);

	void handleSplitH(Dcel* edge, Bstd*& T);
	void handleStartH(Dcel* edge, Bstd*& T);
	void handleEndH(Dcel* edge, Bstd*& T);
	void handleRegularH(Dcel* edge, Bstd*& T);
	void handleMergeH(Dcel* edge, Bstd*& T);

	float crossProduct(Dcel* edge1, Dcel* edge2);
	void unionTwoEdges(Dcel* edge1, Dcel* edge2, Vector2f orig, Bstd* &T);
	void checkCollisionLeftRight(Dcel* edge, Bstd*& T, std::vector<ise>& events);
	void checkCollisionLeftRight2(Dcel* edge, Bstd*& T, std::vector<ise>& events);
	bool twoEdgesCollide(Dcel* edge1, Dcel* edge2, std::vector<ise>& events, Bstd* &T);
	void rebalanceTree(Bstd* T);
	void deleteHolesInHoles(int start, int end);

	void overLappingVertex(Bstd* T, std::vector<ise>& events);
	void overLappingVertexHelper(Bstd* T, Dcel* edge1, std::vector<ise>& events);
	bool deleted = false;
};