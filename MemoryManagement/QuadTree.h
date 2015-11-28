#pragma once

#include <iostream>
#include <vector>
#include "TMemoryPool.h"
#include "Boundary2D.h"

template<typename TObject, size_t PoolSize = 256>
class QuadTree {
public:
	struct Entry {
		TObject obj;
		Point2D pos;
		Entry(TObject obj, Point2D pos) : obj{ obj }, pos{ pos } {}
		~Entry() {}
		bool operator==(const Entry& other) const {
			return this->obj == other.obj;
		}
	};

	QuadTree() : min_quad_{ 1.0f, 1.0f } {
		root_ = create_node_(Boundary2D());
	}

	QuadTree(const Boundary2D& boundary, size_t capacity, const Point2D& minQuad) : capacity_{ capacity }, min_quad_{ minQuad } {
		root_ = create_node_(boundary);
	}

	QuadTree(const QuadTree& other) : capacity_{ other.capacity_ }, min_quad_{ other.min_quad_ } {
		root_ = copy_tree_(other.root_);
	}

	virtual ~QuadTree() {}

	QuadTree& operator=(const QuadTree& other) {
		if (this != &other) {
			this->root_ = copy_tree_(other.root_);
			this->capacity_ = other.capacity_;
			this->min_quad_ = other.min_quad_;
		}
		return *this;
	}

	void clear() {
		clear_(root_);
	}

	size_t size() const {
		return size_(root_);
	}

	bool insert(const TObject& obj, const Point2D& pos) {
		return insert_(root_, Entry(obj, pos));
	}

	bool insert(const Entry& entry) {
		return insert_(root_, entry);
	}

	bool remove(const TObject& obj, const Point2D& pos) {
		return remove_(root_, Entry(obj, pos));
	}

	bool remove(const Entry& entry) {
		return remove_(root_, entry);
	}

	std::vector<Entry> queryRange(const Boundary2D& range) const {
		return query_range_(root_, range);
	}

	void print() const {
		print_(root_);
	}

private:
	struct TreeNode {
		std::vector<Entry> entries;
		Boundary2D boundary;
		TreeNode* nw;
		TreeNode* ne;
		TreeNode* sw;
		TreeNode* se;
		TreeNode(size_t capacity) : nw { nullptr }, ne{ nullptr }, sw{ nullptr }, se{ nullptr } {
			entries.reserve(capacity);
		}
		~TreeNode() {}
	};

	TMemoryPool<TreeNode, PoolSize> pool_;
	TreeNode* root_;
	size_t capacity_;
	Point2D min_quad_;

	TreeNode* create_node_(const Boundary2D& boundary) {
		TreeNode* node = pool_.create<size_t>(std::move(capacity_));
		node->boundary = boundary;
		return node;
	}

	bool is_leaf_(TreeNode* node) const {
		return node != nullptr && node->nw == nullptr;
	}

	bool insert_(TreeNode* node, const Entry& entry) {
		// entryect cannot be added: not within boundary
		if (!node->boundary.contains(entry.pos)) {
			return false;
		}
		
		// Only add entryect to leaf node
		if (is_leaf_(node)) {
			// Added if not at capacity
			if (node->entries.size() < capacity_) {
				node->entries.push_back(entry);
				return true;
			}
			// Subdivide if at capacity
			// If can't subdivide, just add to this node
			if (!subdivide_(node)) {
				node->entries.push_back(entry);
				return true;
			}
		}

		if (insert_(node->nw, entry)) return true;
		if (insert_(node->ne, entry)) return true;
		if (insert_(node->sw, entry)) return true;
		if (insert_(node->se, entry)) return true;

		// entryect cannot be added for some reason (this should never happen)
		return false;
	}

	bool remove_(TreeNode* node, const Entry& entry) {
		// entryect cannot be removed: not within boundary
		if (!node->boundary.contains(entry.pos)) {
			return false;
		}

		// Remove entryect if this quad is leaf and its present
		if (is_leaf_(node)) {
			std::vector<Entry>& entries = node->entries;
			std::vector<Entry>::iterator newEnd = std::remove(entries.begin(), entries.end(), entry);
			if (newEnd != entries.end()) {
				entries.erase(newEnd, entries.end());
				return true;
			}
			return false;
		}

		// Remove entryect from children and unsubdivide if possible
		if (remove_(node->nw, entry) || remove_(node->ne, entry) || remove_(node->sw, entry) || remove_(node->se, entry)) {
			unsubdivide_(node);
			return true;
		}

		// entryect to remove was not found in the children
		return false;
	}

