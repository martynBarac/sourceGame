#include "cbase.h"
#include "polygon.h"
#include <fstream>
#include <string>
#include <stack>
#include "globals.h"
void traverseDcel2(Dcel* edge, Dcel* startEdge)
{
	if (edge == startEdge)
	{
		std::cout << "done\n";
		return;
	}
	std::cout << edge << "\n";
	std::cout << edge->orig.x << ", " << edge->orig.y << "\n";
	traverseDcel2(edge->next, startEdge);
	return;
}
void Polygon::traverseDcel(Dcel* edge)
{
	std::cout << edge << "\n";
	std::cout << edge->orig.x << ", " << edge->orig.y << "\n";
	
	traverseDcel2(edge->next, edge);
	return;
}
void Polygon::traverseDcelList(std::vector<Dcel*> e)
{
	for (size_t i = 0; i < e.size(); i++)
	{
		Dcel * edge = e[i];
		std::cout << edge->orig.x << ", " << edge->orig.y << "\n";
	}
}

void Polygon::translate(Vector2f trans)
{
	for (size_t i = 0; i < edges.size(); i++)
	{
		edges[i]->orig.x += trans.x;
		edges[i]->orig.y += trans.y;
	}
	boundingBox[0].x += trans.x;
	boundingBox[0].y += trans.y;
	boundingBox[1].x += trans.x;
	boundingBox[1].y += trans.y;
}

bool compareY(Dcel* edge1, Dcel* edge2)
{
	return (edge1->orig.y > edge2->orig.y || (edge1->orig.y == edge2->orig.y) && (edge1->orig.x < edge2->orig.x));
}
bool compareY(Vector2f edge1, Vector2f edge2)
{
	return (edge1.y > edge2.y || ((edge1.y == edge2.y) && (edge1.x < edge2.x)));
}
// Find the next edge by travering around the left wall
// We know this edge will be part of the face because it connected to left wall
// https://github.com/Sixtisam/polygon_triangulation/tree/main
Dcel* findNextEdge(Dcel* helperEdge, Vector2f origin, Vector2f origin2, bool& edge1First)
{
	if (helperEdge->orig == origin)
	{
		return helperEdge;
	}
	
	bool holeEdge = helperEdge->helper->holeEdge;
	Dcel* prev;
	if (!holeEdge)
	{
		prev = helperEdge->prev;
		while (prev->orig != origin && prev != helperEdge)
		{
			if (prev->hasTwin)
			{
				edge1First = true;
			}
			prev = prev->prev;
			//std::cout << "BACKINGUP " << prev->orig << " " << prev->next->orig << "\n";
		}
	}
	else
		
	{
		prev = helperEdge->next;
		while (prev->orig != origin && prev != helperEdge)
		{
			if (prev->hasTwin)
			{
				edge1First = true;
			}
			prev = prev->next;
			//std::cout << "BACKINGUP " << prev->orig << " " << prev->next->orig << "\n";
		}
	}
	

	if (prev == helperEdge)
	{
		std::cout << "WTF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
		
	}
	return prev;
}

//TODO: Add twins
void Polygon::connectEdges(Dcel* edge1, Dcel* edge2, Dcel* newEdge1, Dcel* newEdge2)
{
	newEdge1->orig = edge1->orig;
	bool edge1First = false;
	//std::cout << "Connecting Edges: (" << edge1->orig << "), (" << edge2->helper->orig << ")\n";
	//std::cout << "Connecting Nbors (TYPE): (" << edge1->type << "), (" << edge2->helper->type << ")\n";
	//std::cout << "next: " << edge2->helper->next->orig << "\n";
	//std::cout << "Connecting Edges: (" << edge1->orig << "), (" << edge2->orig << ")\n";
	edge2 = findNextEdge(edge2, edge2->helper->orig, edge1->orig, edge1First);
	//edge2 = edge2->helper;
	//if (edge1First) std::swap(edge1, edge2);
	
	//std::cout << "Connecting Edges: (" << edge1->orig << "), (" << edge2->orig << ")\n";

	Dcel* edge2Prev = edge2->prev;

	newEdge1->prev = edge1->prev;
	edge1->prev->next = newEdge1;

	newEdge1->next = edge2;
	edge2->prev = newEdge1;


	newEdge2->orig = edge2->orig;

	newEdge2->prev = edge2Prev;
	edge2Prev->next = newEdge2;

	newEdge2->next = edge1;
	edge1->prev = newEdge2;


	newEdge1->twin = newEdge2;
	newEdge2->twin = newEdge1;

	newEdge2->hasTwin = true;
	newEdge1->hasTwin = true;

	start_edges.push_back(newEdge1);
	
	start_edges.push_back(newEdge2);
	edges.push_back(newEdge1);
	edges.push_back(newEdge2);
	return;
}

/*void Polygon::duplicate(Polygon p)
{
	Dcel* start = p.start_edge;
	Dcel* next = p.start_edge->next;
	points.push_back(Vector2f(float(start->orig.x), float(start->orig.y)));
	int num_points = 1;
	while (next != start)
	{
		num_points++;
		points.push_back( Vector2f(float(next->orig.x), float(next->orig.y)));
		next = next->next;
	}
	n_points = num_points;
	//start_edge = start;
}*/

void Polygon::duplicate(Polygon p)
{
	//std::cout << "duping!\n";
	int num_points = 0;
	if (p.start_edges.empty())
	{
		p.start_edges.push_back(p.start_edge);
	}
	for (size_t i = 0; i < p.start_edges.size(); i++)
	{
		//std::cout << "looping!\n";
		
		Dcel* start = p.start_edges[i];
		
		num_points++;
		Dcel* next = p.start_edges[i]->next;
		Dcel* prev = new Dcel();
		prev->orig = Vector2f(float(start->orig.x), float(start->orig.y));
		if (i == 0) start_edge = prev;
		Dcel* newStart = prev;
		points.push_back(Vector2f(float(start->orig.x), float(start->orig.y)));
		start_edges.push_back(prev);
		edges.push_back(prev);
		while (next != start)
		{
			num_points++;
			Vector2f point = Vector2f(float(next->orig.x), float(next->orig.y));
			points.push_back(point);
			Dcel* newEdge = new Dcel();
			newEdge->prev = prev;
			prev->next = newEdge;
			newEdge->orig = point;
			edges.push_back(newEdge);

			prev = newEdge;
			next = next->next;
			if (next == start)
			{
				newStart->prev = prev;
				prev->next = newStart;
			}
		}
		//std::cout << "TRAVERSING\n";
		//traverseDcel(start);
		//std::cout << "ueea\n";
	}
	n_points = num_points;
	
	//start_edge = start;
}

void Polygon::load(std::string path)
{
	std::ifstream polygon_file;
	std::string line;
	polygon_file.open(path, std::ios::out);
	points.clear();
	if (polygon_file.is_open())
	{
		while (std::getline(polygon_file, line))
		{
			
			int num_points = 0;
			bool in_tuple = false;
			bool in_ypos = false;
			std::string str_xpos = "";
			std::string str_ypos = "";
			for (size_t i = 0; i < line.length(); i++)
			{
				if (line[i] == *"(" )
				{
					num_points++;
					in_tuple = true;
				}
				else if (in_tuple)
				{
					
					if (line[i] == *",")
					{
						in_ypos = true;
					}

					else if (line[i] == *")")
					{
						in_tuple = false;
						in_ypos = false;
						float xpos = std::stof(str_xpos);
						float ypos = std::stof(str_ypos);
					
						Vector2f pt = Vector2f(xpos, ypos);
						points.push_back(pt);
						str_xpos = "";
						str_ypos = "";
					}
					else if (in_ypos)
					{
						str_ypos += line[i];
					}
					else
					{
						str_xpos += line[i];
					}
				}
			}
			//std::cout << "number of points = " << num_points << "\n";
			n_points = num_points;
		}
		polygon_file.close();
	}
	else std::cout << "cannot open\n";
}


void Polygon::generateDcel()
{
	// Now we have a list of vectors, convert this into a doubly conneted edgelist
	edges.clear();
	//edges.resize(n_points);
	struct Dcel* startEdge = new Dcel;
	struct Dcel* prevEdge = startEdge;
	struct Dcel* startEdgeT = new Dcel;
	struct Dcel* prevEdgeT = startEdgeT;
	float top = -INFINITY;
	float left = INFINITY;
	float right = -INFINITY;
	float bottom = INFINITY;
	for (uint8 i = 0; i < points.size() - 1; i++)
	{
		Vector2f prevPoint = points[i];
		Vector2f nextPoint = points[ (i + 1) % points.size()];
		struct Dcel* edge = new Dcel;
		edge->orig = nextPoint;
		prevEdge->next = edge;
		edge->prev = prevEdge;

		struct Dcel* edgeT = new Dcel;
		prevEdgeT->twin = prevEdge;
		prevEdge->twin = prevEdgeT;

		edgeT->orig = nextPoint;
		prevEdgeT->prev = edgeT;
		edgeT->next = prevEdgeT;


		//std::cout << " Prev   " << prevEdge->orig << " next  ";
		//std::cout << edge->orig << "\n";

		//std::cout << " PrevT   " << prevEdgeT->orig << " nextT  ";
		//std::cout << edgeT->orig << "\n";
		//std::cout << i << "\n";
		if (prevEdge->orig.x > right) right = prevEdge->orig.x;
		if (prevEdge->orig.x < left) left = prevEdge->orig.x;
		if (prevEdge->orig.y > top) top = prevEdge->orig.y;
		if (prevEdge->orig.y < bottom) bottom = prevEdge->orig.y;
		edges.push_back(prevEdge);
		prevEdge = edge;
		prevEdgeT = edgeT;

	}
	int i = points.size() - 1;
	//std::cout << i << "\n";
	Vector2f prevPoint = points[i];
	Vector2f nextPoint = points[(i + 1) % (points.size())];
	startEdge->orig = nextPoint;
	prevEdge->next = startEdge;
	startEdge->prev = prevEdge;

	prevEdgeT->twin = prevEdge;
	prevEdge->twin = prevEdgeT;

	prevEdgeT->prev = startEdgeT;
	startEdgeT->next = prevEdgeT;
	startEdgeT->orig = nextPoint;
	//std::cout << i << "\n";
	if (prevEdge->orig.x > right) right = prevEdge->orig.x;
	if (prevEdge->orig.x < left) left = prevEdge->orig.x;
	if (prevEdge->orig.y > top) top = prevEdge->orig.y;
	if (prevEdge->orig.y < bottom) bottom = prevEdge->orig.y;
	edges.push_back(prevEdge);
	//std::cout << " Prev   " << prevEdge->orig << " next  ";
	//std::cout << startEdge->orig << "\n";
	this->boundingBox[0] = Vector2f(left, top);
	this->boundingBox[1] = Vector2f(right, bottom);
	this->start_edge = startEdge;
	//std::cout << "STARTEDGE= " << start_edge->orig << "\n";
	//std::cout << "\ntraverseDCELList\n";
	//traverseDcelList(edges);
	//std::cout << "DONE\n";
	//std::cout << "\ntraverseDCEL\n";
	//traverseDcel(startEdge);
	
}

