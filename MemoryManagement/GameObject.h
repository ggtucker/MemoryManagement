#pragma once

#include "Point2D.h"

class GameObject {
public:
	size_t id;
	Point2D pos;

	friend std::ostream& operator<<(std::ostream& os, const GameObject& obj);
};

std::ostream& operator<<(std::ostream& os, const GameObject& obj) {
	os << "[" << obj.id << ":" << obj.pos << "]";
	return os;
}