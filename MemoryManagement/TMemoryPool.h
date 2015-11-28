#pragma once

#include <mutex>
#include <array>
#include <cassert>

template<typename TObject, size_t PoolSize>
class TMemoryPool {
public:
	TMemoryPool() {
		init_pool_();
	}

	TMemoryPool(const TMemoryPool& other) : pool_{ other.pool_ } {
		init_pool_();
	}

	virtual ~TMemoryPool() {}

	TMemoryPool& operator=(const TMemoryPool& other) {
		if (this != &other) {
			std::lock(mutex_, other.mutex_);
			std::lock_guard<std::mutex> lhs_lock(mutex_, std::adopt_lock);
			std::lock_guard<std::mutex> rhs_lock(other.mutex_, std::adopt_lock);
			pool_ = other.pool_;
			init_pool_();
		}
		return *this;
	}

	TObject* create() {
		std::lock_guard<std::mutex> lock(mutex_);
		assert(firstAvailable_);
		TObject* obj = reinterpret_cast<TObject*>(firstAvailable_);
		firstAvailable_ = firstAvailable_->next;
		return new(obj) TObject();
	}

	void deallocate(TObject* obj) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (obj) {
			PoolNode* node = reinterpret_cast<PoolNode*>(obj);
			node->next = firstAvailable_;
			firstAvailable_ = node;
		}
	}

private:
	union PoolNode {
		TObject obj;
		PoolNode* next;
		PoolNode() {}
		~PoolNode() {}
	};
	std::array<PoolNode, PoolSize> pool_;
	PoolNode* firstAvailable_;
	std::mutex mutex_;

	void init_pool_() {
		firstAvailable_ = &pool_[0];
		for (size_t i = 0; i < PoolSize - 1; ++i) {
			pool_[i].next = &pool_[i + 1];
		}
		pool_[PoolSize - 1].next = nullptr;
	}
};