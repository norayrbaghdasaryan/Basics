#ifndef BASICS_TREAP_BASE_HPP
#define BASICS_TREAP_BASE_HPP

#include <cstddef>
#include <memory>
#include <reverse_iterator.hpp>
#include <chrono>
#include <random>

namespace nstd {

/**
 * Treap node base class
 * Implements basic part of the treap node (left, right child nodes, priority)
 * Serves as base class for all kind of treap nodes
 * Does not have key, value getter functions, as they are different for each data structure
 * @tparam Node Treap node class, which inherits from treap_node_base
 * Template parameter is for avoiding persistent down casts
 */
template <typename Node>
class treap_node_base {
    using treap_node = Node;

protected:
    using size_type = size_t;
    using priority_type = unsigned long long;

private:
    // node priority presented in integer type
    priority_type _priority;
    // left child
    treap_node* _left;
    // right child
    treap_node* _right;
    // size showing how many nodes are lying under tree with root of this node
    size_type _size;

public:
    explicit treap_node_base(priority_type priority = 0, treap_node* left = nullptr,
                             treap_node* right = nullptr)
            : _priority(priority), _left(left), _right(right), _size(1) { update(); }

public:
    void set_members(priority_type priority = 0, treap_node* left = nullptr, treap_node* right = nullptr) {
        _priority = priority;
        _left = left;
        _right = right;
        update();
    }

public:
    void set_left(treap_node* node) {
        _left = node;
        update();
    }

    void set_right(treap_node* node) {
        _right = node;
        update();
    }

    treap_node* get_left() { return _left; }

    const treap_node* get_left() const { return _left; }

    treap_node* get_right() { return _right; }

    const treap_node* get_right() const { return _right; }

    size_type size() const { return _size; }

    size_type left_size() const { return (_left != nullptr ? _left->_size : 0); }

    size_type right_size() const { return (_right != nullptr ? _right->_size : 0); }

    priority_type get_priority() const { return _priority; }

private:
    /**
     * Updates size member corresponding to left and right nodes
     */
    void update() { _size = left_size() + right_size() + 1; }
};

/**
* Treap Node destructor class
* Is used as unique pointer destructor class
* @tparam Allocator Treap node allocator class
*/
template <typename Allocator>
class treap_node_destructor {
    using allocator_type = Allocator;
    using allocator_traits = std::allocator_traits<Allocator>;
public:
    using pointer = typename allocator_traits::pointer;
private:
    allocator_type& _allocator;
public:
    bool value_constructed;
public:
    treap_node_destructor(const treap_node_destructor&) = default;

    treap_node_destructor& operator=(const treap_node_destructor&) = delete;

    explicit treap_node_destructor(allocator_type& allocator, bool value_constructed = false) noexcept
            : _allocator(allocator), value_constructed(value_constructed) {}

    /**
     * Destroys underlying value, if it's constructed and deallocates node memory
     * @param node treap node pointer
     */
    void operator()(pointer node) _NOEXCEPT {
        if (value_constructed) {
            allocator_traits::destroy(_allocator, node->get_value_address());
        }
        if (node) {
            allocator_traits::deallocate(_allocator, node, 1);
        }
    }
};

template <typename Node, typename Allocator>
class treap_base {
public:
    using key_type = typename Node::key_type;
    using value_type = typename Node::value_type;
    using allocator_type = Allocator;
    using size_type = size_t;

private:
    using treap_node = Node;
    using alloc_traits = std::allocator_traits<allocator_type>;
    using node_allocator_type = typename alloc_traits::template rebind_alloc<treap_node>;
    using node_traits = std::allocator_traits<node_allocator_type>;

    using node_destructor = treap_node_destructor<node_allocator_type>;
protected:
    using node_holder = std::unique_ptr<treap_node, node_destructor>;

private:
    /**
     * Iterator class for treap data structure
     * @tparam B determines is iterator class for const elements or not
     */
    template <bool B>
    class common_iterator {
        friend class common_iterator<!B>;

    public:
        using value_type = std::conditional_t<B, const treap_base::value_type, treap_base::value_type>;
        using treap_type = std::conditional_t<B, const treap_base, treap_base>;
    private:
        value_type* _value;
        treap_type* _treap;
        size_type _index;
    public:
        common_iterator(value_type* value, treap_type* treap, size_type index);

