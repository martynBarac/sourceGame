#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "polygon.h"
#include <algorithm>
#include <string>
#include <stack>
std::vector<cv::Point2f> control_points;
std::vector<Polygon*> debug_holes;
cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
void mouse_handler(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4)
	{
		std::cout << "Left button of the mouse is clicked - position (" << x << ", "
			<< y << ")" << '\n';
		control_points.emplace_back(x, y);
	}
}

void naive_bezier(const std::vector<cv::Point2f>& points, cv::Mat& window)
{
	auto& p_0 = points[0];
	auto& p_1 = points[1];
	auto& p_2 = points[2];
	auto& p_3 = points[3];

	for (double t = 0.0; t <= 1.0; t += 0.001)
	{
		auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
			3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

		window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
	}
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f>& control_points, double t)
{
	// Implement de Casteljau's algorithm
	// https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm
	std::vector<cv::Point2f> beta = control_points;
	for (unsigned int j = 1; j < control_points.size(); j++) // Take the middle point of all the middle points n-1 times
	{
		for (unsigned int i = control_points.size() - 1; i >= j; i--)
		{
			beta[i] = beta[i - 1] * t + beta[i] * (1 - t); // Get middle point
		}
	}
	return beta[control_points.size() - 1]; //Return the final midpoint 
}

void bezier(const std::vector<cv::Point2f>& control_points, cv::Mat& window)
{
	// TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
	// recursive Bezier algorithm.
	for (double t = 0.0; t <= 1.0; t += 0.001)
	{
		auto p = recursive_bezier(control_points, t);

		// Co-ordinates of nearest pixels
		float x1 = (int)(p.x + 1);
		float x2 = (int)(p.x);
		float y1 = (int)(p.y);
		float y2 = (int)(p.y + 1);
		//std::cout << p.x << " "  << p.y << " " << x1 << " " << x2 << " " << y1 << " " << y2 << " \n";

		// Bilinear interpolation between each pixel
		// https://en.wikipedia.org/wiki/Bilinear_interpolation
		float w11 = (x2 - p.x) * (y2 - p.y) / ((x2 - x1) * (y2 - y1));

		float w12 = (x2 - p.x) * (p.y - y1) / ((x2 - x1) * (y2 - y1));
		float w21 = (p.x - x1) * (y2 - p.y) / ((x2 - x1) * (y2 - y1));
		float w22 = (p.x - x1) * (p.y - y1) / ((x2 - x1) * (y2 - y1));

		//std::cout << p.x << " "  << p.y << " " << w11 << " " << w12 << " " << w21 << " " << w22 << " \n";
		float c11 = window.at<cv::Vec3b>(y1, x1)[1];
		float c12 = window.at<cv::Vec3b>(y2, x1)[1];
		float c21 = window.at<cv::Vec3b>(y1, x2)[1];
		float c22 = window.at<cv::Vec3b>(y2, x2)[1];

		//Apply pixel if it is brighter
		window.at<cv::Vec3b>(y1, x1)[1] = std::max(255 * w11, c11);
		window.at<cv::Vec3b>(y2, x1)[1] = std::max(255 * w12, c12);
		window.at<cv::Vec3b>(y1, x2)[1] = std::max(255 * w21, c21);
		window.at<cv::Vec3b>(y2, x2)[1] = std::max(255 * w22, c22);


		//window.at<cv::Vec3b>(p.y, p.x)[1] = 255;
	}
}

