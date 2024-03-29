#ifndef BASICS_TREAP_HPP
#define BASICS_TREAP_HPP

#include <utility>
#include <memory>
#include <chrono>
#include <random>
#include <stack>
#include <treap_base.hpp>

namespace nstd {

template <typename Node, typename Compare, typename Allocator>
class treap : public treap_base<Node, Allocator> {
    using base_type = treap_base<Node, Allocator>;
    using treap_node = Node;
    using node_holder = typename base_type::node_holder;

public:
    using key_type = typename treap_node::raw_key_type;
    using value_type = typename base_type::value_type;
    using key_compare = Compare;
    using allocator_type = typename base_type::allocator_type;
    using size_type = typename base_type::size_type;

public:
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using reverse_iterator = typename base_type::reverse_iterator;
    using const_reverse_iterator = typename base_type::const_reverse_iterator;

private:
    using base_type::_end;
    using base_type::_begin;
    key_compare _comparator;

private:
    using base_type::end_node;
    using base_type::root;
    using base_type::set_root;
    using base_type::adjust_begin;

public:
    explicit treap(const key_compare& comparator = key_compare(),
                   const allocator_type& allocator = allocator_type());

    treap(const treap& other);

    treap(treap&& other) noexcept;

    treap& operator=(const treap& other);

    treap& operator=(treap&& other) noexcept;

    ~treap() = default;

private:

    /**
     * Merges two nodes into one node
     * Working complexity is O(log size)
     * Provides strong exception safety
     * @param node1 node1
     * @param node2 node2
     * @return merged node, in case of one of the passed node is nullptr, will be returned second one
     */
    treap_node* merge(treap_node* node1, treap_node* node2);

    /**
     * Splits passed node into two nodes with the given key
     * Working complexity is O(log size)
     * Provides strong exception safety
     *
     * @tparam KeyIncluded In case of KeyIncluded parameter is true, node having passed key will be in left tree, in the right otherwise
     * @param node splittable node
     * @param key key to be seperated with
     * @return node pair, where the first tree will consist of nodes, where key is less than passed key
     *         And the second tree will consist of nodes, where key is greater than passed key
     *         The node having passed key will be one of this two trees depending on KeyIncluded template parameter
     */
    template <bool KeyIncluded = false>
    std::pair<treap_node*, treap_node*> split(treap_node* node, const key_type& key);

    /**
     * Using split and merge functions
     * Inserts node in tree
     * @param node node to be inserted
     * @return iterator pointing inserted node
     */
    iterator insert_node(treap_node* node);


    /**
     * Returns tree including all the nodes, which has key between passed key interval
     * Detaches that tree from the main tree
     * Works in O(log size) complexity
     * Provides weak exception guarantee if comparator throws exception during comparisons
     * @tparam EndIncluded boolean parameter showing node with key_end would be in the returned tree or not
     * @param begin_key interval begin key (inclusive endpoint)
     * @param end_key interval end key (inclusive or exclusive endpoint depend on EndIncluded parameter
     * @return proper tree if there exists any node between interval, nullptr otherwise
     */
    template <bool EndIncluded = false>
    treap_node* detach_node_key_interval(const key_type& begin_key, const key_type& end_key);

    /**
     * Returns node with the passed key
     * Detaches that node from the tree
     * Works in O(log size) complexity
     * Provides weak exception guarantee if comparator throws exception during comparisons
     * @param key key
     * @return proper node if there exists node with the passed key, nullptr otherwise
     */
    treap_node* detach_node_with_key(const key_type& key);

public:
    void swap(treap& other) noexcept;

    std::pair<iterator, bool> insert(const value_type& value);

    std::pair<iterator, bool> insert(value_type&& value);

    template <typename InputIterator>
    void insert(InputIterator begin, InputIterator end);

    void insert(std::initializer_list<value_type> il);