        /**
         * This constructor serves as copy constructor for iterator
         * And conversion operator from iterator to const_iterator either
         * @param other other object
         */
        common_iterator(const common_iterator<false>& other);

    public:
        common_iterator<B>& operator++();

        common_iterator<B> operator++(int)&;

        common_iterator<B>& operator+=(ptrdiff_t);

        common_iterator<B>& operator--();

        common_iterator<B> operator--(int)&;

        common_iterator<B>& operator-=(ptrdiff_t);

    public:
        ptrdiff_t operator-(const common_iterator<B>&) const;

    public:
        value_type& operator*() const;

        value_type* operator->() const;

    public:
        bool operator==(const common_iterator<B>&) const;

        bool operator!=(const common_iterator<B>&) const;

        bool operator<(const common_iterator<B>&) const;

        bool operator>(const common_iterator<B>&) const;

        bool operator<=(const common_iterator<B>&) const;

        bool operator>=(const common_iterator<B>&) const;

    public:
        common_iterator<B> operator+(ptrdiff_t) const;

        common_iterator<B> operator-(ptrdiff_t) const;
    };

public:
    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    using reverse_iterator = common_reverse_iterator<iterator>;
    using const_reverse_iterator = common_reverse_iterator<const_iterator>;

protected:
    treap_node* _root = nullptr;
    node_allocator_type _node_allocator;

public:
    explicit treap_base(const allocator_type& allocator = allocator_type());

    /** In base treap class we already don't know how to insert elements, so copy constructor is deleted */
    treap_base(const treap_base& other) = delete;

    treap_base(treap_base&& other) noexcept;

    /** In base treap class we already don't know how to insert elements, so copy assignment operator is deleted */
    treap_base& operator=(const treap_base& other) = delete;

    treap_base& operator=(treap_base&& other) noexcept;

    ~treap_base();

protected:
    /**
     * Destroys underlying tree node values and deallocates memory
     * @param node
     */
    void destroy_tree(treap_node* node);

    /**
     * Constructs treap node and it's value with passed constructor arguments
     * Uses perfect forwarding technique
     * @tparam Args constructor argument types
     * @param args constructor arguments
     * @return node holder, i.e. unique_pointer with special destructor class
     */
    template <typename... Args>
    node_holder construct_node(Args&& ... args);

    treap_node* node_of_order(size_type index);

    /**
     * Works with O(log size) complexity
     * @param index order of node
     * @return proper treap node index, when index < size, nullptr when index = size
     * Throws std::out_of_range exception, when index > size
     */
    const treap_node* node_of_order(size_type index) const;

public:
    void swap(treap_base& other) noexcept;

    bool empty() const { return size() == 0; }

    size_type size() const { return _root != nullptr ? _root->size() : 0; }

public:
    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    reverse_iterator rbegin();

    const_reverse_iterator rbegin() const;

    reverse_iterator rend();

    const_reverse_iterator rend() const;

    const_iterator cbegin() const;

    const_iterator cend() const;

    const_reverse_iterator crbegin() const;