// Craete merge sort using https://en.wikipedia.org/wiki/Merge_sort

void Polygon::mergeSort(std::vector<Vector2f> & A, std::vector<Vector2f>& B)
{
	const int len = A.size();
	copyArray(A, 0, len, B);
	SplitMerge(B, 0, len, A);
}

void Polygon::SplitMerge(std::vector<Vector2f>& A, int start, int end, std::vector<Vector2f>& B)
{
	if (end - start <= 1)
		return;
	int middle = (start+end)/2;
	SplitMerge(B, start, middle, A);
	SplitMerge(B, middle, end, A);
	merge(A, start, middle, end, B);
}

void Polygon::merge(std::vector<Vector2f>& A, int start, int middle, int end, std::vector<Vector2f>& B)
{
	int i = start;
	int j = middle;
	for (int k=start; k<end;k++)
	{
		//CompareY??
		if (i < middle && (j >= end || compareY(A[i], A[j])))
		{
			B[k] = A[i];
			i += 1;
		}
		else
		{
			B[k] = A[j];
			j += 1;
		}
	}
}

void Polygon::copyArray(std::vector<Vector2f>& A, int start, int end, std::vector<Vector2f>& B)
{
	
	for (int i = start; i < end; i++)
	{
		B.push_back(A[i]);
	}
}

void Polygon::mergeSort(std::vector<Dcel*> pts, std::vector<Dcel*>& sortedPts)
{
	const int len = pts.size();
	/*std::vector<Dcel*> A;
	std::cout << "survived0 NOTHING\n";
	for (int i = 0; i < len; i++)
	{
		A.push_back(pts[i]);
	}*/
	copyArray(pts, 0, len, sortedPts);
	SplitMerge(pts, 0, len, sortedPts);
}

void Polygon::SplitMerge(std::vector<Dcel*> A, int start, int end, std::vector<Dcel*>& B)
{
	if (end - start <= 1)
		return;
	int middle = (start + end) / 2;
	SplitMerge(B, start, middle, A);
	SplitMerge(B, middle, end, A);
	merge(A, start, middle, end, B);
}

void Polygon::merge(std::vector<Dcel*> A, int start, int middle, int end, std::vector<Dcel*>& B)
{
	int i = start;
	int j = middle;
	for (int k = start; k < end; k++)
	{
		if (
			i < middle && (j >= end || compareY(A[i], A[j])))
		{
			B[k] = A[i];
			i += 1;
		}
		else
		{
			B[k] = A[j];
			j += 1;
		}
	}
}

void Polygon::copyArray(std::vector<Dcel*> A, int start, int end, std::vector<Dcel*>& B)
{
	for (int i = start; i < end; i++)
	{
		B.push_back(A[i]);
	}
}

void Polygon::sortEdgeList()
{
	std::vector<Dcel*> work;
	mergeSort(edges, work);
	edges = work;
}


Bstd * generateBstHelper(std::vector<Dcel*> A, int start, int end)
{
	if (end-start <=0)
	{
		return nullptr;
	}
	int middle = (start + end) / 2;
	Bstd* root = new Bstd;
	root->data = A[middle];
	root->left = generateBstHelper(A, start, middle);
	root->right = generateBstHelper(A, middle+1, end);
	return root;
}

void Polygon::generateBst()
{
	Q = generateBstHelper(edges, 0, edges.size());
}
void Polygon::generateBst(int start, int end)
{
	Q = generateBstHelper(edges, start, end);
}
void Polygon::handleSplitH(Dcel* edge, Bstd*& T)
{
	sweepLocation = edge->orig;
	rebalanceTree(T);

	insertBstX(T, edge->prev, &sweepLocation);
	insertBstX(T, edge, &sweepLocation);
}

void Polygon::handleStartH(Dcel* edge, Bstd*& T)
{
	sweepLocation = edge->orig;
	rebalanceTree(T);
	insertBstX(T, edge->prev, &sweepLocation);
	insertBstX(T, edge, &sweepLocation);
}

void Polygon::handleEndH(Dcel* edge, Bstd*& T)
{
	
	
	T = delBstX(T, edge->prev, &sweepLocation);
	//rebalanceTree(T);
	T = delBstX(T, edge, &sweepLocation);
	sweepLocation = edge->orig;
}

void Polygon::handleRegularH(Dcel* edge, Bstd*& T)
{

	if (edge->prev->orig.y < edge->orig.y || (edge->prev->orig.y == edge->orig.y && edge->next->orig.y > edge->orig.y))
	{
		//std::cout << "LEFT WALL\n";
		
		sweepLocation = edge->orig;
		
		T = delBstX(T, edge, &sweepLocation);
		rebalanceTree(T);
		insertBstX(T, edge->prev, &sweepLocation);

	}
	//Right wall
	else
	{
		sweepLocation = edge->orig;
		
		T = delBstX(T, edge->prev, &sweepLocation);
		rebalanceTree(T);
		insertBstX(T, edge, &sweepLocation);
	}
}

void Polygon::handleMergeH(Dcel* edge, Bstd*& T)
{

	T = delBstX(T, edge, &sweepLocation);
	T = delBstX(T, edge->prev, &sweepLocation);
	sweepLocation = edge->orig;
}

void Polygon::markAsHole()
{
	for (size_t i = 0; i < edges.size(); i++)
	{
		edges[i]->holeEdge = true;
	}
}

bool Polygon::checkBoundingBoxIntersection(Polygon other)
{
	// If top is below bottom its false

	if (boundingBox[0].y < other.boundingBox[1].y) return false;
	
	//Left is greater than right
	if (boundingBox[0].x > other.boundingBox[1].x) return false;

	if (boundingBox[1].x < other.boundingBox[0].x) return false;
	if (boundingBox[1].y > other.boundingBox[0].y) return false;
//	std::cout << "IN BOUNDING BOX!\n";
	return true;
}
void Polygon::deleteHolesInHoles(int start, int end)
{
	int i = start;
	Bstd* T = new Bstd(); // Counter clockwise
	Bstd* U = new Bstd(); // Clockwise
	while (i < end)
	{
		Dcel * thisEdge = edges[i];
		//std::cout << "THISEDGE = " << thisEdge << "\n";
		//std::cout << "THISEDGEMARKED = " << thisEdge->marked << "\n";
		//std::cout << "Tsize = " << T->data << "\n";


		//countCounterClock = countCounterClock % 2;
		//countClock = countClock % 2;
		thisEdge->inside = false;

		if (thisEdge->direction == cCW)
		{
			
			//For ccw edges
			
			Vector2f oldSweep = Vector2f( sweepLocation.x, sweepLocation.y);
			sweepLocation = thisEdge->orig;
			//printLeftRight(T);
			vertexType type = determineType(*thisEdge, false);
			int countCounterClock = 0;
			
			switch (type)
			{
			case splitv:
				break;
			case mergev:
				handleMergeH(thisEdge, T);
			//	countCounterClock += 1;
				break;
			case startv:
				countCounterClock += 1;
				break;
			case endv:
				handleEndH(thisEdge, T);
				countCounterClock += 1;
				break;
			case regularv:
				if (thisEdge->prev->orig.y < thisEdge->orig.y || (thisEdge->prev->orig.y == thisEdge->orig.y && thisEdge->next->orig.y > thisEdge->orig.y))
				{
					countCounterClock += 1;
					T = delBstX(T, thisEdge, &sweepLocation);
				}
				else
				{
					T = delBstX(T, thisEdge->prev, &sweepLocation);
				}
				break;
			default:
	
				break;
			}
			countCounterClock += countAll(T, thisEdge, &sweepLocation);
			int countClock = countAll(U, thisEdge, &sweepLocation);
			//std::cout << countClock << "\n";
			if (countCounterClock % 2 == 1 || countClock%2 ==  0)
			{
				//std::cout <<"inside type = " << thisEdge->type << "\n";
				thisEdge->visited = false;
				thisEdge->inside = true;
			}
			
			switch (type)
			{
			case splitv:
				//std::cout << "Handling split\n";
				handleSplitH(thisEdge, T);
				break;
			case mergev:
				//std::cout << "Handling merge\n";
				
				break;
			case startv:
				//std::cout << "Handling start\n";
				handleStartH(thisEdge, T);
				break;
			case endv:
				//std::cout << "Handling end\n";
				
				break;
			case regularv:
				//std::cout << "Handling regular\n";
				if (thisEdge->prev->orig.y < thisEdge->orig.y || (thisEdge->prev->orig.y == thisEdge->orig.y && thisEdge->next->orig.y > thisEdge->orig.y))
				{
					//std::cout << "LEFT WALL\n";

					//sweepLocation = thisEdge->orig;

					
					rebalanceTree(T);
					insertBstX(T, thisEdge->prev, &sweepLocation);

				}
				//Right wall
				else
				{
					//sweepLocation = thisEdge->orig;

					
					rebalanceTree(T);
					insertBstX(T, thisEdge, &sweepLocation);
				}
				break;
			default:
				//std::cout << "Handling def\n";
				break;
			}
			//std::cout << "NEXT VERTEX\n\n";
			sweepLocation = oldSweep;
		}
		else if (thisEdge->direction == cW)
		{
			//sweepLocation = thisEdge->orig;
			//std::cout << countClock - countCounterClock << " cW \n";
			//For clockwise edges
			//printLeftRight(U);
			vertexType type = determineType(*thisEdge, false);
			switch (type)
			{
			case splitv:
				//std::cout << "Handling split\n";
				handleSplitH(thisEdge, U);
				break;
			case mergev:
				//std::cout << "Handling merge\n";
				handleMergeH(thisEdge, U);
				break;
			case startv:
				//std::cout << "Handling start\n";
				handleStartH(thisEdge, U);
				break;
			case endv:
				//std::cout << "Handling end\n";
				handleEndH(thisEdge, U);
				break;
			case regularv:
				//std::cout << "Handling regular\n";
				handleRegularH(thisEdge, U);
				break;
			default:
				//std::cout << "Handling def\n";
				break;
			}

			//std::cout << "count all\n";
			//
		}
		else
		{
		std::cout << "UNDEFINED EDGE DIRECTION !! \n";
		}
		
		
		i++;
	}
	//std::cout << "OVERS\n";
	delete T;
	delete U;
}


