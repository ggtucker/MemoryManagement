#include "TMemoryPool.h"
#include <iostream>
#include <vector>

int main() {
	TMemoryPool<int, 20> pool;
	for (int i = 0; i < 20; ++i) {
		int* num = pool.create();
		//*num = i;
		std::cout << num << " -> 0x" << std::hex << *num << std::endl;
	}
	//pool.deallocate(num);
}