// https://en.wikipedia.org/wiki/Line_drawing_algorithm
void drawLine(Vector2f begin, Vector2f end, cv::Mat& window)
{
	auto x1 = begin.x;
	auto y1 = 700-begin.y;
	auto x2 = end.x;
	auto y2 = 700-end.y;

	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

	dx = x2 - x1;
	dy = y2 - y1;
	dx1 = fabs(dx);
	dy1 = fabs(dy);
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;

	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1;
			y = y1;
			xe = x2;
		}
		else
		{
			x = x2;
			y = y2;
			xe = x1;
		}
		window.at<cv::Vec3b>(y, x)[1] = 255;
		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
			{
				px = px + 2 * dy1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					y = y + 1;
				}
				else
				{
					y = y - 1;
				}
				px = px + 2 * (dy1 - dx1);
			}
			//            delay(0);
			window.at<cv::Vec3b>(y, x)[1] = 255;
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1;
			y = y1;
			ye = y2;
		}
		else
		{
			x = x2;
			y = y2;
			ye = y1;
		}
		window.at<cv::Vec3b>(y, x)[1] = 255;
		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
			{
				py = py + 2 * dx1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					x = x + 1;
				}
				else
				{
					x = x - 1;
				}
				py = py + 2 * (dx1 - dy1);
			}
			//            delay(0);
			window.at<cv::Vec3b>(y, x)[1] = 255;
		}
	}
}

void drawPolygonHelper(Dcel* edge, Dcel* startEdge, float xShift, float yShift, float scale, cv::Mat& window, cv::Scalar color)
{
	
	
	Vector2f v1 = edge->orig;
	Vector2f v2 = edge->next->orig;
	Vector2f v3 = startEdge->orig;
	//std::cout << "draw! " << v1 << " " << v2 << "    Edge:"<< edge << "   Startedge: " << startEdge<< "\n";
	//std::cout <<"(" << edge->orig << "), ";
	v1.x *= scale;
	v1.y *= scale;
	v2.x *= scale;
	v2.y *= scale;
	v3.x *= scale;
	v3.y *= scale;
	v1.x += xShift;
	v2.x += xShift;
	v3.x += xShift;
	v1.y += yShift;
	v2.y += yShift;
	v3.y += yShift;
	//cv::circle(window, cv::Point(v1.x, 700 - v1.y), 5, { 255, 255, 255 }, 5);
	
	
	//drawLine(v1, v2, window);
	cv::arrowedLine(window, cv::Point(v1.x, 700 - v1.y), cv::Point(v2.x, 700 - v2.y), color);
	float size = 3.0f;
	/*
	
	switch (edge->type)
	{
	case regularv:
		cv::circle(window, cv::Point(v1.x, 700 - v1.y), size, { 0, 255, 0 }, size);
		break;
	case startv:
		cv::rectangle(window, cv::Rect2d(cv::Point2d(v1.x + size, 700 - v1.y + size), cv::Point2d(v1.x - size, 700 - v1.y - size)), { 0, 255, 0 });
		break;
	case endv:
		cv::rectangle(window, cv::Rect2d(cv::Point2d(v1.x + size, 700 - v1.y + size), cv::Point2d(v1.x - size, 700 - v1.y - size)), { 0, 255, 0 }, size);
		break;
	case mergev:
		cv::drawMarker(window, cv::Point(v1.x, 700 - v1.y), { 0, 255, 0 }, cv::MARKER_TRIANGLE_DOWN, size*3);
		break;
	case splitv:
		cv::drawMarker(window, cv::Point(v1.x, 700 - v1.y), { 0, 255, 0 }, cv::MARKER_TRIANGLE_UP, size*3);
		break;
	default:
		break;
	}
	*/
	if (edge->inside)
	{
		cv::drawMarker(window, cv::Point(v1.x, 700 - v1.y), { 0, 255, 0 }, cv::MARKER_TRIANGLE_UP, size * 3);
	}
	//cv::circle(window, cv::Point(v2.x, 700 - v2.y), 1, { 255, 0, 0 }, 3);
	//cv::circle(window, cv::Point(v1.x, 700 - v1.y), 1, { 0, 255, 0 }, 3);
	//cv::circle(window, cv::Point(v3.x, 700 - v3.y), 1, { 0, 255, 255 }, 4);
	if (edge->direction == cCW)
	{
		//cv::circle(window, cv::Point(v1.x, 700 - v1.y), 1, { 255, 0, 0 }, 3);
	}
	else if (edge->direction == cW)
	{
		//cv::circle(window, cv::Point(v1.x, 700 - v1.y), 1, { 0, 255, 0 }, 3);
	}
	else
	{
		//cv::circle(window, cv::Point(v1.x, 700 - v1.y), 1, { 0, 0, 255 }, 3);
	}
	cv::circle(window, cv::Point(v1.x, 700 - v1.y), 1, { 255, 0, 0 }, 3);
	if (edge->drawtext)
	{
		cv::circle(window, cv::Point(v1.x, 700 - v1.y), 1, { 255, 0, 255 }, 3);
		cv::putText(window, //target image
			(std::string) edge->orig, //text
			cv::Point(v1.x, 700 - v1.y - 10), //top-left position
			cv::FONT_HERSHEY_DUPLEX,
			0.3,
			CV_RGB(255, 255, 255), //font color
			0.4);
	}

	cv::imshow("Polygons", window);
	//int key = cv::waitKey(0);
	if (edge->orig.x == startEdge->orig.x && edge->orig.y == startEdge->orig.y)
		return;
	drawPolygonHelper(edge->next, startEdge, xShift, yShift, scale, window, color);

}