    /**
     * Inserts a node in the tree with the value constructed with passed arguments
     * If the key already exists, nothing happens
     * Working complexity is O (log size)
     * Provides weak exception safety in case of comparator comparison throws exception
     * @param args args
     * @return pair, where the first one is inserted iterator and the second one is boolean showing whether the key was actually inserted or not
     */
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&& ... args);

private:
    template <typename... Args>
    std::pair<iterator, bool> emplace_with_key(const key_type& key, Args&& ... args);

public:
    /**
     * Erases passed key interval from the main tree
     * Works in O (end - begin + log size) complexity
     * Provides weak exception guarantee if comparator comparisons throw exception
     * @param begin_key begin key (inclusive endpoint)
     * @param end_key end key (exclusive endpoint)
     * @returns iterator pointing on first node after interval
     */
    iterator erase_key_interval(const key_type& begin_key, const key_type& end_key);

    /**
     * Erases passed key interval from the main tree
     * Works in O (end - begin + log size) complexity
     * Provides weak exception guarantee if comparator comparisons throw exception
     * @param begin_key begin key (inclusive endpoint)
     * @param end_key end key (inclusive endpoint)
     * @returns iterator pointing on first node after interval
     */
    iterator erase_key_interval_with_end(const key_type& begin_key, const key_type& end_key);

    /**
     * Erases the node with the passed key
     * If there is no node having passed key, then nothing happens
     * Working complexity is O (log size)
     * Provides weak exception safety in case of comparator comparison throws exception
     * @returns iterator pointing on node after key
     * @param key
     */
    iterator erase_key(const key_type& key);

    using base_type::erase;

    bool contains(const key_type& key) const;

    /**
     * Returns iterator with the passed key
     * Returns end iterator with the key wasn't found
     * @param key
     * @return
     */
    iterator find(const key_type& key);

    const_iterator find(const key_type& key) const;

    /**
     * Returns first iterator having greater or equal key than passed key
     * @param key
     * @return iterator
     */
    iterator lower_bound(const key_type& key);

    const_iterator lower_bound(const key_type& key) const;

    /**
     * Returns first iterator having greater key than passed key
     * @param key
     * @return
     */
    iterator upper_bound(const key_type& key);

    const_iterator upper_bound(const key_type& key) const;

    /**
     * Returns the key, which is located in the passed index
     * Works in O (log size) complexity
     * @param index index
     * @return proper key when index < size
     * Throws std::out_of_range exception otherwise
     */
    const key_type& key_of_order(size_type index) const;

    /**
     * Returns index of the passed key
     * Works in O (log size) complexity
     * @param key key
     * @return proper index, when the tree has the key, size() otherwise
     */
    size_type order_of_key(const key_type& key) const;

    using base_type::size;

    using base_type::empty;

private:
    /**
     * Returns node with the passed key
     * Works in O (log size) complexity
     * @param key key
     * @return proper ndoe, when the tree has the key, end node otherwise
     */
    treap_node* node_of_key(const key_type& key);

    const treap_node* node_of_key(const key_type& key) const;

    treap_node* lower_bound_node(const key_type& key);

    const treap_node* lower_bound_node(const key_type& key) const;

    treap_node* upper_bound_node(const key_type& key);

    const treap_node* upper_bound_node(const key_type& key) const;

public:
    using base_type::begin;

    using base_type::cbegin;

    using base_type::rbegin;

    using base_type::crbegin;

    using base_type::end;

    using base_type::cend;

    using base_type::rend;

    using base_type::crend;

};

template <typename Node, typename Compare, typename Allocator>
treap<Node, Compare, Allocator>::treap(const key_compare& comparator, const allocator_type& allocator)
        : base_type(allocator), _comparator(comparator) {}

template <typename Node, typename Compare, typename Allocator>
treap<Node, Compare, Allocator>::treap(const treap& other)
        : base_type(other), _comparator(other._comparator) {
    for (auto it = other.begin(); it != other.end(); ++it) {
        insert(*it);
    }
}

