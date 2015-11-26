#include "TMemoryPool.h"
#include <iostream>
#include <vector>

int main() {
	TMemoryPool<int, 20> pool;
	for (int i = 0; i < 100; ++i) {
		int* num = pool.create();
		*num = i;
		std::cout << num << " -> " << *num << std::endl;
	}
	//pool.deallocate(num);
}