void drawPolygon(Polygon p, float xShift, float yShift, float scale, cv::Mat& window)
{
	/*std::cout << "Number of edges: " << p.edges.size() << "\n";
	for (int i = 0; i < p.edges.size(); i++)
	{
		std::cout <<"EDGE:  " << p.edges[i]->orig << "\n";
		std::cout << p.edges[i]->holeEdge << "\n";
		std::cout << p.edges[i]->inside << "\n";
		std::cout << p.edges[i]->type << "\n";
		std::cout << p.edges[i]->type << "\n";
		std::cout << "\n";
	}
	std::cout << p.start_edge->prev << "done\n";*/
	//std::cout << p.start_edges.size() << " seperate parts\n";
	int R = rand() % 255;
	int G = rand() % 255;
	int B = rand() % 255;

	drawPolygonHelper(p.start_edge, p.start_edge->prev, xShift, yShift, scale, window, { (float)R,(float)G,(float)B });

	
	for (int i = 0; i < p.start_edges.size(); i++)
	{
		//window.setTo(cv::Scalar(0, 0, 0));
		int R = rand() % 255;
		int G = rand() % 255;
		int B = rand() % 255;

		//int key = cv::waitKey(0);
		drawPolygonHelper(p.start_edges[i], p.start_edges[i]->prev, xShift, yShift, scale, window, { (float) R,(float)G,(float)B });

	}
	Vector2f v1 = p.boundingBox[0];
	Vector2f v2 = p.boundingBox[1];

	v1.x *= scale;
	v1.y *= scale;
	v2.x *= scale;
	v2.y *= scale;

	v1.x += xShift;
	v2.x += xShift;

	v1.y += yShift;
	v2.y += yShift;
	//std::cout << "bounding boxes\n";
	//std::cout << p.boundingBox[0] << "\n";
	//std::cout << p.boundingBox[1] << "\n";
	//cv::rectangle(window, cv::Rect2d(cv::Point2d(v1.x, 700 - v1.y), cv::Point2d(v2.x, 700 - v2.y)), { (float)R,(float)G,(float)B });
	return;
}


void traverseStack(Bstd * Q)
{
	if (Q == nullptr)
		return;
	traverseStack(Q->left);
	std::cout << Q->data->orig.y << "\n";
	traverseStack(Q->right);
}

void traverseStackInOrder(Bstd* Q)
{
	if (Q == nullptr)
		return;
	traverseStackInOrder(Q->left);
	std::cout << Q->data->orig.y << "\n";
	traverseStackInOrder(Q->right);
}