void Polygon::addHole(Polygon &hole, float xoffset, float yoffset, bool ignoreHoles)
{
	std::vector<Dcel*> hole_edges = hole.edges;
	Bstd* T = new Bstd();
	Bstd* U = new Bstd();
	int top = 0;
	int bottom = edges.size() + hole_edges.size();
	Vector2f lowest;
	std::vector<Dcel*> new_edges;
	//Check if any of the hole is in main polygon
	size_t k = 0;
	size_t i = 0;
	size_t j = 0;
	bool polygonInside = false;
	bool polygonOutside = false;
	bool wallOutside = false;
	bool wallAllOut = false;
	//for (int i = 0; i < hole_edges.size(); i++)
	//{
	//	hole_edges[i]->orig.x += xoffset;
	//	hole_edges[i]->orig.y += yoffset;
	//}
	//Check bounding box collisions
	while (k < edges.size() + hole_edges.size())
	{
		//std::cout << "i = " << i << " j = " << j << " k = " << k << "\n";
		//std::cout << hole_edges.size() << "\n";
		if ((i < edges.size() && j>= hole_edges.size()) || (i < edges.size() &&
			compareY(edges[i], hole_edges[j])))
			
		{
			//We're in the polygon
			//Handle the type of vertex. Nothing to be done except adding to the stack.
			//Only count vertex that are not holes
			new_edges.push_back(edges[i]);
			if (edges[i]->holeEdge && ignoreHoles)
			{
				i += 1;
				k += 1;
				continue;
			}
			if (j == 0)
			{
				Vector2f lower;
				if (compareY(edges[i]->next, edges[i]))
				{
					lower = edges[i]->orig;
				}
				else
				{
					lower = edges[i]->next->orig;
				}

				if (compareY(lowest, lower))
				{
					top = k;
					lowest = lower;
				}
			}
			
			vertexType type = determineType(*edges[i], false);
			switch (type)
			{
			case splitv:
				//std::cout << "Handling split\n";
				handleSplitH(edges[i], T);
				break;
			case mergev:
				//std::cout << "Handling merge\n";
				handleMergeH(edges[i], T);
				break;
			case startv:
				//std::cout << "Handling start\n";
				handleStartH(edges[i], T);
				break;
			case endv:
				//std::cout << "Handling end\n";
				handleEndH(edges[i], T);
				break;
			case regularv:
				//std::cout << "Handling regular\n";
				handleRegularH(edges[i], T);
				break;
			default:
				//std::cout << "Handling def\n";
				break;
			}
			//std::cout << "NEXT\n\n";
			//printLeftRight(T);
			int count = 0;
			sweepLocation = edges[i]->orig;
			count = countAll(U, edges[i], &sweepLocation);
			if (count % 2 == 0)
			{
				wallOutside = true;
				
			}
			if (count % 2 == 1 && !ignoreHoles)
			{
				//edges[i]->inside = true;
				//std::cout << edges[i]->orig << " is inside\n";
				edges[i]->inside = true;
				wallAllOut = true;
			}
			else if (count % 2 == 1)
			{
				edges[i]->inside = true;
				//std::cout << edges[i]->orig << " is inside\n";
				//wallAllOut = true;
			}
			i += 1;
		}
		else
		{
			//Find the index of the top and bottom of the hole
			new_edges.push_back(hole_edges[j]);
			hole_edges[j]->marked = true;
			if (j == hole_edges.size() - 1)
			{

				bottom = k;
		
			}
			
			vertexType type = determineType(*hole_edges[j], false);
			switch (type)
			{
			case splitv:
				//std::cout << "Handling split\n";
				handleSplitH(hole_edges[j], U);
				break;
			case mergev:
				//std::cout << "Handling merge\n";
				handleMergeH(hole_edges[j], U);
				break;
			case startv:
				//std::cout << "Handling start\n";
				handleStartH(hole_edges[j], U);
				break;
			case endv:
				//std::cout << "Handling end\n";
				handleEndH(hole_edges[j], U);
				break;
			case regularv:
				//std::cout << "Handling regular\n";
				handleRegularH(hole_edges[j], U);
				break;
			default:
				//std::cout << "Handling def\n";
				break;
			}
			//std::cout << "NEXT\n\n";
			//printLeftRight(T);
			//We're at a hole
			//Count everything to the left of itself in the stack
			//If it is even, then we are out of the polygon
			// Find left
			int count = 0;
			sweepLocation = hole_edges[j]->orig;
			count = countAll(T, hole_edges[j], &sweepLocation);
			//std::cout << "count: " << count << "\n";
			if (count % 2 == 1)
			{
				//if (ignoreHoles) hole_edges[j]->inside = true;
				hole_edges[j]->inside = true;
				polygonInside = true;
			}
			else if(count % 2 == 0)
			{
				polygonOutside = true;
			}
			j += 1;
		}
		k++;
	}
	//if (i < edges.size()) wallOutside = true;
	//if (polygonInside)
	if (!wallOutside && !polygonInside) 
	{
		std::cout << "Wall inside \n";
		deleted = true;

	}
	/*if (polygonInside && !polygonOutside && wallOutside && !ignoreHoles)
	{
		std::cout << "EVIL CASE!\n";
		for (int i = 0; i < hole_edges.size(); i++)
		{
			delete hole_edges[i];
		}
		for (int i = 0; i < edges.size(); i++)
		{
			edges[i]->inside = false;
		}
		hole.start_edge = nullptr;
		return;
	}*/
	
	//if (polygonInside) std::cout << "polygon inside\n";
	//if (j == 0) std::cout << "no checks\n";
	if (true)
	{
		start_holes.push_back(hole.start_edge);
		for (size_t i = 0; i < hole.start_edges.size(); i++)
		{
			//hole_edges[i]->holeEdge = true;
			start_holes.push_back(hole.start_edges[i]);
			//std::cout << "added start hole\n";
		}
		
		edges.resize(new_edges.size());
		//for (int i = 0; i < hole_edges.size(); i++)
	//	{
	//		//hole_edges[i]->holeEdge = true;
	//		edges.push_back(hole_edges[i]);
	//	}
		
		//new_edges.resize(edges.size());
		//merge(edges, 0, middle, edges.size(), new_edges);
		for (size_t i = 0; i < edges.size(); i++)
		{
			edges[i] = new_edges[i];
		}
		edges = new_edges;
	}
	else
	{
		//std::cout << "POLYGON WAS OUTSIDE ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
	}
	
	
	delete T;
	delete U;
	generateBst(top, bottom);
}

//Calcs the winding number
void Polygon::detectHole()
{

}

vertexType Polygon::determineType(Dcel edge, bool debug)
{
	Vector2f thisPoint = edge.orig;
	Vector2f prevPoint = edge.prev->orig;
	Vector2f nextPoint = edge.next->orig;
	if (debug)std::cout << "determiing type!\n Prev: " << prevPoint << "\n This: " << thisPoint << "\n next: " << nextPoint << "\n";
	float cp = edge.orig.cross(prevPoint, nextPoint);
	
	if (cp >= 0)
	{
		if (compareY(thisPoint, prevPoint) && compareY(thisPoint, nextPoint))
		{
			return startv;
		}
		else if (compareY(prevPoint, thisPoint) && compareY(nextPoint, thisPoint))
		{
			return endv;
		}
		else
		{
			return regularv;
		}
	}
	else if (cp < 0)
	{
		if (compareY(thisPoint , prevPoint) && compareY(thisPoint, nextPoint))
		{
			return splitv;
		}
		else if (compareY(prevPoint, thisPoint) && compareY(nextPoint, thisPoint))
		{
			return mergev;
		}
		else
		{
			return regularv;
		}
	}
	else
	{
		return regularv;
	}
	return regularv;
}

void Polygon::handleSplit(Dcel * edge, Bstd* &T)
{
	Bstd* leftEdgeRoot = findPrev(T, edge, &sweepLocation);
	insertBstX(T, edge->prev, &sweepLocation);
	edge->prev->helper = edge;
	if (leftEdgeRoot != nullptr)
	{
		//std::cout << "found left edge \n";
		Dcel* leftEdge = leftEdgeRoot->data;
		if (leftEdge->helper != nullptr)
		{
			//std::cout << "found left edge2 \n";
			Dcel* newEdge1 = new Dcel;
			Dcel* newEdge2 = new Dcel;
			connectEdges(edge, leftEdge, newEdge1, newEdge2);
		}
		leftEdge->helper = edge;
	}
	//std::cout << edge->prev->orig << "set helper!\n";
	
}

void Polygon::handleStart(Dcel* edge, Bstd* &T)
{
	insertBstX(T, edge->prev, &sweepLocation);
	edge->prev->helper = edge;
}

void Polygon::handleEnd(Dcel* edge, Bstd* &T)
{
	if (edge->helper != nullptr)
	{
		
		if (edge->helper->type == mergev)
		{
			//std::cout << "CONNECTSU\n";
			Dcel* newEdge1 = new Dcel;
			Dcel* newEdge2 = new Dcel;
			connectEdges(edge, edge, newEdge1, newEdge2);
		}
	}
	//std::cout << edge->orig <<"  "<< edge->next->orig<<"  BEFOREDEL\n";
	T = delBstX(T, edge, &sweepLocation);
	//std::cout << "AFTERDEL\n";
}

