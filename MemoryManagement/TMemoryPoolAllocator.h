#pragma once

#include <cassert>
#include <utility>
#include "TMemoryPool.h"

template <typename T, size_t PoolSize>
class TMemoryPoolAllocator {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	template <typename Type>
	struct rebind {
		using other = Type;
	};

	pointer address(reference r) const {
		return &r;
	}

	const_pointer address(const_reference r) const {
		return &r;
	}

	size_t max_size() const {
		return PoolSize;
	}

	bool operator!=(const TMemoryPoolAllocator& other) const {
		return !(*this == other);
	}

	bool operator==(const TMemoryPoolAllocator& other) const {
		return false;
	}

	TMemoryPoolAllocator() {}
	TMemoryPoolAllocator(const TMemoryPoolAllocator&) {}
	template <typename U> TMemoryPoolAllocator(const TMemoryPoolAllocator<U>&) {}
	~TMemoryPoolAllocator() {}

	pointer allocate(size_t n) {
		assert(n == 1);
		return pool_.create();
	}

	void deallocate(pointer p, size_t n) {
		assert(n == 1);
		pool_.deallocate(p);
	}

	template <class U, class... Args>
	void construct(U* p, Args&&... args) {
		new ((void*)p) U(std::forward<Args>(args));
	}

	void destroy(pointer p) {
		p->~T();
	}

private:
	TMemoryPool<T, PoolSize> pool_;
};