#pragma once

#include <iostream>
#include <vector>
#include "GameObject.h"
#include "TMemoryPool.h"
#include "Boundary2D.h"

template<size_t PoolSize = 256>
class QuadTree {
public:
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

	bool insert(const GameObject& obj) {
		return insert_(root_, obj);
	}

	bool remove(const GameObject& obj) {
		return remove_(root_, obj);
	}

	std::vector<GameObject> queryRange(const Boundary2D& range) const {
		return query_range_(root_, range);
	}

	void print() const {
		print_(root_);
	}

private:
	struct TreeNode {
		std::vector<GameObject> objects;
		Boundary2D boundary;
		TreeNode* nw;
		TreeNode* ne;
		TreeNode* sw;
		TreeNode* se;
		TreeNode(size_t capacity) : nw { nullptr }, ne{ nullptr }, sw{ nullptr }, se{ nullptr } {
			objects.reserve(capacity);
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

	bool insert_(TreeNode* node, const GameObject& obj) {
		// Object cannot be added: not within boundary
		if (!node->boundary.contains(obj.pos)) {
			return false;
		}
		
		// Only add object to leaf node
		if (is_leaf_(node)) {
			// Added if not at capacity
			if (node->objects.size() < capacity_) {
				node->objects.push_back(obj);
				return true;
			}
			// Subdivide if at capacity
			// If can't subdivide, just add to this node
			if (!subdivide_(node)) {
				node->objects.push_back(obj);
				return true;
			}
		}

		if (insert_(node->nw, obj)) return true;
		if (insert_(node->ne, obj)) return true;
		if (insert_(node->sw, obj)) return true;
		if (insert_(node->se, obj)) return true;

		// Object cannot be added for some reason (this should never happen)
		return false;
	}

	bool remove_(TreeNode* node, const GameObject& obj) {
		// Object cannot be removed: not within boundary
		if (!node->boundary.contains(obj.pos)) {
			return false;
		}

		// Remove object if this quad is leaf and its present
		if (is_leaf_(node)) {
			std::vector<GameObject>& objects = node->objects;
			std::vector<GameObject>::iterator newEnd = std::remove(objects.begin(), objects.end(), obj);
			if (newEnd != objects.end()) {
				objects.erase(newEnd, objects.end());
				return true;
			}
			return false;
		}

		// Remove object from children and unsubdivide if possible
		if (remove_(node->nw, obj) || remove_(node->ne, obj) || remove_(node->sw, obj) || remove_(node->se, obj)) {
			unsubdivide_(node);
			return true;
		}

		// Object to remove was not found in the children
		return false;
	}

	std::vector<GameObject> query_range_(TreeNode* node, const Boundary2D& range) const {
		std::vector<GameObject> objectsInRange;

		// Abort if range does not intersect this quad's boundary
		if (!node->boundary.intersects(range)) {
			return objectsInRange;
		}

		// Check the objects at this quad level
		if (is_leaf_(node)) {
			const std::vector<GameObject>& objects = node->objects;
			for (size_t i = 0; i < objects.size(); ++i) {
				if (range.contains(objects[i].pos)) {
					objectsInRange.push_back(objects[i]);
				}
			}
			return objectsInRange;
		}

		// Add the objects from the children
		std::vector<GameObject> nwObjects = query_range_(node->nw, range);
		std::vector<GameObject> neObjects = query_range_(node->ne, range);
		std::vector<GameObject> swObjects = query_range_(node->sw, range);
		std::vector<GameObject> seObjects = query_range_(node->se, range);
		objectsInRange.insert(objectsInRange.end(), nwObjects.begin(), nwObjects.end());
		objectsInRange.insert(objectsInRange.end(), neObjects.begin(), neObjects.end());
		objectsInRange.insert(objectsInRange.end(), swObjects.begin(), swObjects.end());
		objectsInRange.insert(objectsInRange.end(), seObjects.begin(), seObjects.end());

		return objectsInRange;
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
		for (const GameObject& obj : node->objects) {
			insert_(node->nw, obj);
			insert_(node->ne, obj);
			insert_(node->sw, obj);
			insert_(node->se, obj);
		}
		node->objects.clear();
		return true;
	}

	bool unsubdivide_(TreeNode* node) {
		// Is parent node of leaf nodes
		if (!is_leaf_(node) && is_leaf_(node->nw)) {
			size_t childSum = size_(node);
			if (childSum <= capacity_) {
				const std::vector<GameObject>& objects = node->objects;
				const std::vector<GameObject>& nwObjects = node->nw->objects;
				const std::vector<GameObject>& neObjects = node->ne->objects;
				const std::vector<GameObject>& swObjects = node->sw->objects;
				const std::vector<GameObject>& seObjects = node->se->objects;
				objects.insert(objects.end(), nwObjects.begin(), nwObjects.end());
				objects.insert(objects.end(), neObjects.begin(), neObjects.end());
				objects.insert(objects.end(), swObjects.begin(), swObjects.end());
				objects.insert(objects.end(), seObjects.begin(), seObjects.end());
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
			return node->objects.size() + size_(node->nw) + size_(node->ne) + size_(node->sw) + size_(node->se);
		}
		return 0;
	}

	TreeNode* copy_tree_(TreeNode* node) const {
		if (node) {
			TreeNode* copied = create_node_(node->boundary);
			copied->objects = node->objects;
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
			if (is_leaf_(node) && !node->objects.empty()) {
				std::cout << node->boundary << " size(" << node->objects.size() << "): ";
				for (const GameObject& obj : node->objects) {
					std::cout << obj;
				}
				std::cout << std::endl;
			}
		}
	}
};