void Polygon::handleRegular(Dcel* edge, Bstd* &T)
{
	//Left wall
	//std::cout << "left wall\n";
	// Check is broken
	if (edge->prev->orig.y < edge->orig.y || (edge->prev->orig.y == edge->orig.y && edge->next->orig.y > edge->orig.y))
	{
		insertBstX(T, edge->prev, &sweepLocation);
		//std::cout << "left wall\n";
		edge->prev->helper = edge;
		if (edge->helper != nullptr)
		{
			if (edge->helper->type == mergev)
			{
				Dcel* newEdge1 = new Dcel;
				Dcel* newEdge2 = new Dcel;
				connectEdges(edge, edge, newEdge1, newEdge2);
			}
		}
		T = delBstX(T, edge, &sweepLocation);
		
		
		
	}
	//Right wall
	else
	{
		Bstd* leftEdgeRoot = findPrev(T, edge, &sweepLocation);
		if (leftEdgeRoot != nullptr)
		{
			Dcel* leftEdge = leftEdgeRoot->data;
			//std::cout << leftEdge << "\n";
			if (leftEdge->helper != nullptr)
			{
				if (leftEdge->helper->type == mergev)
				{
					Dcel* newEdge1 = new Dcel;
					Dcel* newEdge2 = new Dcel;
					connectEdges(edge,  leftEdge, newEdge1, newEdge2);
				}
			}
			leftEdge->helper = edge;
		}
	}
}

void Polygon::handleMerge(Dcel* edge, Bstd* &T)
{
	
	T = delBstX(T, edge, &sweepLocation);
	//printLeftRight(T);
	Bstd* leftEdgeRoot = findPrev(T, edge, &sweepLocation);
	if (leftEdgeRoot != nullptr)
	{
		Dcel* leftEdge = leftEdgeRoot->data;
		if (leftEdge->helper != nullptr && leftEdge->helper != edge->helper)
		{
			if (leftEdge->helper->type == mergev)
			{
				Dcel* newEdge1 = new Dcel;
				Dcel* newEdge2 = new Dcel;

				connectEdges(edge, leftEdge, newEdge1, newEdge2);
				
			}
		}
		
		
		
		leftEdge->helper = edge;
		
		//std::cout << leftEdge->orig << " merge added helper\n";
	}
	if (edge->helper != nullptr)
	{
		if (edge->helper->type == mergev)
		{

			Dcel* newEdge1 = new Dcel;
			Dcel* newEdge2 = new Dcel;
			connectEdges(edge, edge, newEdge1, newEdge2);

		}
	}
}


void Polygon::makeMonotone()
{
	// Initialise the status T
	Bstd* T = new Bstd();
	while (Q != nullptr)
	{
		Bstd v = popBst(Q);
		//printLeftRight(T);
		if (v.data != nullptr)
		{
			//std::cout << v.data->orig << "\n";
			vertexType type = determineType(*v.data, false);
			sweepLocation = v.data->orig;
			v.data->type = type;
			switch (type)
			{
			case splitv:
				//std::cout << "Handling split\n";
				handleSplit(v.data, T);
				break;
			case mergev:
				//std::cout << "Handling merge\n";
				handleMerge(v.data, T);
				break;
			case startv:
				//std::cout << "Handling start\n";
				handleStart(v.data, T);
				break;
			case endv:
				//std::cout << "Handling end\n";
				handleEnd(v.data, T);
				break;
			case regularv:
				//std::cout << "Handling regular\n";
				handleRegular(v.data, T);
				break;
			default:
				//std::cout << "Handling def\n";
				break;
			}
		}
		//std::cout << "\n\n NEXT VERTEX!\n";
		//if (stat == nullptr) break;
	}
	//None of the edges are holes after monotoning it
	//for (int i = 0; i < edges.size(); i++)
	//{
	//	edges[i]->holeEdge = false;
	//}
}

std::vector< std::vector<Vector2f>> Polygon::triangulateMonotone(Dcel* startEdge,  Polygon* &newPoly)
{
	//std::cout << startEdge->orig << " START\n";
	std::vector< std::vector<Vector2f>> triangles;
	if (startEdge->triangulated)
	{
		//std::cout << startEdge << " Already triangulated\n";
		return triangles;
	}
	//This part is not yet triangulated.
	//Means this segment can be a new polygon
	newPoly = new Polygon();
	newPoly->start_edge = startEdge;
	// First find top and bottom verts
	// We can assume no split or merge vertex
	// Look for a single start and single end
	Dcel* curr = startEdge->next;
	Dcel* top = nullptr;
	Dcel* bottom = nullptr;
	float left = INFINITY;
	float right = -INFINITY;
	int size = 0;
	while (true)
	{
		//If the edge was originally a start/end then it has to be top/bottom;
		//std::cout << curr->orig<< " looping\n";
		/*if (curr->next->type == startv && !curr->holeEdge)
		{
			top = curr->next;
		}
		else if (curr->next->type == endv && !curr->holeEdge)
		{
			bottom = curr->next;
		}*/
		///else
		//{
		curr->next->prev = curr;
		curr->prev->next = curr;
			Vector2f thisPoint = curr->next->orig;
			Vector2f prevPoint = curr->orig;
			Vector2f nextPoint = curr->next->next->orig;
			float cp = thisPoint.cross(prevPoint, nextPoint);
			//std::cout <<" prev: " << curr->prev->orig << " \n";
			if (compareY(thisPoint, prevPoint) && compareY(thisPoint, nextPoint) && cp>=0)
			{
				
				top = curr->next;
				//std::cout << "TOP " << top->orig << "\n";
			}
			else if (compareY(prevPoint, thisPoint) && compareY(nextPoint, thisPoint) && cp>=0)
			{
				bottom = curr->next;
			}
			if (thisPoint.x > right) right = thisPoint.x;
			if (thisPoint.x < left) left = thisPoint.x;
		//}
		curr->next->triangulated = true;
		size++;
		
		if (curr == startEdge) break;
		curr = curr->next;
		
	}
	if (bottom == nullptr || top == nullptr)
	{
		Dcel* printEdge = startEdge->next;
		while (true)
		{
			std::cout << printEdge->orig << "  ";
			if (printEdge == startEdge) break;
			printEdge = printEdge->next;
		}
		std::cout << " DID NOT FIND BOTTOM OR TOP\n";
		return triangles;
	}
	newPoly->boundingBox[0] = Vector2f(left, top->orig.y);
	newPoly->boundingBox[1] = Vector2f(right, bottom->orig.y);
	if (size <= 3)
	{
		
		//std::cout << "Already a triangle\n";
		std::vector<Vector2f> triangle;
		triangle.resize(3);
		triangle[0] = curr->orig;
		triangle[1] = curr->next->orig;
		triangle[2] = curr->next->next->orig;
		triangles.push_back(triangle);

		newPoly->edges.push_back(curr);
		newPoly->edges.push_back(curr->next);
		newPoly->edges.push_back(curr->next->next);
		newPoly->sortEdgeList();
		
		return triangles;
	}
	//Now we know what the left and right chains are, we can mergeSort them in linear time;

	std::vector<Dcel*> polygon;
	std::vector<Dcel*> leftChain;
	std::vector<Dcel*> rightChain;
	curr = top->next;
	int middle = 1;
	vertexType leftv = mergev; // 4
	vertexType rightv = splitv;// 8
	polygon.push_back(top);
	leftChain.push_back(top);
	top->type = leftv;
	//std::cout << "top = " << top->orig << "\n";
	//std::cout << "bot = " << bottom->orig << "\n";
	while (curr != bottom)
	{
		
		middle += 1;
		polygon.push_back(curr);
		curr->type = rightv;
		rightChain.push_back(curr);
		curr = curr->next;
	}
	curr = top->prev;
	while (curr != bottom)
	{
		
		polygon.push_back(curr);
		curr->type = leftv;
		leftChain.push_back(curr);
		curr = curr->prev;
	}
	bottom->type = leftv;
	polygon.push_back(bottom);
	leftChain.push_back(bottom);
	std::vector<Dcel*> sortedPolygon;
	sortedPolygon.resize(polygon.size());
	//std::cout << "unsorted\n";
	//for (int i = 0; i < polygon.size(); i++)
	//{
	//	std::cout << polygon[i]->orig << "\n";
	//}
	//std::cout << "middle ="<< polygon[middle]->orig << "\n";
	merge(polygon, 0, middle, polygon.size(), sortedPolygon);
	//std::cout << "\nSORTed\n";
	for (size_t i = 0; i < sortedPolygon.size(); i++)
	{
	//	std::cout << sortedPolygon[i]->orig << "\n";;
		newPoly->edges.push_back(sortedPolygon[i]);
	}
	std::stack<Dcel*> S;
	// Push p1, push p2
	S.push(sortedPolygon[0]);
	S.push(sortedPolygon[1]);
	for (size_t i = 2; i < sortedPolygon.size() - 1; i++)
	{
		//std::cout << "looping4\n";
		//If u_j and S.top lie of different chains then
		if (sortedPolygon[i]->type != S.top()->type)
		{
			while (S.size() > 0)
			{
				Dcel* v = S.top();
				S.pop();
				if (S.size() > 0)
				{
					//std::cout << "created triangle\n";
					std::vector<Vector2f> triangle;
					triangle.resize(3);
					triangle[0] = sortedPolygon[i]->orig;
					if (sortedPolygon[i]->type == rightv)
					{
						triangle[1] = v->orig;
						triangle[2] = S.top()->orig;
					}
					else
					{
						triangle[1] = v->orig;
						triangle[2] = S.top()->orig;
					}
					triangles.push_back(triangle);
				}
			}
			S.push(sortedPolygon[i - 1]);
			S.push(sortedPolygon[i]);
		}

		
		//If we lie on the same chain then we have to look up
		else
		{
			Dcel* temp = S.top();
			S.pop();
			Dcel* v = S.top();
			float cp = temp->orig.cross(sortedPolygon[i]->orig, v->orig);
			while (!S.empty() && ((cp > 0 && temp->type == leftv) || (cp < 0 && temp->type == rightv)))
			{
				
				
				//std::cout << "prev: " << sortedPolygon[i]->orig << "\n";
				//std::cout << "this: " << temp->orig << "\n";
				//std::cout << "next " << v->orig << "\n";
				//std::cout << "cp is: " << cp << "\n";
				//std::cout << "side is : " << temp->type << "\n";
				//std::cout << "created triangle2\n";
				std::vector<Vector2f> triangle;
				triangle.resize(3);
				triangle[0] = sortedPolygon[i]->orig;
				triangle[1] = temp->orig;
				triangle[2] = v->orig;
				triangles.push_back(triangle);
				temp = v;
				S.pop();
				if (!S.empty())
				{
					v = S.top();
					cp = temp->orig.cross(sortedPolygon[i]->orig, v->orig);
					
				}
			}
			S.push(temp);
			S.push(sortedPolygon[i]);
		}
	}
	Dcel* temp = S.top();
	S.pop();
	while (S.size() > 0)
	{
		//std::cout << "looping5\n";
		Dcel* v = S.top();
		//std::cout << "created triangle3\n";
		std::vector<Vector2f> triangle;
		triangle.resize(3);
		triangle[0] = bottom->orig;
		triangle[1] = v->orig;
		triangle[2] = temp->orig;
		triangles.push_back(triangle);
		temp = v;
		S.pop();
	}
	//std::cout << "done!!!!\n";
	return triangles;
}

