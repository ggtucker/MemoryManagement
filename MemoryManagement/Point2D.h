#pragma once

#include <ostream>

class Point2D {
public:
	Point2D() : x{ 0.0f }, y{ 0.0f } {}
	Point2D(float x, float y) : x{ x }, y{ y } {}
	Point2D(const Point2D& p) : x{ p.x }, y{ p.y } {}
	virtual ~Point2D() {}

	Point2D& operator=(const Point2D& other) {
		if (this != &other) {
			this->x = other.x;
			this->y = other.y;
		}
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Point2D& p);
	friend Point2D operator+(const Point2D& lhs, const Point2D& rhs);
	friend Point2D operator-(const Point2D& lhs, const Point2D& rhs);

	float getX() const { return x; }
	float getY() const { return y; }

private:
	float x, y;
};

std::ostream& operator<<(std::ostream& os, const Point2D& p) {
	os << "(" << p.x << "," << p.y << ")";
	return os;
}

Point2D operator+(const Point2D& lhs, const Point2D& rhs) {
	return Point2D(lhs.x + rhs.x, lhs.y + rhs.y);
}

Point2D operator-(const Point2D& lhs, const Point2D& rhs) {
	return Point2D(lhs.x - rhs.x, lhs.y - rhs.y);
}