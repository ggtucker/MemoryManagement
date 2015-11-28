#include "TMemoryPool.h"
#include "QuadTree.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

int main() {
	/*TMemoryPool<int, 20> pool;
	for (int i = 0; i < 20; ++i) {
		int* num = pool.create();
		*num = i;
		std::cout << num << " -> " << *num << std::endl;
	}*/

	Boundary2D quadBoundary(Point2D(100.0f, 100.0f), Point2D(100.0f, 100.0f));
	size_t quadCapacity = 5;
	Point2D minQuadSize(10.0f, 10.0f);
	QuadTree<size_t> qtree(quadBoundary, quadCapacity, minQuadSize);
	srand(static_cast <unsigned> (time(0)));
	for (int i = 0; i < 50; ++i) {
		size_t id = i;
		float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f;
		float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f;
		Point2D pos(x, y);
		qtree.insert(id, pos);
	}

	for (int i = 0; i < 20; ++i) {
		size_t id = 1337 + i;
		Point2D pos(0.0f, 0.0f);
		qtree.insert(id, pos);
	}

	qtree.print();
	
	Boundary2D targetRange(Point2D(100.0f, 100.0f), Point2D(20.0f, 20.0f));
	std::vector<QuadTree<size_t>::Entry> entries = qtree.queryRange(targetRange);
	std::cout << std::endl << "RangeQuery " << targetRange << ": ";
	for (const QuadTree<size_t>::Entry& entry : entries) {
		std::cout << "[" << entry.obj << ":" << entry.pos << "]";
	}
	std::cout << std::endl;
}