    const_reverse_iterator crend() const;

protected:
    static std::mt19937_64 random_generator;
};

template <typename Node, typename Allocator>
std::mt19937_64 treap_base<Node, Allocator>::random_generator(
        std::chrono::steady_clock::now().time_since_epoch().count());

//======================common_iterator implementation==========================================


template <typename Node, typename Allocator>
template <bool B>
treap_base<Node, Allocator>::common_iterator<B>::common_iterator(value_type* value, treap_type* treap,
                                                                 size_type index)
        :_value(value), _treap(treap), _index(index) {}

template <typename Node, typename Allocator>
template <bool B>
treap_base<Node, Allocator>::common_iterator<B>::common_iterator(const common_iterator<false>& other)
        :_value(other._value), _treap(other._treap), _index(other._index) {}

template <typename Node, typename Allocator>
template <bool B>
typename treap_base<Node, Allocator>::template common_iterator<B>&
treap_base<Node, Allocator>::common_iterator<B>::operator++() {
    auto* node = _treap->node_of_order(++_index);
    _value = (node == nullptr ? nullptr : node->get_value_address());
    return *this;
}

template <typename Node, typename Allocator>
template <bool B>
typename treap_base<Node, Allocator>::template common_iterator<B>
treap_base<Node, Allocator>::common_iterator<B>::operator++(int)& {
    common_iterator iter = *this;
    ++(*this);
    return iter;
}

template <typename Node, typename Allocator>
template <bool B>
typename treap_base<Node, Allocator>::template common_iterator<B>&
treap_base<Node, Allocator>::common_iterator<B>::operator+=(ptrdiff_t n) {
    auto* node = _treap->node_of_order(_index += n);
    _value = (node == nullptr ? nullptr : node->get_value_address());
    return *this;
}

template <typename Node, typename Allocator>
template <bool B>
typename treap_base<Node, Allocator>::template common_iterator<B>&
treap_base<Node, Allocator>::common_iterator<B>::operator--() {
    auto* node = _treap->node_of_order(--_index);
    _value = (node == nullptr ? nullptr : node->get_value_address());
    return *this;
}

template <typename Node, typename Allocator>
template <bool B>
typename treap_base<Node, Allocator>::template common_iterator<B>
treap_base<Node, Allocator>::common_iterator<B>::operator--(int)& {
    common_iterator iter = *this;
    --(*this);
    return iter;
}

template <typename Node, typename Allocator>
template <bool B>
typename treap_base<Node, Allocator>::template common_iterator<B>&
treap_base<Node, Allocator>::common_iterator<B>::operator-=(ptrdiff_t n) {
    auto* node = _treap->node_of_order(_index -= n);
    _value = (node == nullptr ? nullptr : node->get_value_address());
    return *this;
}

template <typename Node, typename Allocator>
template <bool B>
ptrdiff_t treap_base<Node, Allocator>::common_iterator<B>::operator-(const common_iterator<B>& iter) const {
    return static_cast<ptrdiff_t>(_index) - iter._index;
}

template <typename Node, typename Allocator>
template <bool B>
auto treap_base<Node, Allocator>::common_iterator<B>::operator*() const -> value_type& {
    return *_value;
}

template <typename Node, typename Allocator>
template <bool B>
auto treap_base<Node, Allocator>::common_iterator<B>::operator->() const -> value_type* {
    return _value;
}

template <typename Node, typename Allocator>
template <bool B>
bool treap_base<Node, Allocator>::common_iterator<B>::operator==(const common_iterator<B>& iter) const {
    return _value == iter._value;
}

template <typename Node, typename Allocator>
template <bool B>
bool treap_base<Node, Allocator>::common_iterator<B>::operator!=(const common_iterator<B>& iter) const {
    return _value != iter._value;
}

template <typename Node, typename Allocator>
template <bool B>
bool treap_base<Node, Allocator>::common_iterator<B>::operator<(const common_iterator<B>& iter) const {
    return _index < iter._index;
}

template <typename Node, typename Allocator>
template <bool B>
bool treap_base<Node, Allocator>::common_iterator<B>::operator>(const common_iterator<B>& iter) const {
    return _index > iter._index;
}

template <typename Node, typename Allocator>
template <bool B>
bool treap_base<Node, Allocator>::common_iterator<B>::operator<=(const common_iterator<B>& iter) const {
    return _index <= iter._index;
}

template <typename Node, typename Allocator>
template <bool B>
bool treap_base<Node, Allocator>::common_iterator<B>::operator>=(const common_iterator<B>& iter) const {
    return _index >= iter._index;
}

template <typename Node, typename Allocator>
template <bool B>
typename treap_base<Node, Allocator>::template common_iterator<B>
treap_base<Node, Allocator>::common_iterator<B>::operator+(ptrdiff_t n) const {
    common_iterator<B> iter = *this;
    return iter += n;
}

template <typename Node, typename Allocator>
template <bool B>
typename treap_base<Node, Allocator>::template common_iterator<B>
treap_base<Node, Allocator>::common_iterator<B>::operator-(ptrdiff_t n) const {
    common_iterator<B> iter = *this;
    return iter -= n;
}

//==========================================Treap base implementation==========================================

template <typename Node, typename Allocator>
treap_base<Node, Allocator>::treap_base(const allocator_type& allocator)
        : _root(nullptr), _node_allocator(allocator) {}

template <typename Node, typename Allocator>
treap_base<Node, Allocator>::treap_base(treap_base<Node, Allocator>&& other) noexcept
        : _root(std::exchange(other._root, nullptr)),
          _node_allocator(std::move(other._node_allocator)) {}

template <typename Node, typename Allocator>
treap_base<Node, Allocator>&
treap_base<Node, Allocator>::operator=(treap_base<Node, Allocator>&& other) noexcept {
    if (this != &other) {
        treap_base<Node, Allocator> moved(std::move(other));
        this->swap(moved);
    }
    return *this;
}

template <typename Node, typename Allocator>
treap_base<Node, Allocator>::~treap_base() {
    destroy_tree(_root);
}

template <typename Node, typename Allocator>
void treap_base<Node, Allocator>::destroy_tree(treap_node* node) {
    if (node != nullptr) {
        // destroy child nodes
        destroy_tree(node->get_left());
        destroy_tree(node->get_right());
        // destroy node key and deallocate memory
        node_holder holder(node, treap_node_destructor(_node_allocator, true));
    }
}

template <typename Node, typename Allocator>
void treap_base<Node, Allocator>::swap(treap_base<Node, Allocator>& other) noexcept {
    std::swap(_root, other._root);
}

template <typename Node, typename Allocator>
template <typename... Args>
typename treap_base<Node, Allocator>::node_holder treap_base<Node, Allocator>::construct_node(Args&& ... args) {
    // allocate memory for new node
    node_holder holder(node_traits::allocate(_node_allocator, 1), treap_node_destructor(_node_allocator));
    // construct key using perfect forwarding technique
    node_traits::construct(_node_allocator, holder->get_value_address(), std::forward<Args>(args)...);
    // set value constructed flag true in order to destroy constructed value using deleter
    holder.get_deleter().value_constructed = true;
    // initialize non-initialized memory for avoiding segfaults
    holder->set_members(random_generator(), nullptr, nullptr);
    return holder;
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::treap_node* treap_base<Node, Allocator>::node_of_order(size_type index) {
    return const_cast<treap_node*>(const_cast<const treap_base<Node, Allocator>*>(this)->node_of_order(index));
}

template <typename Node, typename Allocator>
const typename treap_base<Node, Allocator>::treap_node*
treap_base<Node, Allocator>::node_of_order(size_type index) const {
    if (index == size()) {
        return nullptr;
    }
    if (index > size()) {
        throw std::out_of_range(std::to_string(index) + " index out of bounds");
    }
    ++index;
    treap_node* root = _root;
    while (root != nullptr) {
        size_type left_count = root->left_size();
        if (index == left_count + 1) {
            return root;
        }
        if (index <= left_count) {
            root = root->get_left();
            continue;
        }
        root = root->get_right();
        index -= left_count + 1;
    }
    throw std::runtime_error("Unreachable code");
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::iterator treap_base<Node, Allocator>::begin() {
    if (empty()) {
        return end();
    }
    const size_type index = 0;
    return {node_of_order(index)->get_value_address(), this, index};
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::const_iterator treap_base<Node, Allocator>::begin() const {
    return cbegin();
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::iterator treap_base<Node, Allocator>::end() {
    return {nullptr, this, size()};
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::const_iterator treap_base<Node, Allocator>::end() const {
    return cend();
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::reverse_iterator treap_base<Node, Allocator>::rbegin() {
    return {end()};
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::const_reverse_iterator treap_base<Node, Allocator>::rbegin() const {
    return {end()};
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::reverse_iterator treap_base<Node, Allocator>::rend() {
    return {begin()};
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::const_reverse_iterator treap_base<Node, Allocator>::rend() const {
    return {begin()};
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::const_iterator treap_base<Node, Allocator>::cbegin() const {
    if (empty()) {
        return cend();
    }
    const size_type index = 0;
    return {node_of_order(index)->get_value_address(), this, index};
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::const_iterator treap_base<Node, Allocator>::cend() const {
    return {nullptr, this, size()};
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::const_reverse_iterator treap_base<Node, Allocator>::crbegin() const {
    return {cend()};
}

template <typename Node, typename Allocator>
typename treap_base<Node, Allocator>::const_reverse_iterator treap_base<Node, Allocator>::crend() const {
    return {cbegin()};
}

} // namespace nstd

#endif //BASICS_TREAP_BASE_HPP