// Two edges intersect assuing both in the stack

/*
def intersect(x1, x2, x3, x4, y1, y2, y3, y4):
	d = ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4))
	if (d == 0):
		return None, None
	t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / d
	u = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / d
	return t, u
*/

float Polygon::crossProduct(Dcel* edge1, Dcel* edge2)
{
	Vector2f v1 = edge1->next->orig - edge1->orig;
	Vector2f v2 = edge2->next->orig - edge2->orig;
	return (v1.x * v2.y) - (v1.y * v2.x);
}
void Polygon::unionTwoEdges(Dcel* edge1, Dcel* edge2, Vector2f orig, Bstd* &T)
{
	
	float cp = crossProduct(edge1, edge2);
	//Create a new edge at the intersection
	//New edge is between current edge and next edge
	Dcel* newEdge1 = new Dcel();
	Dcel* newEdge2 = new Dcel();
	newEdge1->holeEdge = edge1->holeEdge;
	newEdge2->holeEdge = edge2->holeEdge;
	newEdge1->inside = !edge1->inside;
	newEdge2->inside = !edge2->inside;
	newEdge1->marked = edge1->marked;
	newEdge2->marked = edge2->marked;
	//newEdge2->inside = true;
	//newEdge2->inside = true;
	//newEdge2->inside = false;
	//newEdge2->inside = false;
	//newEdge1->inside = edge1->inside;
	//newEdge2->inside = edge2->inside;
	newEdge1->orig = orig;
	newEdge2->orig = orig;

	//New edges connect to the next edge
	/*newEdge1->next = edge1->next;
	edge1->next->prev = newEdge1;
	newEdge2->next = edge2->next;
	edge2->next->prev = newEdge2;

	//Nee edges connect to current edges
	edge1->next = newEdge2;
	newEdge2->prev = edge1;
	edge2->next = newEdge1;
	newEdge1->prev = edge2;*/

	newEdge1->next = edge1->next;
	edge1->next->prev = newEdge1;
	newEdge2->next = edge2->next;
	edge2->next->prev = newEdge2;

	//Nee edges connect to current edges
	edge1->next = newEdge1;
	newEdge1->prev = edge1;
	edge2->next = newEdge2;
	newEdge2->prev = edge2;

	newEdge2->unionHelper = newEdge1;
	newEdge1->unionHelper = newEdge2;
	
	//Add to the stack if required
	//std::cout << "sweeploc: " << sweepLocation << "   newEdge1: " << newEdge1->orig << "   newEdge2: " << newEdge2->orig << "\n";
	//GOT Rid of the +EPSILON on X idk WHAT its for
	if (compareY(sweepLocation, newEdge1->orig))
	{
		
		insertBstY(Q, newEdge1);
		
	}

	
	if (compareY(sweepLocation, edge1->orig))
	{
		//Figure out which to add to status
		
		insertBstY(Q, edge1);
		delBstX(T, edge1, &sweepLocation);
		
		insertBstX(T, newEdge1, &sweepLocation);
		
	}
	if (compareY(sweepLocation, newEdge2->orig))
	{
		insertBstY(Q, newEdge2);
	}
	
	if (compareY(sweepLocation, edge2->orig))
	{
		//std::cout << "HERE\n";
		insertBstY(Q, edge2);
		delBstX(T, edge2, &sweepLocation);
		
		insertBstX(T, newEdge2, &sweepLocation);
		//std::cout << "lived\n";
	}
	
	edges.push_back(newEdge1);
	edges.push_back(newEdge2);

	//Conditions for edges within main polygon
	
	
	if (!edge1->holeEdge && edge2->inside)
	{
		start_edges.push_back(newEdge1);
	}
	else if (!edge1->holeEdge && cp > 0)
	{
		start_edges.push_back(newEdge1);
	}

	if (!edge2->holeEdge && edge1->inside)
	{
		start_edges.push_back(newEdge2);
	}
	else if (!edge2->holeEdge && cp < 0)
	{
		start_edges.push_back(newEdge2);
	}

	/*if (edge1->holeEdge && edge2->holeEdge)
	{
		if (newEdge2->next->inside || newEdge2->inside)
		{
			start_edges.push_back(newEdge1);
		}
		else
		{
			start_edges.push_back(newEdge2);
		}
	}*/
	//Conditions for edges within circle
	if (edge1->holeEdge && edge2->holeEdge && edge1->inside)
	{
		start_holes.push_back(newEdge2);
	}
	else if (edge1->holeEdge && edge2->holeEdge && cp < 0)
	{
		start_holes.push_back(newEdge2);
	}
	if (edge1->holeEdge && edge2->holeEdge && edge2->inside)
	{
		start_holes.push_back(newEdge1);
	}
	else if (edge1->holeEdge && edge2->holeEdge && cp > 0)
	{
		start_holes.push_back(newEdge1);
	}
	/*if (edge1->holeEdge && edge2->holeEdge)
	{
		start_holes.push_back(newEdge1);
		start_holes.push_back(newEdge2);
	}*/
	
}
bool Polygon::twoEdgesCollide(Dcel* edge1, Dcel* edge2, std::vector<ise> &events, Bstd* &T)
{
	float x1 = edge1->orig.x;
	float x2 = edge1->next->orig.x;
	float x3 = edge2->orig.x;
	float x4 = edge2->next->orig.x;

	float y1 = edge1->orig.y;
	float y2 = edge1->next->orig.y;
	float y3 = edge2->orig.y;
	float y4 = edge2->next->orig.y;
	float d = ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
	float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / d;
	float u = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / d;
	//std::cout << "  Checking " << edge1->orig << "  Against " << edge2->orig <<"  "<< edge2->next->orig << "\nt: " << t << "  u: " << u << "\n";
	//TODO: Handle cases where its overlapping
	//if ((edge1->orig.equals(edge2->orig) || edge1->next->orig.equals(edge2->next->orig))||
	//	(edge1->next->orig.equals(edge2->orig) || edge1->orig.equals(edge2->next->orig)))
	//{
	//	//std::cout << "overlap\n";
	//	return false;
	//}
	//if (t > EPSILON && t < 1- EPSILON && u > EPSILON && u < 1- EPSILON) 
	//This could get evil because of floating point cases.
	if (t >10*EPSILON && t < 1-10*EPSILON && u >10*EPSILON && u < 1-10*EPSILON)
	{
		
		
		float px = x1 + t * (x2 - x1);
		float py = y1 + t * (y2 - y1);
		ise event;
		event.edge1 = edge1;
		event.edge2 = edge2;
		event.orig = Vector2f(px, py);
		//std::cout << edge1->orig << "  COLLIDES WITH " << edge2->orig << "  Cross =" << (cp) << "  Orig =" << event.orig << "  Sweep =" << sweepLocation << "\n";
	//	std::cout << "t = " << t << "  u = " << (u) << "\n";
		events.push_back(event);
		unionTwoEdges(edge1, edge2, Vector2f(px, py), T);
		return true;
	}
	//Check colinear
	//float cp = edge1->orig.cross(edge2->orig, edge2->next->orig);
	//float cp2 = edge1->next->orig.cross(edge2->orig, edge2->next->orig);
	//float cp3 = edge2->orig.cross(edge1->orig, edge1->next->orig);
	//float cp4 = edge2->next->orig.cross(edge1->orig, edge1->next->orig);
	//std::cout << "  cp1 =" << cp << "  cp2 =" << (cp2) << "  cp3 =" << cp3 << "  cp4 =" << cp4 << "\n";
	return false;
}

void Polygon::checkCollisionLeftRight(Dcel* edge, Bstd*& T, std::vector<ise>& events)
{
	Bstd* leftEdgeRoot;
	leftEdgeRoot = findPrev(T, edge, &sweepLocation);
	//std::cout << "checking left\n";
	if (leftEdgeRoot != nullptr)
	{

		twoEdgesCollide(edge, leftEdgeRoot->data, events, T);
	}
	//std::cout << "checking right\n";
	Bstd* rightEdgeRoot;
	rightEdgeRoot = findNext(T, edge, &sweepLocation);
	if (rightEdgeRoot != nullptr)
	{
		twoEdgesCollide(edge, rightEdgeRoot->data, events, T);
	}
	
}

void Polygon::checkCollisionLeftRight2(Dcel* edge, Bstd*& T, std::vector<ise>& events)
{
	Bstd* leftEdgeRoot;
	leftEdgeRoot = findPrev(T, edge, &sweepLocation);
	Bstd* rightEdgeRoot;
	rightEdgeRoot = findNext(T, edge, &sweepLocation);
	if (leftEdgeRoot != nullptr)
	{
		if (rightEdgeRoot != nullptr)
		{
			twoEdgesCollide(leftEdgeRoot->data, rightEdgeRoot->data, events, T);
		}
	}
}

void Polygon::handleSplitL(Dcel* edge, Bstd*& T, std::vector<ise>& events)
{
	sweepLocation = edge->orig;
	
	rebalanceTree(T);
	Bstd* leftEdgeRoot;
	leftEdgeRoot = findPrev(T, edge, &sweepLocation);

	if (leftEdgeRoot != nullptr)
	{
		twoEdgesCollide(edge, leftEdgeRoot->data, events, T);
	}

	Bstd* rightEdgeRoot;
	rightEdgeRoot = findNext(T, edge->prev, &sweepLocation);

	if (rightEdgeRoot != nullptr)
	{
		twoEdgesCollide(edge->prev, rightEdgeRoot->data, events, T);
	}
	//rebalanceTree(T);
	
	insertBstX(T, edge, &sweepLocation);
	insertBstX(T, edge->prev, &sweepLocation);
}

