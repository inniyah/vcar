#ifndef SIMPLE_QUEUE_H_5A11FEAE_D5CF_11E5_A970_10FEED04CD1C
#define SIMPLE_QUEUE_H_5A11FEAE_D5CF_11E5_A970_10FEED04CD1C

#include <cstdlib>
#include <atomic>

template <typename T>
class SimpleQueue {
private:
	struct Node {
		Node(T item) : data(item), next(nullptr) { }
		T data;
		Node * next;
	};

	Node * first_node; // No need to make this atomic, as only the Producer will use it
	std::atomic<Node*> barrier_node;
	std::atomic<Node*> last_node;

public:
	SimpleQueue() {
		// The list will initially contain the separator
		first_node = barrier_node = last_node = new Node(T());
	}

	~SimpleQueue() {
		// Remove all the nodes in the list
		while (first_node != nullptr) {
			Node * node = first_node;
			first_node = node->next;
			delete node;
		}
	}

	void push(const T & item) {
		last_node->next = new Node(item);
		last_node  = last_node->next;
		// Remove unused nodes
		while (first_node != barrier_node) {
			Node* tmp = first_node;
			first_node = first_node->next;
			delete tmp;
		}
	}

	bool pop(T & item) {
		if (barrier_node != last_node) {
			item = barrier_node->next->data;
			barrier_node = barrier_node->next;
			return true;
		}
		return false; // The queue is empty
	}
};

#endif // SIMPLE_QUEUE_H_5A11FEAE_D5CF_11E5_A970_10FEED04CD1C
