#include <algorithm>
#include <functional>
template <typename T, typename Compare = std::less<T>> class priorityQueue {
public:
	explicit priorityQueue(const Compare &compare = Compare())
	    : compare{ compare } {};

	void push(T element) {
		this->elements.push_back(std::move(element));
		std::push_heap(elements.begin(), elements.end(), compare);
	}

	T pop() {
		std::pop_heap(elements.begin(), elements.end(), compare);
		T top = std::move(elements.back());
		elements.pop_back();
		return top;
	}

	const T &peek() const { return elements.front(); }

	void clear() { elements.clear(); }

	inline bool empty() { return elements.empty(); }

private:
	std::vector<T> elements;
	Compare compare;
};