#pragma once

#include "Point3D.h"

class Boundary3D {
public:
	Boundary3D() {}
	Boundary3D(Point3D center, Point3D halfLength) : center{ center }, halfLength{ halfLength } {}
	Boundary3D(const Boundary3D& b) : center{ b.center }, halfLength{ b.halfLength } {}
	virtual ~Boundary3D() {}

	Boundary3D& operator=(const Boundary3D& other) {
		if (this != &other) {
			this->center = other.center;
			this->halfLength = other.halfLength;
		}
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Boundary3D& b);

	bool contains(Point3D p) {
		Point3D min = getMin();
		Point3D max = getMax();
		return p.getX() < max.getX() && p.getX() >= min.getX()
			&& p.getY() < max.getY() && p.getY() >= min.getY()
			&& p.getZ() < max.getZ() && p.getZ() >= min.getZ();
	}

	bool intersects(Boundary3D other) {
		Point3D min = getMin();
		Point3D max = getMax();
		Point3D omin = other.getMin();
		Point3D omax = other.getMax();
		return !(max.getX() <= omin.getX() || omax.getX() <= min.getX()
			  || max.getY() <= omin.getY() || omax.getY() <= min.getY()
			  || max.getZ() <= omin.getZ() || omax.getZ() <= min.getZ());
	}

	Point3D getMin() { return center - halfLength; }
	Point3D getMax() { return center + halfLength; }

	Point3D getCenter() { return center; }
	Point3D getHalfLength() { return halfLength; }

private:
	Point3D center;
	Point3D halfLength;
};