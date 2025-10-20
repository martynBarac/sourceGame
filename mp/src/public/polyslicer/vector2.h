#pragma once
#include "iostream"
#include "string"
#include <sstream>
#include <iomanip>

#include "globals.h"
class Vector2f
{
public:
	float x;
	float y;
	
	Vector2f(float _x, float _y)
	{
		x = _x;
		y = _y;
	}
	Vector2f()
	{
		x = 0;
		y = 0;
	}
	
	virtual ~Vector2f()
	{
		x = 0;
		y = 0;
	}

	friend std::ostream& operator<<(std::ostream& os, Vector2f const& v) {
		return os << v.x << ", " << v.y;
	}
	float cross(Vector2f prev, Vector2f next)
	{
		return ((prev.x - x) * (next.y - y)) - ((prev.y - y) * (next.x - x));
	}
	bool equals(Vector2f v)
	{
		if (x >= v.x - EPSILON && x <= v.x + EPSILON)
		{
			if (y >= v.y - EPSILON && y <= v.y + EPSILON)
			{
				return true;
			}
		}
		return false;
	}
	bool equals2(Vector2f v)
	{
		if (x >= v.x - 0.005 && x <= v.x + 0.005)
		{
			if (y >= v.y - 0.005 && y <= v.y + 0.005)
			{
				return true;
			}
		}
		return false;
	}
	bool operator == (const Vector2f& v) {
		if (x == v.x && y == v.y)
			return true;
		return false;
	}
	bool operator != (const Vector2f& v) {
		if (x == v.x && y == v.y)
			return false;
		return true;
	}
	Vector2f operator - (const Vector2f& v) {
		return Vector2f(x - v.x, y - v.y);
	}
	operator std::string() const
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(2) << x << ", " << y;
		std::string s1 = stream.str();
		return s1;
	}
};

