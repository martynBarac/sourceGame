#include "cbase.h"
#include "bst.h"


Bstd popBst(Bstd*& root)
{
	Bstd popped = *root;
	Bstd* minimum = root;
	Bstd* prev = root;
	if (root->left == nullptr && root->right == nullptr)
	{
		popped = *root;
		root = nullptr;
		return popped;
	}

	while (minimum->left != nullptr)
	{
		prev = minimum;
		minimum = minimum->left;
	}

	popped.left = minimum->left;
	popped.right = minimum->right;
	popped.data = minimum->data;

	if (minimum->right != nullptr)
	{


		minimum->data = minimum->right->data;
		minimum->left = minimum->right->left;
		Bstd* temp = minimum->right->right;
		delete minimum->right;
		minimum->right = temp;
		return popped;
	}

	prev->left = nullptr;
	delete minimum;
	return popped;
}
Bstd* findMin(Bstd* root)
{
	Bstd* current = root;
	while (current->left != nullptr)
	{
		current = current->left;
	}
	return current;
}

/*Bstd* findPrev(Bstd* root, Dcel* edge)
{
	if (root == nullptr)
		return nullptr;
	if (root->data == nullptr)
		return nullptr;
	std::cout << "findprev: " << edge->orig.x << " , " << root->data->orig.x << "\n";
	if (edge->orig.x < root->data->orig.x)
	{
		return findPrev(root->left, edge);
	}
	else if (edge->orig.x > root->data->orig.x)
	{
		if (root->right == nullptr) return root;
		return findPrev(root->right, edge);
	}
	if (root->left == nullptr) return root;
	return root->left;
}*/
/*
Bstd* findPrev(Bstd* root, Dcel* edge)
{
	if (root->data == nullptr)
		return nullptr;
	Bstd* next = root;
	Bstd* result = nullptr;

	while (next != nullptr) {
		std::cout << "checking: " << next->data->orig << "   against: "<< edge->orig <<"\n";
		if (next->data->orig.x <= edge->orig.x) {
			// Update result and move to the right subtree
			std::cout << "found: " << next->data->orig << "\n";
			result = next;
			next = next->left;
		}
		//else if (next->data->orig.x == edge->orig.x && next->data->orig.y < edge->orig.y)
		//{
		//	result = next;
		//	next = next->left;
		//}
		else {
			// Move to the left subtree
			next = next->right;
		}
	}

	return result;
}
*/
// Calculate the cross product of two vectors (ab and ac)
float crossProduct(const Vector2f& a, const Vector2f& b, const Vector2f& c) {
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// Check if line segment AB is to the left of line segment CD
bool isLeft(const Vector2f& A, const Vector2f& B, const Vector2f& C, const Vector2f& D) {
	return crossProduct(A, B, C) > 0 && crossProduct(A, B, D) > 0;
}
// Check if one line segment is to the left of another, regardless of order
bool isLeftOf(const Vector2f& A, const Vector2f& B, const Vector2f& C, const Vector2f& D) {
	return isLeft(A, B, C, D) || isLeft(D, C, B, A);
}
//https://github.com/velipso/polybooljs/blob/1edf0d3bdf8cc7c58ae733d06db9ecc91b22ef33/lib/epsilon.js#L20
bool pointLeftOrOnLine(Vector2f p, Dcel* leftEdge)
{
	Vector2f top = leftEdge->next->orig;
	Vector2f bottom = leftEdge->orig;
	if (leftEdge->orig.y > leftEdge->next->orig.y || (leftEdge->orig.y == leftEdge->next->orig.y) && (leftEdge->orig.x < leftEdge->next->orig.x))
	{
		top = leftEdge->orig;
		bottom = leftEdge->next->orig;
	}
	float Ax = top.x;
	float Ay = top.y;
	float Bx = bottom.x;
	float By = bottom.y;
	float Cx = p.x;
	float Cy = p.y;
	bool result = (Cx - Ax) * (By - Ay) - (Cy - Ay) * (Bx - Ax) >= EPSILON;
	//std::cout << "testing point  " << p << " Against  " << leftEdge->orig << "  result = " << result << "\n";
	return result;
}
//https://sean.cm/a/polygon-clipping-pt2
bool pointsColinear(Vector2f p1, Vector2f p2, Vector2f p3)
{
	float dx1 = p1.x - p2.x;
	float dx2 = p2.x - p3.x;
	float dy1 = p1.y - p2.y;
	float dy2 = p2.y - p3.y;
	bool result = std::abs(dx1 * dy2 - dx2 * dy1) < EPSILON;

	return result;
}

/*bool edgeLeftOfEdge(Dcel* edge, Dcel* leftEdge, Vector2f* sweepLoc)
{
	//Vector2f a1 = edge->next->orig;
	//Vector2f a2 = edge->orig;
	//if (edge->next->orig.y > a1.y || (edge->next->orig.y == a1.x) && (edge->next->orig.y < a1.x))
	//{
	//	a1 = edge->orig;
	//	Vector2f a2 = edge->next->orig;
	//}
	Vector2f a1 = edge->orig;
	Vector2f a2 = edge->next->orig;
	if (pointsColinear(a1, leftEdge->orig, leftEdge->next->orig))
	{
		if (pointsColinear(a2, leftEdge->orig, leftEdge->next->orig))
		{
			return true;
		}
		return pointLeftOrOnLine(a2, leftEdge);
	}
	return pointLeftOrOnLine(a1, leftEdge);
}*/
//TODO: There is a better version on stack overflow. The case where one segment's y's is between the other will break this case 
float edgeLeftOfEdge2(Dcel* edge, Dcel* leftEdge, Vector2f* sweepLoc)
{
	//

	float y1 = leftEdge->orig.y;
	float x1 = leftEdge->orig.x;
	float y2 = leftEdge->next->orig.y;
	float x2 = leftEdge->next->orig.x;

	//If there are more bugs here then need to find a way to do epsilon better (decide between - or +)
	float bottom = std::fmin(y1, y2);
	float y = sweepLoc->y - EPSILON;
	if (y < bottom)
	{
		y = sweepLoc->y;
	}

	//float y = sweepLoc->y;
	if ((y1 - y2) == 0)
	{

		// Logic behind this is that if we are horizontal, then we are directly on the sweep line
		// Therefore the x must be sweepline x as the sweepline location is updated before the check
		return sweepLoc->x;

	}
	float x = ((x1 - x2) / (y1 - y2)) * (y - y1) + x1;
	return x;
}

bool edgeLeftOfEdge(Dcel* edge, Dcel* leftEdge, Vector2f* sweepLoc)
{
	/*float top1;
	float top2;
	float bot1;
	float bot2;
	if (edge->orig.y > edge->next->orig.y)
	{
		top1 = edge->orig.y;
		bot1 = edge->next->orig.y;
	}
	else
	{
		bot1 = edge->orig.y;
		top1 = edge->next->orig.y;
	}
	if (leftEdge->orig.y > leftEdge->next->orig.y)
	{
		top2 = leftEdge->orig.y;
		bot2 = leftEdge->next->orig.y;
	}
	else
	{
		bot2 = leftEdge->orig.y;
		top2 = leftEdge->next->orig.y;
	}


	//float yMin = std::min(edge->orig.y, leftEdge->orig.y);
	//float yMax = std::max(edge->next->orig.y, leftEdge->next->orig.y);
	float yMin = std::max(bot1, bot2);
	float yMax = std::min(top1, top2);

	float y = yMin + yMax;
	y *= 0.5;*/
	//std::cout << "y = " << y << " \n";
	Vector2f top1;
	Vector2f top2;
	Vector2f bot1;
	Vector2f bot2;
	Vector2f* oldSweep = sweepLoc;
	if (edge->orig.y > edge->next->orig.y)
	{
		top1 = edge->orig;
		bot1 = edge->next->orig;
	}
	else
	{
		bot1 = edge->orig;
		top1 = edge->next->orig;
	}
	if (leftEdge->orig.y > leftEdge->next->orig.y)
	{
		top2 = leftEdge->orig;
		bot2 = leftEdge->next->orig;
	}
	else
	{
		bot2 = leftEdge->orig;
		top2 = leftEdge->next->orig;
	}
	if (top1.equals(top2))
	{
		if (bot2.y > bot1.y)
		{
			sweepLoc = &bot2;
		}
		else
		{
			sweepLoc = &bot1;
		}
	}
	else if (bot1.equals(bot2))
	{
		if (bot2.y > bot1.y)
		{
			sweepLoc = &top2;
		}
		else
		{
			sweepLoc = &top1;
		}
	}
	float x1 = edgeLeftOfEdge2(edge, leftEdge, sweepLoc);
	float x2 = edgeLeftOfEdge2(leftEdge, edge, sweepLoc);
	sweepLoc = oldSweep;
	//std::cout << "edge = " << edge->orig << " leftEdge = " << leftEdge->orig << " leftEdgeNext = " << leftEdge->next->orig << "\n";
	//std::cout << "x1 = " << x1 << " x2 = " << x2 << "\n";
	/*if (x1 == x2)
	{
		Vector2f top2 = leftEdge->next->orig;
		Vector2f bottom2 = leftEdge->orig;
		if (leftEdge->orig.y > leftEdge->next->orig.y || (leftEdge->orig.y == leftEdge->next->orig.y) && (leftEdge->orig.x < leftEdge->next->orig.x))
		{
			top2 = leftEdge->orig;
			bottom2 = leftEdge->next->orig;
		}
		Vector2f top1 = edge->next->orig;
		Vector2f bottom1 = edge->orig;
		if (edge->orig.y > edge->next->orig.y || (edge->orig.y == edge->next->orig.y) && (edge->orig.x < edge->next->orig.x))
		{
			top1 = edge->orig;
			bottom1 = edge->next->orig;
		}
		float dx1 = bottom1.x - top1.x;
		float dy1 = bottom1.y - top1.y;
		float dx2 = bottom2.x - top2.x;
		float dy2 = bottom2.y - top2.y;
		float dxdy1 = dx1 / dy1;
		float dxdy2 = dx2 / dy2;
		return dxdy1 > dxdy2;
		//return edgeLeftOfEdgeBackup(edge, leftEdge, sweepLoc);
	}*/
	return  x1 > x2;
}


bool edgeRightOfEdge(Dcel* edge, Dcel* leftEdge, Vector2f* sweepLoc)
{
	Vector2f top1;
	Vector2f top2;
	Vector2f bot1;
	Vector2f bot2;
	Vector2f* oldSweep = sweepLoc;
	if (edge->orig.y > edge->next->orig.y)
	{
		top1 = edge->orig;
		bot1 = edge->next->orig;
	}
	else
	{
		bot1 = edge->orig;
		top1 = edge->next->orig;
	}
	if (leftEdge->orig.y > leftEdge->next->orig.y)
	{
		top2 = leftEdge->orig;
		bot2 = leftEdge->next->orig;
	}
	else
	{
		bot2 = leftEdge->orig;
		top2 = leftEdge->next->orig;
	}
	if (top1.equals(top2))
	{
		if (bot2.y > bot1.y)
		{
			sweepLoc = &bot2;
		}
		else
		{
			sweepLoc = &bot1;
		}
	}
	else if (bot1.equals(bot2))
	{
		if (bot2.y > bot1.y)
		{
			sweepLoc = &top2;
		}
		else
		{
			sweepLoc = &top1;
		}
	}
	float x1 = edgeLeftOfEdge2(edge, leftEdge, sweepLoc);
	float x2 = edgeLeftOfEdge2(leftEdge, edge, sweepLoc);
	sweepLoc = oldSweep;
	//std::cout << "edge = " << edge->orig << " rightedge = " << leftEdge->orig << " rightEdgeNext = " << leftEdge->next->orig << "\n";
	//std::cout << "x1 = " << x1 << " x2 = " << x2 << "\n";
	//if (x1 == x2)
	//{
	//	return edge->orig.cross(edge->next->orig, leftEdge->next->orig) > EPSILON;
	//}
	return x1 < x2;
}

//bool edgeRightOfEdge(Dcel* edge, Dcel* leftEdge, Vector2f* sweepLoc)
//{
//
//	return !edgeLeftOfEdge(edge, leftEdge, sweepLoc);
//}


Bstd* findPrev(Bstd* root, Dcel* edge, Vector2f* sweepLoc)
{
	if (root->data == nullptr)
		return nullptr;
	Bstd* next = root;
	Bstd* result = nullptr;

	while (next != nullptr) {
		//std::cout << "loopTING!\n";
		//std::cout << "checking: " << next->data->orig << "   against: "<< edge->orig <<"\n";
		//If the edge is to the right
		/*if ((next->data->orig.x <= edge->orig.x || next->data->next->orig.x <= edge->orig.x) && next->data->orig != edge->orig) {
			// Update result and move to the right subtree
			std::cout << "found: " << next->data->orig << "\n";
			result = next;
			next = next->right;
		}*/
		if (edgeRightOfEdge(edge, next->data, sweepLoc) && next->data != edge) {
			// Update result and move to the right subtree
			//std::cout << "found: " << next->data->orig << "\n";
			result = next;
			next = next->right;
		}
		//else if (next->data->orig.x == edge->orig.x && next->data->orig.y < edge->orig.y)
		//{
		//	result = next;
		//	next = next->left;
		//}
		else {
			// Move to the left subtree
			next = next->left;
		}
	}

	return result;
}

Bstd* findNext(Bstd* root, Dcel* edge, Vector2f* sweepLoc)
{
	if (root->data == nullptr)
		return nullptr;
	Bstd* next = root;
	Bstd* result = nullptr;
	while (next != nullptr) {
		//std::cout << "checking: " << next->data->orig << "+" << next->data->next->orig << "   against: " << edge->orig << "\n";
		if (edgeLeftOfEdge(edge, next->data, sweepLoc) && next->data != edge) {
			//std::cout << "found: " << next->data->orig << "\n";
			result = next;
			next = next->left;
		}
		else {
			// Move to the right subtree
			next = next->right;
		}
	}
	return result;
}

int countAll(Bstd* root, Dcel* edge, Vector2f* sweepLoc)
{
	// Base case
	if (root == nullptr) return 0;
	if (root->data == nullptr) return 0;

	// If the edge is to the left then it is counted
	if (edgeRightOfEdge(edge, root->data, sweepLoc) && root->data != edge)
	{
		//std::cout << edge->orig << " is left of " << root->data->orig << "\n";
		return 1 + countAll(root->left, edge, sweepLoc) + countAll(root->right, edge, sweepLoc);
	}


	// If the edge is not to the left then search other brach without being counted
	else
		return countAll(root->left, edge, sweepLoc);
}
/*
Bstd* delBstX(Bstd* &root, Dcel* edge)
{
	Bstd* next = root;
	Bstd* prev = nullptr;
	if (root->left == nullptr && root->right == nullptr)
	{
		if (edge->orig.x == root->data->orig.x && edge->orig.y == root->data->orig.y)
			root->data = nullptr;
		return root;
	}
	if (root->data == nullptr)
	{
		return root;
	}
	if (next == nullptr)
	{
		return root;
	}
	while (next != nullptr && !(next->data->orig.x == edge->orig.x && next->data->orig.y == edge->orig.y))
	{
		prev = next;
		if (edge->orig.x < next->data->orig.x)
		{
			next = next->left;
		}
		else if (edge->orig.x > next->data->orig.x)
		{
			next = next->right;
		}
		else if (edge->orig.y < next->data->orig.y)
		{
			next = next->left;
		}
		else if (edge->orig.y > next->data->orig.y)
		{
			next = next->right;
		}

	}

	// Found nothing do nothing
	if (next == nullptr)
	{
		//std::cout << "found nothong\n";
		return root;
	}
	//std::cout << "found now time to delete\n";
	// Found point to delete
	// No children
	if (next->right == nullptr && next->left == nullptr)
	{
		//std::cout << "No children\n";
		if (prev == nullptr)
		{
			root = nullptr;
		}
		if (next == prev->left)
		{
			prev->left = next->left;
		}
		else
		{
			prev->right = next->left;

		}
		delete next;
	}
	// Right only
	else if (next->right != nullptr && next->left == nullptr)
	{
		//std::cout << "Right only\n";
		if (prev == nullptr)
		{
			root = next->right;
			return root;
		}
		else if (next == prev->left)
			prev->left = next->right;
		else
			prev->right = next->right;
		delete next;
	}
	// Left only
	else if (next->left != nullptr && next->right == nullptr)
	{
		//std::cout << "Left only\n";
		if (prev == nullptr)
		{
			root = next->left;
			return root;
		}
		else if (next == prev->left)
			prev->left = next->left;
		else
			prev->right = next->left;
		delete next;
	}
	// Both children
	else
	{
		//std::cout << "Both\n";
		Bstd* nextRoot = next->right;
		Bstd* prevRoot = nullptr;
		while (nextRoot->left != nullptr)
		{
			prevRoot = nextRoot;
			nextRoot = nextRoot->left;
		}
		if (prevRoot != nullptr)
			prevRoot->left = nextRoot->right;
		else
			next->right = nextRoot->right;
		next->data = nextRoot->data;
		delete nextRoot;
	}
	return root;
}
*/
Bstd* delBstX(Bstd*& root, Dcel* edge, Vector2f* sweepLoc)
{
	//std::cout << "del\n";
	Bstd* next = root;
	Bstd* prev = nullptr;
	if (next == nullptr)
	{
		return root;
	}
	//std::cout << "a\n";
	if (edge->orig.y == edge->next->orig.y) return root;
	if (root->data == nullptr)
	{
		return root;
	}
	if (root->left == nullptr && root->right == nullptr)
	{
		if (edge->orig.x == root->data->orig.x && edge->orig.y == root->data->orig.y)
			root->data = nullptr;
		return root;
	}
	//std::cout << "b\n";

	//std::cout << "c\n";
	while (next != nullptr && !(next->data->orig.x == edge->orig.x && next->data->orig.y == edge->orig.y && next->data->next->orig == edge->next->orig))
	{
		prev = next;

		if (edgeLeftOfEdge(edge, next->data, sweepLoc))
		{
			next = next->left;
		}
		else
		{

			next = next->right;
		}

	}

	// Found nothing do nothing
	if (next == nullptr)
	{
		//std::cout << "found nothong!!!!!!!!!!!!!!!!!!!!!!!\n";
		return root;
	}
	//std::cout << "found now time to delete\n";
	// Found point to delete
	// No children
	if (next->right == nullptr && next->left == nullptr)
	{
		//std::cout << "No children\n";
		if (prev == nullptr)
		{
			root = nullptr;
		}
		if (next == prev->left)
		{
			prev->left = next->left;
		}
		else
		{
			prev->right = next->left;

		}
		delete next;
	}
	// Right only
	else if (next->right != nullptr && next->left == nullptr)
	{
		//std::cout << "Right only\n";
		if (prev == nullptr)
		{
			root = next->right;
			return root;
		}
		else if (next == prev->left)
			prev->left = next->right;
		else
			prev->right = next->right;
		delete next;
	}
	// Left only
	else if (next->left != nullptr && next->right == nullptr)
	{
		//std::cout << "Left only\n";
		if (prev == nullptr)
		{
			root = next->left;
			return root;
		}
		else if (next == prev->left)
			prev->left = next->left;
		else
			prev->right = next->left;
		delete next;
	}
	// Both children
	else
	{
		//std::cout << "Both\n";
		Bstd* nextRoot = next->right;
		Bstd* prevRoot = nullptr;
		while (nextRoot->left != nullptr)
		{
			prevRoot = nextRoot;
			nextRoot = nextRoot->left;
		}
		if (prevRoot != nullptr)
			prevRoot->left = nextRoot->right;
		else
			next->right = nextRoot->right;
		next->data = nextRoot->data;
		delete nextRoot;
	}
	return root;
}

Bstd* insertBstX(Bstd*& root, Dcel* edge, Vector2f* sweepLoc)
{

	Bstd* next = root;
	Bstd* prev = nullptr;

	if (next == nullptr)
	{
		Bstd* newNode = new Bstd(edge);
		root = newNode;
		return root;
	}
	if (root->data == nullptr)
	{
		root->data = edge;
		return root;
	}
	Bstd* newNode = new Bstd(edge);
	newNode->data = edge;


	if (edge->orig.y == edge->next->orig.y) return root;
	while (next != nullptr)
	{
		prev = next;
		if (edge->orig == next->data->orig && edge->next->orig == next->data->next->orig)
		{
			return next;
		}

		if (edgeLeftOfEdge(edge, next->data, sweepLoc))
		{
			//std::cout << next->data->orig << " is right of " << edge->orig << "\n";
			next = next->left;
		}
		else
		{
			//std::cout << next->data->orig << " is left of " << edge->orig << "\n";
			next = next->right;
		}
	}
	if (prev == nullptr)
		prev = newNode;
	else if (!edgeLeftOfEdge(edge, prev->data, sweepLoc))
	{
		prev->right = newNode;
	}
	else
	{
		prev->left = newNode;
	}
	return prev;
}

Bstd* insertBstY(Bstd*& root, Dcel* edge)
{
	Bstd* next = root;
	Bstd* prev = nullptr;


	if (next == nullptr)
	{
		Bstd* newNode = new Bstd(edge);
		root = newNode;
		return root;
	}
	if (root->data == nullptr)
	{
		root->data = edge;
		return root;
	}
	Bstd* newNode = new Bstd(edge);
	newNode->data = edge;



	while (next != nullptr)
	{
		prev = next;
		if (edge->orig == next->data->orig && edge->next->orig == next->data->next->orig)
		{
			return next;
		}
		if (edge->orig.y > next->data->orig.y || ((edge->orig.y == next->data->orig.y) && (edge->orig.x < next->data->orig.x)))
		{
			//std::cout << next->data->orig << " is right of " << edge->orig << "\n";
			next = next->left;
		}
		else
		{
			//std::cout << next->data->orig << " is left of " << edge->orig << "\n";
			next = next->right;
		}
	}

	if (prev == nullptr)
		prev = newNode;

	else if (!(edge->orig.y > prev->data->orig.y || ((edge->orig.y == prev->data->orig.y) && (edge->orig.x < prev->data->orig.x))))
	{
		prev->right = newNode;
	}
	else
	{
		prev->left = newNode;
	}

	return prev;
}


void printLeftRight(Bstd* root)
{

	if (root == nullptr || root->data == nullptr)
	{
		//std::cout << "NULL\n";
		return;
	}

	printLeftRight(root->left);
	std::cout << root->data->orig << "\n";
	printLeftRight(root->right);
}