void Polygon::handleStartL(Dcel* edge, Bstd*& T, std::vector<ise>& events)
{
	sweepLocation = edge->orig;
	rebalanceTree(T);
	Bstd* leftEdgeRoot;
	leftEdgeRoot = findPrev(T, edge->prev, &sweepLocation);

	if (leftEdgeRoot != nullptr)
	{
		twoEdgesCollide(edge->prev, leftEdgeRoot->data, events, T);
	}
	
	Bstd* rightEdgeRoot;
	rightEdgeRoot = findNext(T, edge, &sweepLocation);

	if (rightEdgeRoot != nullptr)
	{
		twoEdgesCollide(edge, rightEdgeRoot->data, events, T);
	}
	
	
	insertBstX(T, edge->prev, &sweepLocation);
	insertBstX(T, edge, &sweepLocation);
	
	
	
}

void Polygon::handleEndL(Dcel* edge, Bstd*& T, std::vector<ise>& events)
{
	
	T = delBstX(T, edge->prev, &sweepLocation);
	T = delBstX(T, edge, &sweepLocation);
	sweepLocation = edge->orig;
	rebalanceTree(T);
	checkCollisionLeftRight2(edge, T, events);
}

void Polygon::handleRegularL(Dcel* edge, Bstd*& T, std::vector<ise>& events)
{
	//Fix may not work but try this
	//if (edge->prev->orig.y < edge->orig.y || (edge->prev->orig.y == edge->orig.y && edge->next->orig.y > edge->orig.y))
	if (edge->prev->orig.y < edge->orig.y || (edge->prev->orig.y == edge->orig.y && edge->next->orig.y > edge->orig.y) || (edge->prev->orig.y == edge->orig.y && edge->prev->orig.x > edge->orig.x))

	{
		//std::cout << "LEFT WALL\n";
		
		//rebalanceTree(T);
		T = delBstX(T, edge, &sweepLocation);
		sweepLocation = edge->orig;
		rebalanceTree(T);
		checkCollisionLeftRight(edge->prev, T, events);
		checkCollisionLeftRight2(edge, T, events);
		
		insertBstX(T, edge->prev, &sweepLocation);
		
	}
	//Right wall
	else
	{
		
		
		//rebalanceTree(T);
		//sweepLocation = edge->prev->orig;
		T = delBstX(T, edge->prev, &sweepLocation);
		sweepLocation = edge->orig;
		rebalanceTree(T);
		checkCollisionLeftRight(edge, T, events);
		checkCollisionLeftRight2(edge->prev, T, events);
		
		insertBstX(T, edge, &sweepLocation);
	}
}

void Polygon::handleMergeL(Dcel* edge, Bstd*& T, std::vector<ise>& events)
{
	
	T = delBstX(T, edge, &sweepLocation);
	T = delBstX(T, edge->prev, &sweepLocation);
	//
	sweepLocation = edge->orig;
	rebalanceTree(T);
	checkCollisionLeftRight2(edge, T, events);
}

