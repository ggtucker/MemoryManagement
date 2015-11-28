#pragma once

#include <ostream>

class Point3D {
public:
	Point3D() : x{ 0.0f }, y{ 0.0f }, z{ 0.0f } {}
	Point3D(float x, float y, float z) : x{ x }, y{ y }, z{ z } {}
	Point3D(const Point3D& p) : x{ p.x }, y{ p.y }, z{ p.z } {}
	virtual ~Point3D() {}

	Point3D& operator=(const Point3D& other) {
		if (this != &other) {
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
		}
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Point3D& p);
	friend Point3D operator+(const Point3D& lhs, const Point3D& rhs);
	friend Point3D operator-(const Point3D& lhs, const Point3D& rhs);

	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }

private:
	float x, y, z;
};

std::ostream& operator<<(std::ostream& os, const Point3D& p) {
	os << "(" << p.x << "," << p.y << "," << p.z << ")";
	return os;
}

Point3D operator+(const Point3D& lhs, const Point3D& rhs) {
	return Point3D(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Point3D operator-(const Point3D& lhs, const Point3D& rhs) {
	return Point3D(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}