	std::vector<Entry> query_range_(TreeNode* node, const Boundary2D& range) const {
		std::vector<Entry> entriesInRange;

		// Abort if range does not intersect this quad's boundary
		if (!node->boundary.intersects(range)) {
			return entriesInRange;
		}

		// Check the entries at this quad level
		if (is_leaf_(node)) {
			const std::vector<Entry>& entries = node->entries;
			for (size_t i = 0; i < entries.size(); ++i) {
				if (range.contains(entries[i].pos)) {
					entriesInRange.push_back(entries[i]);
				}
			}
			return entriesInRange;
		}

		// Add the entries from the children
		std::vector<Entry> nwEntries = query_range_(node->nw, range);
		std::vector<Entry> neEntries = query_range_(node->ne, range);
		std::vector<Entry> swEntries = query_range_(node->sw, range);
		std::vector<Entry> seEntries = query_range_(node->se, range);
		entriesInRange.insert(entriesInRange.end(), nwEntries.begin(), nwEntries.end());
		entriesInRange.insert(entriesInRange.end(), neEntries.begin(), neEntries.end());
		entriesInRange.insert(entriesInRange.end(), swEntries.begin(), swEntries.end());
		entriesInRange.insert(entriesInRange.end(), seEntries.begin(), seEntries.end());

		return entriesInRange;
	}

	bool subdivide_(TreeNode* node) {
		const Boundary2D& boundary = node->boundary;
		Point2D hhLength = boundary.getHalfHalfLength();
		if (hhLength.getX() <= min_quad_.getX() || hhLength.getY() <= min_quad_.getY()) {
			return false;
		}
		node->nw = create_node_(boundary.getNW());
		node->ne = create_node_(boundary.getNE());
		node->sw = create_node_(boundary.getSW());
		node->se = create_node_(boundary.getSE());
		for (const Entry& entry : node->entries) {
			insert_(node->nw, entry);
			insert_(node->ne, entry);
			insert_(node->sw, entry);
			insert_(node->se, entry);
		}
		node->entries.clear();
		return true;
	}

	bool unsubdivide_(TreeNode* node) {
		// Is parent node of leaf nodes
		if (!is_leaf_(node) && is_leaf_(node->nw)) {
			size_t childSum = size_(node);
			if (childSum <= capacity_) {
				const std::vector<Entry>& entries = node->entries;
				const std::vector<Entry>& nwEntries = node->nw->entries;
				const std::vector<Entry>& neEntries = node->ne->entries;
				const std::vector<Entry>& swEntries = node->sw->entries;
				const std::vector<Entry>& seEntries = node->se->entries;
				entries.insert(entries.end(), nwEntries.begin(), nwEntries.end());
				entries.insert(entries.end(), neEntries.begin(), neEntries.end());
				entries.insert(entries.end(), swEntries.begin(), swEntries.end());
				entries.insert(entries.end(), seEntries.begin(), seEntries.end());
				delete_children_(node);
				return true;
			}
		}
		return false;
	}

	void delete_children_(TreeNode* node) {
		if (node) {
			pool_.deallocate(node->nw);
			pool_.deallocate(node->ne);
			pool_.deallocate(node->sw);
			pool_.deallocate(node->se);
			node->nw = nullptr;
			node->ne = nullptr;
			node->sw = nullptr;
			node->se = nullptr;
		}
	}

	void clear_(TreeNode* node) {
		if (node) {
			clear_(node->nw);
			clear_(node->ne);
			clear_(node->sw);
			clear_(node->se);
			pool_.deallocate(node);
		}
	}

	size_t size_(TreeNode* node) const {
		if (node) {
			return node->entries.size() + size_(node->nw) + size_(node->ne) + size_(node->sw) + size_(node->se);
		}
		return 0;
	}

	TreeNode* copy_tree_(TreeNode* node) const {
		if (node) {
			TreeNode* copied = create_node_(node->boundary);
			copied->entries = node->entries;
			copied->nw = copy_tree_(node->nw);
			copied->ne = copy_tree_(node->ne);
			copied->sw = copy_tree_(node->sw);
			copied->se = copy_tree_(node->se);
			return copied;
		}
		return nullptr;
	}

	void print_(TreeNode* node) const {
		if (node) {
			print_(node->nw);
			print_(node->ne);
			print_(node->sw);
			print_(node->se);
			if (is_leaf_(node) && !node->entries.empty()) {
				std::cout << node->boundary << " size(" << node->entries.size() << "): ";
				for (const Entry& entry : node->entries) {
					std::cout << "[" << entry.obj << ":" << entry.pos << "]";
				}
				std::cout << std::endl;
			}
		}
	}
};