void Polygon::findIntersections()
{
	// Initialise the status T
	std::vector<ise> events;
	Bstd* T = new Bstd();
	while (Q != nullptr)
	{
		Bstd v = popBst(Q);
		//printLeftRight(T);
		if (v.data != nullptr)
		{
			//std::cout << v.data->orig << "\n";
			vertexType type = determineType(*v.data, false);
			v.data->type = type;
			switch (type)
			{
			case splitv:
				//std::cout << "Handling split\n";
				handleSplitL(v.data, T, events);
				break;
			case mergev:
				//std::cout << "Handling merge\n";
				handleMergeL(v.data, T, events);
				break;
			case startv:
				//std::cout << "Handling start\n";
				handleStartL(v.data, T, events);
				break;
			case endv:
				//std::cout << "Handling end\n";
				handleEndL(v.data, T, events);
				break;
			case regularv:
				//std::cout << "Handling regular\n";
				handleRegularL(v.data, T, events);
				break;
			default:
				//std::cout << "Handling def\n";
				break;
			}
		}
		//std::cout << "\n\n NEXT VERTEX!\n";
		//if (stat == nullptr) break;
	}
	//for (int i = 0; i < events.size(); i++)
	//{
	//	Dcel* edge1 = events[i].edge1;
	//	Dcel* edge2 = events[i].edge2;
	//	float cp = crossProduct(edge1, edge2);
		//std::cout << "Collision orig: " << events[i].orig << "  ";
		//std::cout << "Collision cp: " << cp << "\n";
		//unionTwoEdges(events[i].edge1, events[i].edge2, events[i].orig);
	//}

	//https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
	//std::cout << "FINDHOLES IN WALL\n";
	//If there is an even number of intersections perfrom union. Else there will be errorz.
	if (events.size() % 2 == 0)
	{
		for (size_t i = 0; i < edges.size(); i++)
		{
			if (edges[i]->unionHelper != nullptr)
			{
				Dcel* newEdge1 = edges[i];
				Dcel* newEdge2 = edges[i]->unionHelper;
				Dcel* edge1 = newEdge1->prev;
				Dcel* edge2 = newEdge2->prev;
				//Nee edges connect to current edges
				edge1->next = newEdge2;
				newEdge2->prev = edge1;
				edge2->next = newEdge1;
				newEdge1->prev = edge2;
				newEdge1->unionHelper = nullptr;
				newEdge2->unionHelper = nullptr;
				float cp1 = crossProduct(edge1, newEdge2);
				float cp2 = crossProduct(edge2, newEdge1);
				newEdge1->inside = cp1 <= 0;
				newEdge2->inside = cp2 <= 0;
				//newEdge1->holeEdge = cp1 >= 0;
				//newEdge2->holeEdge = cp2 >= 0;
				//newEdge1->inside = false;
				//newEdge2->inside = false;

			}
		}
	}
	else
	{
		std::cout << "failsafe activated\n";
		for (size_t i = 0; i < edges.size(); i++)
		{
			if (edges[i]->unionHelper != nullptr)
			{
				Dcel* newEdge1 = edges[i];
				Dcel* newEdge2 = edges[i]->unionHelper;
				newEdge1->unionHelper = nullptr;
				newEdge2->unionHelper = nullptr;
			}
		}
		return;
	}
	

	size_t i = 0;
	
	start_edges.push_back(start_edge);
	
	//std::cout << "FINDHOLES IN WALL1\n";
	while (i < start_edges.size())
	{
		Dcel* start = start_edges[i];
		Dcel* prev = start;
		Dcel* next = start_edges[i]->next;
		//std::cout << "startEdge = " << start_edges[i]->orig << "\n";
		
		//if (startedHole) continue;
		
		Dcel* lowestPoint = nullptr;
		while (!next->visited)
		{

			if (lowestPoint == nullptr)
			{
				lowestPoint = next;
			}
			else if (next->orig.y < lowestPoint->orig.y)
			{
				lowestPoint = next;
			}
			//Delete any invalid holes to avoid crashes!
			//if (next->holeEdge && !next->inside)
			//{
			//	start_edges.erase(start_edges.begin() + i);
			//	i -= 1;
			//	break;
			//}
			if (next->inside && ! next->holeEdge)
			{
				start_edges.erase(start_edges.begin() + i);
				i -= 1;
				break;
			}
			if (next == start)
			{
				//If the area is valid, go back around and visit everything
				Vector2f thisPoint = lowestPoint->orig;
				Vector2f prevPoint = lowestPoint->prev->orig;
				Vector2f nextPoint = lowestPoint->next->orig;
				//std::cout << "determiing type!\n Prev: " << prevPoint << "\n This: " << thisPoint << "\n next: " << nextPoint << "\n";
				float cp = lowestPoint->orig.cross(prevPoint, nextPoint);

				if (cp > 0)
				{
					Dcel* next2 = next->next;
					while (!next2->visited && next2 != start)
					{
						next2->visited = true;
						next2->direction = cW;
						next2->triangulated = false;
						next2 = next2->next;
					}
					start->direction = cW;
					start->visited = true;
					break;
				}
				else
				{
					if (cp == 0)
					{
						std::cout << "conundrum\n";
					}
					//std::cout << "\nREAZED!\n";
					//std::cout << cp << "\n";

					start_edges.erase(start_edges.begin() + i);
					i -= 1;
				}
				//start->visited = true;
				break;
			}
			next = next->next;
			prev = prev->next;
		}
		i++;
	}
	
	//Check if holes are good
	//std::cout << "FINDHOLES IN WALL2\n";
	for (size_t i = 0; i < start_holes.size(); i++)
	{
		Dcel* start = start_holes[i];
		Dcel* prev = start;
		Dcel* next = start_holes[i]->next;
		Dcel* lowestPoint = nullptr;
		//std::cout << "startHole = " << start_holes[i]->orig << "\n";
		//Run around the hole, if we dont hit anything on the way, set to visited
		while (!next->visited)
		{
			//If we run into an edge that is not a hole then its invalid.
			if (!next->holeEdge)
			{
				break;
			}
			if (!next->inside)
			{
				break;
			}
			
			
			
			
			if (lowestPoint == nullptr)
			{
				lowestPoint = next;
			}
			else if (next->orig.y < lowestPoint->orig.y)
			{
				lowestPoint = next;
			}

			if (next == start)
			{
				//If the area is valid, go back around and visit everything
				Vector2f thisPoint = lowestPoint->orig;
				Vector2f prevPoint = lowestPoint->prev->orig;
				Vector2f nextPoint = lowestPoint->next->orig;
				//std::cout << "determiing type!\n Prev: " << prevPoint << "\n This: " << thisPoint << "\n next: " << nextPoint << "\n";
				float cp = lowestPoint->orig.cross(prevPoint, nextPoint);
			
				//if (cp < 0)
				if (true)
				{
					//We made it through 1 loop so now we confirm it is a real hole.
					Dcel* next2 = next->next;
					while (!next2->visited && next2 != start)
					{
						//Mark as visited so that they are not deleted
						
						if (cp < 0)
						{
							next2->visited = true;
							next2->direction = cCW;
						}

						else next2->direction = cW;
						next2->triangulated = false;
						next2 = next2->next;
					}
					
					
					if (cp < 0)
					{
						start_edges.push_back(start_holes[i]);
						start->direction = cCW;
						start->visited = true;
					}
					else start->direction = cW;
					break;
				}
				else if (cp == 0)
				{
					std::cout << "conundrum\n";
					
				}
				break;
			}
			next = next->next;
			prev = prev->next;
		}
	}
	
	
	i = 0;
	while( i < edges.size())
	{
		//std::cout << "CHECKING :" << edges[i]->orig << "  " << edges[i]->holeEdge << "\n";
		if (!edges[i]->visited)
		{
			Dcel* edge = edges[i];
			//std::cout << "DELETED " << edge->orig << "\n";
			
			if (edge == start_edge)
			{
				if (i < edges.size() - 1)
				{
					start_edge = edges[i + 1];
				}
				else if (start_edges.size() > 0)
				{
					start_edge = nullptr;
					for (size_t j = 0; j < start_edges.size(); j++)
					{
						if (start_edge != start_edges[j])
						{
							
							break;
						}
					}
					if (start_edge == nullptr)
					{
						std::cout << "MEGA WARNING2 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!||||||||$$$$$$$&^#&%@&#^%&^#%&^@#%&^#%&^@#&^#%#@^&^\n";
					}
					
				}
				else
				{
					std::cout << "MEGA WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!||||||||$$$$$$$&^#&%@&#^%&^#%&^@#%&^#%&^@#&^#%#@^&^\n";
					start_edge = nullptr;
				}
				
			}
			edges.erase(edges.begin() + i);
			delete edge;
			i -= 1;
		}
		else
		{
			
			//edges[i]->visited = false;
		}
		
		i += 1;
	}
	for (size_t i = 0; i < start_edges.size(); i++)
	{
		//std::cout << start_edges[i]->direction <<" checking\n";
		if (start_edges[i]->direction == cCW && start_edges[i]->holeEdge) std::cout << "hole detected\n";
	}
	sortEdgeList();
	deleteHolesInHoles(0, edges.size());
	//std::cout << "FINDHOLES IN WALL3\n";
	i = 0;
	while (i < start_edges.size())
	{
		if (!start_edges[i]->visited)
		{
			start_edges.erase(start_edges.begin() + i);
			i--;
		}
		i++;
	}
	i = 0;
	while (i < edges.size())
	{
		//std::cout << "CHECKING :" << edges[i]->orig << "  " << edges[i]->holeEdge << "\n";
		if (!edges[i]->visited)
		{
			Dcel* edge = edges[i];
			//std::cout << "DELETED " << edge->orig << "\n";

			if (edge == start_edge)
			{
				if (i < edges.size() - 1)
				{
					start_edge = edges[i + 1];
				}
				else if (start_edges.size() > 0)
				{
					start_edge = nullptr;
					for (size_t j = 0; j < start_edges.size(); j++)
					{
						if (start_edge != start_edges[j])
						{
							
							break;
						}
					}
					if (start_edge == nullptr)
					{
						std::cout << "MEGA WARNING2 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!||||||||$$$$$$$&^#&%@&#^%&^#%&^@#%&^#%&^@#&^#%#@^&^\n";
					}

				}
				else
				{
					std::cout << "MEGA WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!||||||||$$$$$$$&^#&%@&#^%&^#%&^@#%&^#%&^@#&^#%#@^&^\n";
					start_edge = nullptr;
				}

			}
			edges.erase(edges.begin() + i);
			delete edge;
			i -= 1;
		}
		else
		{

			//edges[i]->visited = false;
		}

		i += 1;
	}
	//std::cout << "FINDHOLES IN WALL4\n";
	//std::cout << "SE: " << start_edge << "\n";
	//std::cout << "SE orig: " << start_edge->orig << "\n";
	//std::cout << "SE size " << start_edges.size() << "\n";
}
void Polygon::findIntersectionsHoles()
{
	// Initialise the status T
	bool debugMode = false;
	std::vector<ise> events;
	Bstd* T = new Bstd();
	while (Q != nullptr)
	{
		Bstd v = popBst(Q);
		if (debugMode) printLeftRight(T);
		if (v.data != nullptr)
		{
			//std::cout << v.data->orig << "\n";
			vertexType type = determineType(*v.data, debugMode);
			v.data->type = type;
			switch (type)
			{
			case splitv:
				if (debugMode) std::cout << "Handling split\n";
				handleSplitL(v.data, T, events);
				break;
			case mergev:
				if (debugMode) std::cout << "Handling merge\n";
				handleMergeL(v.data, T, events);
				break;
			case startv:
				if (debugMode) std::cout << "Handling start\n";
				handleStartL(v.data, T, events);
				break;
			case endv:
				if (debugMode) std::cout << "Handling end\n";
				handleEndL(v.data, T, events);
				break;
			case regularv:
				if (debugMode) std::cout << "Handling regular\n";
				handleRegularL(v.data, T, events);
				break;
			default:
				//std::cout << "Handling def\n";
				break;
			}
		}
		if (debugMode) std::cout << "\n\n NEXT VERTEX!\n";
		//if (stat == nullptr) break;
	}

	//https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
	//std::cout << "FINDHOLES IN WALL\n";
	//If there is an even number of intersections perfrom union. Else there will be errorz.
	//std::cout << "events.size() " << events.size() << "\n";
	bool superwarn = false;
	if (events.size() % 2 == 0)
	{
		for (size_t i = 0; i < edges.size(); i++)
		{
			if (edges[i]->unionHelper != nullptr)
			{
				Dcel* newEdge1 = edges[i];
				Dcel* newEdge2 = edges[i]->unionHelper;
				Dcel* edge1 = newEdge1->prev;
				Dcel* edge2 = newEdge2->prev;
				//Nee edges connect to current edges
				edge1->next = newEdge2;
				newEdge2->prev = edge1;
				edge2->next = newEdge1;
				newEdge1->prev = edge2;
				newEdge1->unionHelper = nullptr;
				newEdge2->unionHelper = nullptr;
				float cp1 = crossProduct(edge1, newEdge2);
				float cp2 = crossProduct(edge2, newEdge1);
				newEdge1->inside = cp1 <= 0;
				newEdge2->inside = cp2 <= 0;
				//std::cout << "cp1 = " << cp1 << "\n";
				//std::cout << "cp2 = " << cp2 << "\n";
			}
		}
	}
	else
	{
		std::cout << "SUPER WARNING!\n";
		superwarn = true;
		for (size_t i = 0; i < edges.size(); i++)
		{
			if (edges[i]->unionHelper != nullptr)
			{
				Dcel* newEdge1 = edges[i];
				Dcel* newEdge2 = edges[i]->unionHelper;
				newEdge1->unionHelper = nullptr;
				newEdge2->unionHelper = nullptr;
				// Delete the hole thats invalid
				//newEdge1->inside = newEdge1->marked;
				//newEdge2->inside = newEdge2->marked;
				newEdge1->inside = false;
				newEdge2->inside = false;
				//return;
			}
		}
	}

	size_t i = 0;

	start_edges.push_back(start_edge);
	while (i < start_edges.size())
	{
		Dcel* start = start_edges[i];
		Dcel* prev = start;
		Dcel* next = start_edges[i]->next;
		
		
		Dcel* lowestPoint = nullptr;
		while (true)
		{
			if (next->inside || next->visited)
			{
				start_edges.erase(start_edges.begin() + i);
				i -= 1;
				break;
			}
			if (lowestPoint == nullptr)
			{
				lowestPoint = next;
			}
			
			else if (compareY(lowestPoint, next))
			{
				lowestPoint = next;
			}
			
			if (next == start)
			{
				//If the area is valid, go back around and visit everything
				Vector2f thisPoint = lowestPoint->orig;
				Vector2f prevPoint = lowestPoint->prev->orig;
				Vector2f nextPoint = lowestPoint->next->orig;
				//std::cout << "determiing type!\n Prev: " << prevPoint << "\n This: " << thisPoint << "\n next: " << nextPoint << "\n";
				float cp = lowestPoint->orig.cross(prevPoint, nextPoint);
				if (cp < 0)
				//if (true)
				{
					Dcel* next2 = next->next;
					while (!next2->visited && next2 != start)
					{
						next2->visited = true;
						next2->triangulated = false;
						next2 = next2->next;
					}
					start->visited = true;
					break;
				}
				else
				{
					/*std::cout << "\nstarting this!\n";
					Dcel* next2 = next->next;
					while (!next2->visited && next2 != start)
					{
						
						next2->marked = true;
						next2 = next2->next;
					}
					start->marked = true;
					
					//Here we check inside marked holes
					std::cout << "\n3\n";
					deleteHolesInHoles(0, edges.size());

					std::cout << "\n2\n";
					next2 = next->next;
					while (!next2->visited && next2 != start)
					{
						next2->marked = false;
						next2 = next2->next;
					}
					start->marked = false;
					*/
					//std::cout << "\nREAZED!\n";
					//std::cout << cp << "\n";

					start_edges.erase(start_edges.begin() + i);
					i -= 1;
				}
				//start->visited = true;
				break;
			}
			next = next->next;
			prev = prev->next;
		}
		i++;
	}
	
	//Check if holes are good
	//std::cout << "FINDHOLES IN WALL2\n";
	for (size_t i = 0; i < start_holes.size(); i++)
	{
		
		Dcel* start = start_holes[i];
		Dcel* prev = start;
		Dcel* next = start_holes[i]->next;
		Dcel* lowestPoint = nullptr;
		
		//std::cout << "startHole = " << start_holes[i]->orig << "\n";
		//Run around the hole, if we dont hit anything on the way, set to visited
		//std::cout << next << "\n";
		
		while (true)
		{
			//If we run into an edge that is not a hole then its invalid.
			//if (!next->holeEdge)
			//{
			//	break;
			//==}
			if (!superwarn)
			{
				if (next->inside || next->visited)
				{

					start_holes.erase(start_holes.begin() + i);
					i -= 1;
					break;
				}
			}
			


			if (lowestPoint == nullptr)
			{
				lowestPoint = next;
			}
			else if (compareY(lowestPoint, next))
			{
				lowestPoint = next;
			}
			
			if (next == start)
			{
				//If the area is valid, go back around and visit everything
				Vector2f thisPoint = lowestPoint->orig;
				Vector2f prevPoint = lowestPoint->prev->orig;
				Vector2f nextPoint = lowestPoint->next->orig;
				//std::cout << "determiing type!\n Prev: " << prevPoint << "\n This: " << thisPoint << "\n next: " << nextPoint << "\n";
				float cp = lowestPoint->orig.cross(prevPoint, nextPoint);
				if (cp<0)
				//if (true)
				{
					//We made it through 1 loop so now we confirm it is a real hole.
					Dcel* next2 = next->next;
					while (!next2->visited && next2 != start)
					{
						//Mark as visited so that they are not deleted
						next2->visited = true;
						next2->triangulated = false;
						next2 = next2->next;
						
					}
					
					start_edges.push_back(start_holes[i]);
					start->visited = true;
					;
				}
				else
				{
					/*std::cout << "\nstarting this!\n";
					Dcel* next2 = next->next;
					while (!next2->visited && next2 != start)
					{

						next2->marked = true;
						next2 = next2->next;
					}
					start->marked = true;

					//Here we check inside marked holes
					std::cout << "\n3\n";
					deleteHolesInHoles(0, edges.size());

					std::cout << "\n2\n";
					next2 = next->next;
					while (!next2->visited && next2 != start)
					{
						next2->marked = false;
						next2 = next2->next;
					}
					start->marked = false;
					*/
					//std::cout << "\nREAZED!\n";
					//std::cout << cp << "\n";
					start_holes.erase(start_holes.begin() + i);
					i -= 1;
				}
				break;
			}
			next = next->next;
			prev = prev->next;
		}
	}

	//std::cout << "FINDHOLES IN WALL3\n";

	for (size_t j = 0; j < start_edges.size(); j++)
	{
		if (start_edges[j]->deleteMe && start_edges[j]->next->deleteMe&& start_edges[j]->prev->deleteMe)
		{
			start_edges.erase(start_edges.begin() + j);
		}
	}

	for (size_t j = 0; j < edges.size(); j++)
	{
		if (edges[j]->deleteMe)
		{
			if (!(edges[j]->next->deleteMe && edges[j]->prev->deleteMe))
			{
				edges[j]->deleteMe = false;
			}
		}
	}
	i = 0;
	while (i < edges.size())
	{
		//std::cout << "CHECKING :" << edges[i]->orig << "  " << edges[i]->holeEdge << "\n";
		
		if (!edges[i]->visited || (edges[i]->deleteMe))
		{
			Dcel* edge = edges[i];
			//std::cout << "DELETED " << edge->orig << "\n";

			if (edge == start_edge)
			{
				if (i < edges.size() - 1)
				{
					start_edge = edges[i + 1];
				}
				else if (start_edges.size() > 0)
				{
					start_edge = nullptr;
					for (size_t j = 0; j < start_edges.size(); j++)
					{
						if (start_edge != start_edges[j])
						{
							start_edge = start_edges[j];
							break;
						}
					}
					if (start_edge == nullptr)
					{
						std::cout << "MEGA WARNING4 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!||||||||$$$$$$$&^#&%@&#^%&^#%&^@#%&^#%&^@#&^#%#@^&^\n";
					}

				}
				else
				{
					std::cout << "MEGA WARNING3 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!||||||||$$$$$$$&^#&%@&#^%&^#%&^@#%&^#%&^@#&^#%#@^&^\n";
					start_edge = nullptr;
					
				}

			}
			
			edges.erase(edges.begin() + i);
			delete edge;
			i -= 1;
		}
		else
		{
			edges[i]->inside = false;
			edges[i]->visited = false;
			edges[i]->marked = false;
		}	

		i += 1;
	}
	
	//std::cout << "FINDHOLES IN WALL4\n";
}
/*void Polygon::findIntersectionsHoles()
{
	// Initialise the status T
	std::vector<ise> events;
	Bstd* T = new Bstd();
	while (Q != nullptr)
	{
		Bstd v = popBst(Q);
		//printLeftRight(T);
		if (v.data != nullptr)
		{
			//std::cout << v.data->orig << "\n";
			vertexType type = determineType(*v.data, false);
			v.data->type = type;
			switch (type)
			{
			case splitv:
				//std::cout << "Handling split\n";
				handleSplitL(v.data, T, events);
				break;
			case mergev:
				//std::cout << "Handling merge\n";
				handleMergeL(v.data, T, events);
				break;
			case startv:
				//std::cout << "Handling start\n";
				handleStartL(v.data, T, events);
				break;
			case endv:
				//std::cout << "Handling end\n";
				handleEndL(v.data, T, events);
				break;
			case regularv:
				//std::cout << "Handling regular\n";
				handleRegularL(v.data, T, events);
				break;
			default:
				//std::cout << "Handling def\n";
				break;
			}
		}
		//std::cout << "\n\n NEXT VERTEX!\n";
		if (stat == nullptr) break;
	}
	//std::cout << "JOVER\n";
	//https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
	int i = 0;
	if (start_edges.size() == 0)
	{
		start_edges.push_back(start_edge);
	}
	//for (int i = 0; i < start_edges.size(); i++)
	//{
	//	start_holes.push_back(start_edges[i]);
	
	//start_holes.push_back(start_edge);
	//std::cout << "survived1\n";
	
	//Check if holes are good
	while (i < start_edges.size())
	{
		Dcel* start = start_edges[i];
		Dcel* prev = start;
		Dcel* next = start_edges[i]->next;
		//std::cout << "startEdge = " << start_edges[i]->orig << "\n";
		bool startedHole = start->holeEdge;
		//if (startedHole) continue;
		bool isClockwise = false;
		Dcel* lowestPoint = nullptr;
		while (!next->visited)
		{

			if (lowestPoint == nullptr)
			{
				lowestPoint = next;
			}
			else if (next->orig.y < lowestPoint->orig.y || (next->orig.y == lowestPoint->orig.y && next->orig.x > lowestPoint->orig.x))
			{
				lowestPoint = next;
			}
			//Inside points are illegal
			if (next->inside)
			{
				break;
			}
			next = next->next;
			prev = prev->next;
			if (next == start)
			{
				//If the area is valid, go back around and visit everything
				Vector2f thisPoint = lowestPoint->orig;
				Vector2f prevPoint = lowestPoint->prev->orig;
				Vector2f nextPoint = lowestPoint->next->orig;
				//std::cout << "determiing type!\n Prev: " << prevPoint << "\n This: " << thisPoint << "\n next: " << nextPoint << "\n";
				float cp = lowestPoint->orig.cross(prevPoint, nextPoint);
				if (cp < 0)
				{
					Dcel* next2 = next->next;
					while (!next2->visited && next2 != start)
					{
						next2->visited = true;
						next2->triangulated = false;
						next2 = next2->next;
					}
					start->visited = true;
					break;
				}
				else
				{
					start_edges.erase(start_edges.begin() + i);
					i -= 1;
				}
				//start->visited = true;
				break;
			}
		}
		i++;
	}
	for (int i = 0; i < start_holes.size(); i++)
	{
		//std::cout << "CONSIDERING STARTHOLES\n";
		Dcel* start = start_holes[i];
		Dcel* prev = start;
		Dcel* next = start_holes[i]->next;
		Dcel* lowestPoint = nullptr;
		//std::cout << "startHole = " << start_holes[i]->orig << "\n";
		//Run around the hole, if we dont hit anything on the way, set to visited
		while (!next->visited && next != start)
		{
			//If we run into an edge that is not a hole then its invalid.
			//if (!next->holeEdge)
			//{
			//	break;
			//}
			if (lowestPoint == nullptr)
			{
				lowestPoint = next;
			}
			else if (next->orig.y < lowestPoint->orig.y || (next->orig.y == lowestPoint->orig.y && next->orig.x > lowestPoint->orig.x))
			{
				lowestPoint = next;
			}
			//Inside points are illegal
			if (next->inside)
			{
				break;
			}
			next = next->next;
			prev = prev->next;

			if (next == start)
			{
				//If the area is valid, go back around and visit everything
				Vector2f thisPoint = lowestPoint->orig;
				Vector2f prevPoint = lowestPoint->prev->orig;
				Vector2f nextPoint = lowestPoint->next->orig;
				//std::cout << "determiing type!\n Prev: " << prevPoint << "\n This: " << thisPoint << "\n next: " << nextPoint << "\n";
				float cp = lowestPoint->orig.cross(prevPoint, nextPoint);
				if (cp < 0)
				{
					//We made it through 1 loop so now we confirm it is a real hole.
					Dcel* next2 = next->next;
					while (!next2->visited && next2 != start)
					{
						//Mark as visited so that they are not deleted
						next2->visited = true;
						next2->triangulated = false;
						next2 = next2->next;
					}
					//std::cout << "found edge\n";
					start_edges.push_back(start_holes[i]);
					start->visited = true;
					break;
				}
			}
		}
	}

	//std::cout << "survived3\n";

	i = 0;
	while (i < edges.size())
	{
	//	std::cout << "CHECKING :" << edges[i]->orig << "  " << edges[i]->holeEdge << "\n";
		if (!edges[i]->visited)
		{
			Dcel* edge = edges[i];
		//	std::cout << "DELETED " << edge->orig << "\n";
			edges.erase(edges.begin() + i);
			if (edge == start_edge)
			{
				if (i < edges.size() - 1)
				{
					start_edge = edges[i + 1];
				}
				else
				{
					start_edge == nullptr;
					std::cout << "WTF22222222222222222222222222222222222222222222222222222\n";
				}

			}
			delete edge;
			i -= 1;
		}
		else
		{
			edges[i]->visited = false;
		}

		i += 1;
	}
	//std::cout << "survived4\n";
}*/

void Polygon::resetStartEdges()
{
	start_edges.clear();
}

void Polygon::resetAllEdges()
{
	for (size_t i = 0; i < edges.size(); i++)
	{
		edges[i]->holeEdge = false;
		edges[i]->helper = nullptr;
		edges[i]->triangulated = false;
		edges[i]->visited = false;
		edges[i]->inside = false;
		resetStartEdges();
	}
}

// Re adjust the adjacency of all lines
// required to get rid of glitches
void Polygon::rebalanceTree(Bstd* T)
{
	
	std::stack<Dcel*> edgeStack;
	while (!(T->left == nullptr && T->right == nullptr))
	{
		Bstd popped = popBst(T);
		
		edgeStack.push(popped.data);
		
	}
	edgeStack.push(T->data);
	T->data = nullptr;
	while (!edgeStack.empty())
	{
		//std::cout << edgeStack.size() << "\n";
		Dcel* popped = edgeStack.top();
		insertBstX(T, popped, &sweepLocation);
		edgeStack.pop();
	}
}