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
	QuadTree<> qtree(quadBoundary, quadCapacity, minQuadSize);
	srand(static_cast <unsigned> (time(0)));
	for (int i = 0; i < 50; ++i) {
		GameObject obj;
		obj.id = i;
		float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f;
		float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f;
		obj.pos = Point2D(x, y);
		qtree.insert(obj);
	}

	for (int i = 0; i < 20; ++i) {
		GameObject obj;
		obj.id = 1337 + i;
		obj.pos = Point2D(0.0f, 0.0f);
		qtree.insert(obj);
	}

	qtree.print();

	std::vector<GameObject> objects = qtree.queryRange(Boundary2D(Point2D(100.0f, 100.0f), Point2D(20.0f, 20.0f)));
	for (const GameObject& obj : objects) {
		std::cout << "In Range: " << obj << std::endl;
	}
}