template <typename Node, typename Compare, typename Allocator>
treap<Node, Compare, Allocator>::treap(treap&& other) noexcept
        : base_type(std::move(other)), _comparator(std::move(other._comparator)) {}

template <typename Node, typename Compare, typename Allocator>
treap<Node, Compare, Allocator>&
treap<Node, Compare, Allocator>::operator=(const treap& other) {
    if (this != &other) {
        treap copied(other);
        this->swap(copied);
    }
    return *this;
}

template <typename Node, typename Compare, typename Allocator>
treap<Node, Compare, Allocator>&
treap<Node, Compare, Allocator>::operator=(treap&& other) noexcept {
    if (this != &other) {
        treap moved(std::move(other));
        this->swap(moved);
    }
    return *this;
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::treap_node*
treap<Node, Compare, Allocator>::merge(treap_node* node1, treap_node* node2) {
    // return nullptr when both values are null pointers
    if (node1 == nullptr && node2 == nullptr) {
        return nullptr;
    }

    // stack for collecting node values
    std::stack<treap_node*> nodes;
    // stack for collecting priority comparisons
    std::stack<bool> compares;
    while (node1 != nullptr && node2 != nullptr) {
        // after this operator we can suppose that node1.key <= node2.key
        if (_comparator(node2->get_key(), node1->get_key())) {
            std::swap(node1, node2);
        }
        // save comparison result in stack
        bool compare = node1->get_priority() > node2->get_priority();
        compares.push(compare);
        // save corresponding node and update node1 and node2
        if (compare) {
            nodes.push(node1);
            node1 = node1->get_right();
            continue;
        }
        nodes.push(node2);
        node2 = node2->get_left();
    }
    // child will be non-null node among node1 and node2
    treap_node* child = (node1 == nullptr ? node2 : node1);
    // child will be the last merge result
    while (!compares.empty()) {
        // parent of the child node
        treap_node* parent_node = nodes.top();
        nodes.pop();
        // comparison result
        bool compare = compares.top();
        compares.pop();
        // merge parent_node with child
        if (compare) {
            parent_node->set_right(child);
            child = parent_node;
            continue;
        }
        parent_node->set_left(child);
        child = parent_node;
    }
    return child;
}

template <typename Node, typename Compare, typename Allocator>
template <bool KeyIncluded>
auto
treap<Node, Compare, Allocator>::split(treap_node* node,
                                       const key_type& key) -> std::pair<treap_node*, treap_node*> {
    std::stack<treap_node*> nodes;
    std::stack<bool> compares;
    // gather all splittable nodes in stack
    nodes.push(node);
    while (nodes.top() != nullptr) {
        auto* top = nodes.top();
        bool compare = (KeyIncluded ? !_comparator(key, top->get_key()) : _comparator(top->get_key(), key));
        nodes.push(compare ? top->get_right() : top->get_left());
        // store comparison results in separate stack
        compares.push(compare);
    }
    // here we need two null pointers at the top of the stack
    nodes.push(nullptr);
    // in this point nodes count is greater comparison count by 2
    // so when the compares stack is empty, in nodes stack will be left two nodes, which are split results
    while (!compares.empty()) {
        bool compare = compares.top();
        auto* second = nodes.top();
        compares.pop(), nodes.pop();
        auto* first = nodes.top();
        nodes.pop();
        if (compare) {
            nodes.top()->set_right(first);
            nodes.push(second);
            continue;
        }
        nodes.top()->set_left(second);
        std::swap(nodes.top(), first);
        nodes.push(first);
    }
    // return last two nodes in nodes stack
    auto* second = nodes.top();
    nodes.pop();
    auto* first = nodes.top();
    return {first, second};
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::iterator treap<Node, Compare, Allocator>::insert_node(treap_node* node) {
    auto [left, right] = split(root(), node->get_key());
    treap_node* root = merge(merge(left, node), right);
    set_root(root);
    if (left == nullptr) {
        _begin = node;
    }
    return {node};
}

template <typename Node, typename Compare, typename Allocator>
template <bool EndIncluded>
typename treap<Node, Compare, Allocator>::treap_node*
treap<Node, Compare, Allocator>::detach_node_key_interval(const key_type& begin_key, const key_type& end_key) {
    auto [left, begin_included_tree] = split(root(), begin_key);
    auto [interval, right] = split<EndIncluded>(begin_included_tree, end_key);
    treap_node* root = merge(left, right);
    set_root(root);
    if (left == nullptr && interval != nullptr) {
        adjust_begin();
    }
    return interval;
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::treap_node*
treap<Node, Compare, Allocator>::detach_node_with_key(const key_type& key) {
    return detach_node_key_interval<true>(key, key);
}

template <typename Node, typename Compare, typename Allocator>
void treap<Node, Compare, Allocator>::swap(treap<Node, Compare, Allocator>& other) noexcept {
    base_type::swap(other);
    std::swap(_comparator, other._comparator);
}

template <typename Node, typename Compare, typename Allocator>
std::pair<typename treap<Node, Compare, Allocator>::iterator, bool>
treap<Node, Compare, Allocator>::insert(const value_type& value) {
    return emplace_with_key(treap_node::get_key(value), value);
}

template <typename Node, typename Compare, typename Allocator>
std::pair<typename treap<Node, Compare, Allocator>::iterator, bool>
treap<Node, Compare, Allocator>::insert(value_type&& value) {
    return emplace_with_key(treap_node::get_key(value), std::move(value));
}

template <typename Node, typename Compare, typename Allocator>
template <typename InputIterator>
void treap<Node, Compare, Allocator>::insert(InputIterator begin, InputIterator end) {
    for (; begin != end; ++begin) {
        emplace(*begin);
    }
}

template <typename Node, typename Compare, typename Allocator>
void treap<Node, Compare, Allocator>::insert(std::initializer_list<value_type> il) {
    insert(il.begin(), il.end());
}

template <typename Node, typename Compare, typename Allocator>
template <typename... Args>
std::pair<typename treap<Node, Compare, Allocator>::iterator, bool>
treap<Node, Compare, Allocator>::emplace(Args&& ... args) {
    // allocate memory for node and construct value
    node_holder holder = base_type::construct_node(std::forward<Args>(args)...);
    // if the tree already contains key, then just return
    auto it = find(holder->get_key());
    if (it != end()) {
        // node holder will automatically deallocate memory and destroy value
        return {it, false};
    }
    // insert new constructed node into tree and get iterator
    it = insert_node(holder.get());
    // release node holder, as insertion completed successfully
    holder.release();
    return {it, true};
}

template <typename Node, typename Compare, typename Allocator>
template <typename... Args>
std::pair<typename treap<Node, Compare, Allocator>::iterator, bool>
treap<Node, Compare, Allocator>::emplace_with_key(const key_type& key, Args&& ... args) {
    // if the tree already contains key, then just return
    auto it = find(key);
    if (it != end()) {
        return {it, false};
    }
    // allocate memory for node and construct value
    node_holder holder = base_type::construct_node(std::forward<Args>(args)...);
    // insert new constructed node into tree and get iterator
    it = insert_node(holder.get());
    // release node holder, as insertion completed successfully
    holder.release();
    return {it, true};
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::iterator
treap<Node, Compare, Allocator>::erase_key_interval(const key_type& begin_key, const key_type& end_key) {
    auto it = lower_bound(end_key);
    base_type::destroy_tree(detach_node_key_interval(begin_key, end_key));
    return it;
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::iterator
treap<Node, Compare, Allocator>::erase_key_interval_with_end(const key_type& begin_key, const key_type& end_key) {
    auto it = upper_bound(end_key);
    base_type::destroy_tree(detach_node_key_interval<true>(begin_key, end_key));
    return it;
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::iterator
treap<Node, Compare, Allocator>::erase_key(const key_type& key) {
    auto it = upper_bound(key);
    base_type::destroy_tree(detach_node_with_key(key));
    return it;
}

template <typename Node, typename Compare, typename Allocator>
bool treap<Node, Compare, Allocator>::contains(const key_type& key) const {
    return node_of_key(key) != end_node();
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::iterator
treap<Node, Compare, Allocator>::find(const key_type& key) {
    auto node = lower_bound_node(key);
    if (node != end_node() && !_comparator(key, node->get_key())) {
        return {node};
    }
    return end();
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::const_iterator
treap<Node, Compare, Allocator>::find(const key_type& key) const {
    auto node = lower_bound_node(key);
    if (node != end_node() && !_comparator(key, node->get_key())) {
        return {node};
    }
    return end();
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::iterator
treap<Node, Compare, Allocator>::lower_bound(const key_type& key) {
    return {lower_bound_node(key)};
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::const_iterator
treap<Node, Compare, Allocator>::lower_bound(const key_type& key) const {
    return {lower_bound_node(key)};
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::iterator
treap<Node, Compare, Allocator>::upper_bound(const key_type& key) {
    return {upper_bound_node(key)};
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::const_iterator
treap<Node, Compare, Allocator>::upper_bound(const key_type& key) const {
    return {upper_bound_node(key)};
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::treap_node*
treap<Node, Compare, Allocator>::node_of_key(const key_type& key) {
    return const_cast<treap_node*>(const_cast<const treap*>(this)->node_of_key(key));
}

template <typename Node, typename Compare, typename Allocator>
const typename treap<Node, Compare, Allocator>::treap_node*
treap<Node, Compare, Allocator>::node_of_key(const key_type& key) const {
    const treap_node* node = root();
    while (node != nullptr) {
        if (_comparator(key, node->get_key())) {
            node = node->get_left();
            continue;
        }
        if (_comparator(node->get_key(), key)) {
            node = node->get_right();
            continue;
        }
        return node;
    }
    return end_node();
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::treap_node*
treap<Node, Compare, Allocator>::lower_bound_node(const key_type& key) {
    return const_cast<treap_node*>(const_cast<const treap*>(this)->lower_bound_node(key));
}

template <typename Node, typename Compare, typename Allocator>
const typename treap<Node, Compare, Allocator>::treap_node*
treap<Node, Compare, Allocator>::lower_bound_node(const key_type& key) const {
    const treap_node* node = root();
    const treap_node* result = end_node();
    while (node != nullptr) {
        if (_comparator(node->get_key(), key)) {
            node = node->get_right();
            continue;
        }
        result = node;
        node = node->get_left();
    }
    return result;
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::treap_node*
treap<Node, Compare, Allocator>::upper_bound_node(const key_type& key) {
    return const_cast<treap_node*>(const_cast<const treap*>(this)->upper_bound_node(key));
}

template <typename Node, typename Compare, typename Allocator>
const typename treap<Node, Compare, Allocator>::treap_node*
treap<Node, Compare, Allocator>::upper_bound_node(const key_type& key) const {
    const treap_node* node = root();
    const treap_node* result = end_node();
    while (node != nullptr) {
        if (_comparator(key, node->get_key())) {
            result = node;
            node = node->get_left();
            continue;
        }
        node = node->get_right();
    }
    return result;
}

template <typename Node, typename Compare, typename Allocator>
const typename treap<Node, Compare, Allocator>::key_type&
treap<Node, Compare, Allocator>::key_of_order(size_type index) const {
    if (index >= size()) {
        throw std::out_of_range("Index is out of bounds");
    }
    return root()->node_of_order(index)->get_key();
}

template <typename Node, typename Compare, typename Allocator>
typename treap<Node, Compare, Allocator>::size_type
treap<Node, Compare, Allocator>::order_of_key(const key_type& key) const {
    return node_of_key(key)->order();
}

} // namespace nstd

#endif // BASICS_TREAP_HPP
