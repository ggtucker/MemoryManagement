#pragma once

#include <mutex>

template<typename TObject, size_t PoolSize>
class TMemoryPool {
public:
	TMemoryPool() {
		firstAvailable_ = &pool_[0];

		for (size_t i = 0; i < PoolSize - 1; ++i) {
			pool_[i].next = &pool_[i + 1];
		}

		pool_[PoolSize - 1].next = nullptr;
	}

	TObject* create() {
		std::lock_guard<std::mutex> allocator_guard(mutex_);
		if (firstAvailable_) {
			TObject* obj = reinterpret_cast<TObject*>(firstAvailable_);
			firstAvailable_ = firstAvailable_->next;
			return obj;
		}
		return nullptr;
	}

	void deallocate(TObject* obj) {
		std::lock_guard<std::mutex> allocator_guard(mutex_);
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
	};
	PoolNode pool_[PoolSize];
	PoolNode* firstAvailable_;
	std::mutex mutex_;
};