void triangulateAndDraw(Polygon* p,  cv::Mat& window)
{
	Polygon* newPoly = nullptr;
	std::vector< std::vector<Vector2f>> triangles = p->triangulateMonotone(p->start_edge, newPoly);
	for (int j = 0; j < triangles.size(); j++)
	{
		std::vector<cv::Point>  triangle;
		triangle.resize(3);
		triangle[0] = (cv::Point2d) triangles[j][0];
		triangle[1] = (cv::Point2d) triangles[j][1];
		triangle[2] = (cv::Point2d) triangles[j][2];
		int R = rand() % 255;
		int G = rand() % 255;
		int B = rand() % 255;
		cv::fillPoly(window, triangle, { (float)R,(float)G,(float)B });

	}
	if (newPoly != nullptr)
	{
		//newPolygons.push_back(newPoly);
		delete newPoly;
	}
	for (int i = 0; i < p->start_edges.size(); i++)
	{
		Polygon* newPoly2 = nullptr;
		std::vector< std::vector<Vector2f>> triangles = p->triangulateMonotone(p->start_edges[i], newPoly2);
		for (int j = 0; j < triangles.size(); j++)
		{
			std::vector<cv::Point>  triangle;
			triangle.resize(3);
			triangle[0] = (cv::Point2d) triangles[j][0];
			triangle[1] = (cv::Point2d) triangles[j][1];
			triangle[2] = (cv::Point2d) triangles[j][2];
			int R = rand() % 255;
			int G = rand() % 255;
			int B = rand() % 255;
			cv::fillPoly(window, triangle, { (float)R,(float)G,(float)B });
		}
		if (newPoly2 != nullptr)
		{
			//newPolygons.push_back(newPoly2);
			delete newPoly2;
		}
	}
}
std::vector<Polygon*> newHoles;
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		if (!debug_holes.empty())
		{
			newHoles.push_back(debug_holes[0]);
			debug_holes.erase(debug_holes.begin());
		}
		
	}
	else if (event == cv::EVENT_RBUTTONDOWN)
	{
		std::cout << "Right button of the mouse is clicked - position (" << ((float)x - 130) / gscale << ", " << (700 - (float)y - 140) / gscale << ")" << "\n";
		Polygon* newHole = new Polygon();
		newHole->load("holefile3.txt");
		newHole->generateDcel();
		newHole->sortEdgeList();
		newHole->translate(Vector2f(((float)x-130)/ gscale, (700- (float)y-140)/ gscale));
		
		newHoles.push_back(newHole);
		drawPolygon(*newHole, gxpan, gypan, gscale, window);
	}
	else if (event == cv::EVENT_MBUTTONDOWN)
	{
		std::cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << "\n";
	}
	else if (event == cv::EVENT_MOUSEMOVE)
	{
		//std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << "\n";

	}
}

void addDebugHole(float x, float y)
{
	Polygon* newHole = new Polygon();
	newHole->load("holefile3.txt");
	newHole->generateDcel();
	newHole->sortEdgeList();
	newHole->translate(Vector2f(x, y));
	debug_holes.push_back(newHole);
	newHoles.push_back(newHole);
}

