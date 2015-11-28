#pragma once

#include <ostream>
#include "Point2D.h"

class Boundary2D {
public:
	Boundary2D() {}
	Boundary2D(Point2D center, Point2D halfLength) : center{ center }, halfLength{ halfLength } {}
	Boundary2D(const Boundary2D& b) : center{ b.center }, halfLength{ b.halfLength } {}
	virtual ~Boundary2D() {}

	Boundary2D& operator=(const Boundary2D& other) {
		if (this != &other) {
			this->center = other.center;
			this->halfLength = other.halfLength;
		}
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Boundary2D& b);

	bool contains(const Point2D& p) const {
		Point2D min = getMin();
		Point2D max = getMax();
		return p.getX() < max.getX() && p.getX() >= min.getX()
			&& p.getY() < max.getY() && p.getY() >= min.getY();
	}

	bool intersects(const Boundary2D& other) const {
		Point2D min = getMin();
		Point2D max = getMax();
		Point2D omin = other.getMin();
		Point2D omax = other.getMax();
		return !(max.getX() <= omin.getX() || omax.getX() <= min.getX()
			  || max.getY() <= omin.getY() || omax.getY() <= min.getY());
	}

	Point2D getMin() const { return center - halfLength; }
	Point2D getMax() const { return center + halfLength; }

	Point2D getCenter() const { return center; }
	Point2D getHalfLength() const { return halfLength; }

	Point2D getHalfHalfLength() const {
		return Point2D(halfLength.getX() / 2.0f, halfLength.getY() / 2.0f);
	}

	Boundary2D getNW() const {
		Point2D hhLength = getHalfHalfLength();
		Point2D nwCenter = getMin() + hhLength;
		return Boundary2D(nwCenter, hhLength);
	}

	Boundary2D getNE() const {
		Point2D hhLength = getHalfHalfLength();
		Point2D neCenter = Point2D(center.getX(), getMin().getY()) + hhLength;
		return Boundary2D(neCenter, hhLength);
	}

	Boundary2D getSW() const {
		Point2D hhLength = getHalfHalfLength();
		Point2D swCenter = Point2D(getMin().getX(), center.getY()) + hhLength;
		return Boundary2D(swCenter, hhLength);
	}

	Boundary2D getSE() const {
		Point2D hhLength = getHalfHalfLength();
		Point2D seCenter = center + hhLength;
		return Boundary2D(seCenter, hhLength);
	}

private:
	Point2D center;
	Point2D halfLength;
};

std::ostream& operator<<(std::ostream& os, const Boundary2D& b) {
	os << "Bdry(center:" << b.center << ", halfLength:" << b.halfLength << ")";
	return os;
}