int main()
{
	
	cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
	cv::namedWindow("Polygons", cv::WINDOW_AUTOSIZE);
	
	int key = -1;


	Polygon* myPoly = new Polygon();
	Polygon* holePoly = new Polygon();
	Polygon* holePoly2 = new Polygon();
	Polygon* holePoly3 = new Polygon();
	myPoly->load("polyfile31.txt");
	holePoly->load("holefile.txt");
	holePoly2->load("holefile.txt");
	holePoly3->load("holefile.txt");
	holePoly->generateDcel();
	holePoly2->generateDcel();
	holePoly3->generateDcel();
	myPoly->generateDcel();
	holePoly->sortEdgeList();
	holePoly2->sortEdgeList();
	holePoly3->sortEdgeList();
	myPoly->sortEdgeList();
	//newHoles.push_back(holePoly);
	//newHoles.push_back(holePoly2);
	//holePoly->translate(Vector2f(5.32, 0.94));
	holePoly->translate(Vector2f(5.6, -0.18));
	
	holePoly2->translate(Vector2f(5.32, 0.4));
	//myPoly->addHole(*holePoly, 4,1);
	//myPoly->addHole(*holePoly2, 4, 0);
	//myPoly->addHole(*holePoly3, 8.5, 4.5);
	holePoly3->translate(Vector2f(8.5, 4.5));
	myPoly->generateBst();
	
	//myPoly->findIntersections();
	myPoly->sortEdgeList();
	Polygon* newPoly = new Polygon;
	newPoly->duplicate(*myPoly);
	newPoly->sortEdgeList();
	myPoly->generateBst();
	
	
	myPoly->makeMonotone();
	

	//addDebugHole(2.4, 4.08)		  ;
	//addDebugHole(2.58, 4.08)	  ;
	//addDebugHole(6.04, -0.56) ;
	//addDebugHole(5.98, -0.2)  ;
	//addDebugHole(5.96, 0.14)  ;
	//addDebugHole(5.94, 0.4)	  ;
	//addDebugHole(5.98, 0.86)  ;
	//addDebugHole(6.02, 1.4)	  ;
	//addDebugHole(6.02, 1.76)  ;
	//addDebugHole(6, 2.1)	  ;
	//addDebugHole(5.98, 2.64)  ;
	//addDebugHole(6 , 3.1)	  ;
	//addDebugHole(6.02, 3.54)  ;
//	addDebugHole(6.02, 4.12)  ;
//	addDebugHole(6.04, 4.66)  ;
//	addDebugHole(6.04, 5.08)  ;
	//addDebugHole(6.04, 5.66)  ;
	//addDebugHole(6.08, 6.26)  ;
	//addDebugHole(6.12, 7)	  ;

	//myPoly->generateBst();


	myPoly->findIntersections();

	
	//drawPolygon(*myPoly, gxpan, gypan, gscale, window);
	//myPoly->sortEdgeList();
	//myPoly->generateBst();
	//myPoly->makeMonotone();

	//drawPolygon(*myPoly, gxpan, gypan, gscale, window);
	cv::setMouseCallback("Polygons", CallBackFunc, NULL);
	
	std::vector<Polygon*> polygons;
	polygons.push_back(newPoly);
	triangulateAndDraw(myPoly, window);
	
	cv::imshow("Polygons", window);
	//key = cv::waitKey(0);
	//delete myPoly;
	//delete holePoly;
	delete holePoly2;
	window.setTo(cv::Scalar(0, 0, 0));
	bool shouldUpdate = true;
	Polygon* firstHole = new Polygon;
	firstHole->load("holefile2.txt");
	firstHole->generateDcel();
	firstHole->sortEdgeList();
	firstHole->translate(Vector2f(((float)1.74 ), ( (float)1.28 )));
	firstHole->markAsHole();
	while (true)
	{
		std::vector<Polygon*> newPolygons;
		

		//delete holePoly3;
		
		cv::imshow("Polygons", window);
		//cv::imwrite("Polygons.png", window);
		std::cout << "PROGRAM OVA!";
		//key = cv::waitKey(0);
		//for (int i = 0; i < newHoles.size(); i++)
	//	{
		//	drawPolygon(*newHoles[i], gxpan, gypan, gscale, window);
	//	}
		key = cv::waitKey(0);
		window.setTo(cv::Scalar(0, 0, 0));
		//newHoles.push_back(holePoly3);
		
		Polygon* newPoly = new Polygon;
		newPoly->duplicate(*myPoly);
		newPolygons.push_back(newPoly);
		for (int j = 0; j < newHoles.size(); j++)
		{
			//only do this if bounding boxes collide
			shouldUpdate = true;
			Polygon* tempHole;
			tempHole = newHoles[j];
			tempHole->markAsHole();
			
			tempHole->sortEdgeList();
	

			firstHole->addHole(*tempHole, 0, 0, false);
			//firstHole->generateBst();
			//firstHole->sortEdgeList();
			//drawPolygon(*tempHole, gxpan, gypan, gscale, window);
			
			
			firstHole->findIntersectionsHoles();
			//drawPolygon(*firstHole, gxpan, gypan, gscale, window);
			//int key = cv::waitKey(0);
			firstHole->sortEdgeList();
			
			window.setTo(cv::Scalar(0, 0, 0));
			
			//std::cout << "\n\n";
			//firstHole->resetAllEdges();
			//for (int k = 0; k < firstHole->edges.size(); k++)
			//{
			//	firstHole->edges[k]-> = false;
			//}
		
			//firstHole->resetAllEdges2();
		}
		
		for (int i = 0; i < polygons.size(); i++)
		{
			polygons[i]->resetAllEdges();
			
			
			//polygons[i]->addHole(*holePoly3, 0, 0);
			//Dupe all the holes
			std::vector<Polygon*> tempHoles;
			shouldUpdate = false;
			//std::cout << "add holes\n";
			/*for (int j = 0; j < newHoles.size() ; j++)
			{
				//only do this if bounding boxes collide
				if (!polygons[i]->checkBoundingBoxIntersection(*newHoles[j])) continue;
				shouldUpdate = true;
				
				
				tempHole->generateDcel();
				tempHole->sortEdgeList();
				tempHole->markAsHole();
				if (j == 0)
				{
					firstHole = tempHole;
				}
				else
				{
					firstHole->addHole(*tempHole, 0, 0, false);
					firstHole->generateBst();
					//firstHole->sortEdgeList();
					firstHole->findIntersectionsHoles();
					firstHole->sortEdgeList();
				}
				//firstHole->resetAllEdges2();
				
			}*/
			
			
			if (firstHole)
			{
				Polygon* tempHole = new Polygon;
				tempHole->duplicate(*firstHole);
				//tempHole->generateDcel();
				tempHoles.push_back(tempHole);
				//drawPolygon(*tempHole, gxpan, gypan, gscale, window);
				//int key = cv::waitKey(0);
				tempHole->markAsHole();
				tempHole->sortEdgeList();
				//polygons[i]->sortEdgeList();
				
				polygons[i]->addHole(*tempHole, 0, 0);
				
				//firstHole->resetAllEdges();
				
				//drawPolygon(*polygons[i]);
				
				//std::cout << "speed check!\n";

				//std::cout << "added holes!!!!!!!!!!!!!!!!" << newHoles[j]->start_edge << "\n";

				//Delete temp holes
				for (int j = 0; j < tempHoles.size(); j++)
				{
					delete tempHoles[j];
				}
				tempHoles.clear();
			
				//polygons[i]->generateBst();
				
				polygons[i]->findIntersections();
				//drawPolygon(*polygons[i], gxpan, gypan, gscale, window);
				//key = cv::waitKey(0);
				if (polygons[i]->shouldDelete())
				{
					continue;
				}
				if (polygons[i]->start_edge == nullptr)
				{
					continue;
				}
				
			}
			//drawPolygon(*polygons[i], gxpan, gypan, gscale, window);
			
			
			
			
		}
		//int key = cv::waitKey(0);
		for (int i = 0; i < polygons.size(); i++)
		{

			if (polygons[i]->shouldDelete() || polygons[i]->start_edge == nullptr)
			{
				delete polygons[i];
				continue;
			}
			polygons[i]->sortEdgeList();
			
			polygons[i]->generateBst();
			polygons[i]->makeMonotone();
			
			triangulateAndDraw(polygons[i], window);
			delete polygons[i];
		}
		
		polygons.clear();
		polygons.resize(newPolygons.size());
		for (int i = 0; i < polygons.size(); i++)
		{
			polygons[i] = newPolygons[i];
		}
		for (int i = 0; i < newHoles.size(); i++)
		{
			//newHoles[i]->traverseDcel(newHoles[i]->start_edge);
			delete newHoles[i];
		}
		newHoles.clear();
		
	}
	
	
	






	
	
	
	return 0;
}
