#ifndef SEQUENTIAL_LIST_H
#define SEQUENTIAL_LIST_H

#include <memory>
#include <iostream>
#include <type_traits>

template<typename T, typename Alloc = std::allocator<T>>
class Vector {
public:
    typedef T value_type;
    typedef std::allocator_traits<Alloc> AllocTraits;
private:
    size_t m_capacity;
    size_t m_size;
    Alloc m_allocator;
    value_type *m_array;
private:
    template<bool B>
    class common_iterator {
    public:
        typedef std::conditional_t<B, const value_type, value_type> type;
    private:
        type *m_ptr;
    public:
        common_iterator(type *);

    public:
        common_iterator<B> &operator++();

        common_iterator<B> operator++(int);

        common_iterator<B> &operator+=(ptrdiff_t);

        common_iterator<B> &operator--();

        common_iterator<B> operator--(int);

        common_iterator<B> &operator-=(ptrdiff_t);

    public:
        ptrdiff_t operator-(const common_iterator<B> &) const;

    public:
        type &operator*();

        type *operator->();

    public:
        bool operator==(const common_iterator<B> &) const;

        bool operator!=(const common_iterator<B> &) const;

        bool operator<(const common_iterator<B> &) const;

        bool operator>(const common_iterator<B> &) const;

        bool operator<=(const common_iterator<B> &) const;

        bool operator>=(const common_iterator<B> &) const;

    public:
        common_iterator<B> operator+(ptrdiff_t);

        common_iterator<B> operator-(ptrdiff_t);
    };

private:
    template<typename It>
    class common_reverse_iterator {
    public:
        typedef typename It::type type;
    private:
        It m_iterator;
    public:
        common_reverse_iterator(const It &);

    public:
        common_reverse_iterator<It> &operator++();

        common_reverse_iterator<It> operator++(int);

        common_reverse_iterator<It> &operator+=(ptrdiff_t);

        common_reverse_iterator<It> &operator--();

        common_reverse_iterator<It> operator--(int);

        common_reverse_iterator<It> &operator-=(ptrdiff_t);

    public:
        ptrdiff_t operator-(const common_reverse_iterator<It> &) const;

    public:
        type &operator*();

        type *operator->();

    public:
        bool operator==(const common_reverse_iterator<It> &) const;

        bool operator!=(const common_reverse_iterator<It> &) const;

        bool operator<(const common_reverse_iterator<It> &) const;

        bool operator>(const common_reverse_iterator<It> &) const;

        bool operator<=(const common_reverse_iterator<It> &) const;

        bool operator>=(const common_reverse_iterator<It> &) const;

    public:
        common_reverse_iterator<It> operator+(ptrdiff_t);

        common_reverse_iterator<It> operator-(ptrdiff_t);
    };

public:
    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    using reverse_iterator = common_reverse_iterator<iterator>;
    using const_reverse_iterator = common_reverse_iterator<const_iterator>;
public:
    Vector(const Alloc & = Alloc());

    Vector(const Vector<T, Alloc> &);

    Vector(Vector<T, Alloc> &&) noexcept;

    Vector &operator=(const Vector<T, Alloc> &) &;

    Vector &operator=(Vector<T, Alloc> &&) & noexcept;

    ~Vector();

public:
    template<typename... Args>
    void emplace_back(Args &&...);

    void push_back(const value_type &);

    void push_back(value_type &&);

    void pop_back();

public:
    void reserve(size_t);

    value_type &back() { return m_array[m_size - 1]; }

    const value_type &back() const { return m_array[m_size - 1]; }

public:
    iterator begin();

    iterator end();

    const_iterator cbegin() const;

    const_iterator cend() const;

    const_iterator begin() const;

    const_iterator end() const;

public:
    reverse_iterator rbegin();

    reverse_iterator rend();

    const_reverse_iterator crbegin() const;

    const_reverse_iterator crend() const;

    const_reverse_iterator rbegin() const;

    const_reverse_iterator rend() const;

public:
    value_type &operator[](size_t);

    const value_type &operator[](size_t) const;

public:
    size_t size() const { return m_size; }

    bool empty() const { return m_size == 0; }

private:
    void swap(Vector &);

    bool is_full() const { return m_size == m_capacity; }

    void destroy();

private:
    static const size_t MAX_SIZE = 2;
};

template<typename T, typename Alloc>
std::ostream &operator<<(std::ostream &, const Vector<T, Alloc> &);

//======================common_iterator implementation==========================================

template<typename T, typename Alloc>
template<bool B>
Vector<T, Alloc>::common_iterator<B>::common_iterator(type *ptr)
        :m_ptr(ptr) {}

template<typename T, typename Alloc>
template<bool B>
typename Vector<T, Alloc>::template common_iterator<B> &Vector<T, Alloc>::common_iterator<B>::operator++() {
    ++m_ptr;
    return *this;
}

template<typename T, typename Alloc>
template<bool B>
typename Vector<T, Alloc>::template common_iterator<B> Vector<T, Alloc>::common_iterator<B>::operator++(int) {
    common_iterator iter = *this;
    ++m_ptr;
    return iter;
}

template<typename T, typename Alloc>
template<bool B>
typename Vector<T, Alloc>::template common_iterator<B> &Vector<T, Alloc>::common_iterator<B>::operator+=(ptrdiff_t n) {
    m_ptr += n;
    return *this;
}

template<typename T, typename Alloc>
template<bool B>
typename Vector<T, Alloc>::template common_iterator<B> &Vector<T, Alloc>::common_iterator<B>::operator--() {
    --m_ptr;
    return *this;
}

template<typename T, typename Alloc>
template<bool B>
typename Vector<T, Alloc>::template common_iterator<B> Vector<T, Alloc>::common_iterator<B>::operator--(int) {
    common_iterator iter = *this;
    --m_ptr;
    return iter;
}

template<typename T, typename Alloc>
template<bool B>
typename Vector<T, Alloc>::template common_iterator<B> &Vector<T, Alloc>::common_iterator<B>::operator-=(ptrdiff_t n) {
    m_ptr -= n;
    return *this;
}

template<typename T, typename Alloc>
template<bool B>
ptrdiff_t Vector<T, Alloc>::common_iterator<B>::operator-(const common_iterator<B> &iter) const {
    return m_ptr - iter.m_ptr;
}

template<typename T, typename Alloc>
template<bool B>
auto Vector<T, Alloc>::common_iterator<B>::operator*() -> type & {
    return *m_ptr;
}

template<typename T, typename Alloc>
template<bool B>
auto Vector<T, Alloc>::common_iterator<B>::operator->() -> type * {
    return m_ptr;
}

template<typename T, typename Alloc>
template<bool B>
bool Vector<T, Alloc>::common_iterator<B>::operator==(const common_iterator<B> &iter) const {
    return m_ptr == iter.m_ptr;
}

template<typename T, typename Alloc>
template<bool B>
bool Vector<T, Alloc>::common_iterator<B>::operator!=(const common_iterator<B> &iter) const {
    return m_ptr != iter.m_ptr;
}

template<typename T, typename Alloc>
template<bool B>
bool Vector<T, Alloc>::common_iterator<B>::operator<(const common_iterator<B> &iter) const {
    return m_ptr < iter.m_ptr;
}

template<typename T, typename Alloc>
template<bool B>
bool Vector<T, Alloc>::common_iterator<B>::operator>(const common_iterator<B> &iter) const {
    return m_ptr > iter.m_ptr;
}

template<typename T, typename Alloc>
template<bool B>
bool Vector<T, Alloc>::common_iterator<B>::operator<=(const common_iterator<B> &iter) const {
    return m_ptr <= iter.m_ptr;
}

template<typename T, typename Alloc>
template<bool B>
bool Vector<T, Alloc>::common_iterator<B>::operator>=(const common_iterator<B> &iter) const {
    return m_ptr >= iter.m_ptr;
}

template<typename T, typename Alloc>
template<bool B>
typename Vector<T, Alloc>::template common_iterator<B> Vector<T, Alloc>::common_iterator<B>::operator+(ptrdiff_t n) {
    common_iterator<B> iter = *this;
    return iter += n;
}

template<typename T, typename Alloc>
template<bool B>
typename Vector<T, Alloc>::template common_iterator<B> Vector<T, Alloc>::common_iterator<B>::operator-(ptrdiff_t n) {
    common_iterator<B> iter = *this;
    return iter -= n;
}

//======================common_reverse_iterator implementation==========================================


template<typename T, typename Alloc>
template<typename It>
Vector<T, Alloc>::common_reverse_iterator<It>::common_reverse_iterator(const It &iter)
        :m_iterator(iter) {}

template<typename T, typename Alloc>
template<typename It>
typename Vector<T, Alloc>::template common_reverse_iterator<It> &
Vector<T, Alloc>::common_reverse_iterator<It>::operator++() {
    --m_iterator;
    return *this;
}

template<typename T, typename Alloc>
template<typename It>
typename Vector<T, Alloc>::template common_reverse_iterator<It>
Vector<T, Alloc>::common_reverse_iterator<It>::operator++(int) {
    common_reverse_iterator<It> iter = *this;
    --m_iterator;
    return iter;
}

template<typename T, typename Alloc>
template<typename It>
typename Vector<T, Alloc>::template common_reverse_iterator<It> &
Vector<T, Alloc>::common_reverse_iterator<It>::operator+=(ptrdiff_t n) {
    m_iterator -= n;
    return *this;
}

template<typename T, typename Alloc>
template<typename It>
typename Vector<T, Alloc>::template common_reverse_iterator<It> &
Vector<T, Alloc>::common_reverse_iterator<It>::operator--() {
    ++m_iterator;
    return *this;
}

template<typename T, typename Alloc>
template<typename It>
typename Vector<T, Alloc>::template common_reverse_iterator<It>
Vector<T, Alloc>::common_reverse_iterator<It>::operator--(int) {
    common_reverse_iterator iter = *this;
    ++m_iterator;
    return iter;
}

template<typename T, typename Alloc>
template<typename It>
typename Vector<T, Alloc>::template common_reverse_iterator<It> &
Vector<T, Alloc>::common_reverse_iterator<It>::operator-=(ptrdiff_t n) {
    m_iterator += n;
    return *this;
}

template<typename T, typename Alloc>
template<typename It>
ptrdiff_t Vector<T, Alloc>::common_reverse_iterator<It>::operator-(const common_reverse_iterator<It> &iter) const {
    return iter.m_iterator - m_iterator;
}

template<typename T, typename Alloc>
template<typename It>
auto Vector<T, Alloc>::common_reverse_iterator<It>::operator*() -> type & {
    return *m_iterator;
}

template<typename T, typename Alloc>
template<typename It>
auto Vector<T, Alloc>::common_reverse_iterator<It>::operator->() -> type * {
    return std::addressof(*m_iterator);
}

template<typename T, typename Alloc>
template<typename It>
bool Vector<T, Alloc>::common_reverse_iterator<It>::operator==(const common_reverse_iterator<It> &iter) const {
    return m_iterator == iter.m_iterator;
}

template<typename T, typename Alloc>
template<typename It>
bool Vector<T, Alloc>::common_reverse_iterator<It>::operator!=(const common_reverse_iterator<It> &iter) const {
    return m_iterator != iter.m_iterator;
}

template<typename T, typename Alloc>
template<typename It>
bool Vector<T, Alloc>::common_reverse_iterator<It>::operator<(const common_reverse_iterator<It> &iter) const {
    return iter.m_iterator < m_iterator;
}

template<typename T, typename Alloc>
template<typename It>
bool Vector<T, Alloc>::common_reverse_iterator<It>::operator>(const common_reverse_iterator<It> &iter) const {
    return iter.m_iterator > m_iterator;
}

template<typename T, typename Alloc>
template<typename It>
bool Vector<T, Alloc>::common_reverse_iterator<It>::operator<=(const common_reverse_iterator<It> &iter) const {
    return iter.m_iterator <= m_iterator;
}

template<typename T, typename Alloc>
template<typename It>
bool Vector<T, Alloc>::common_reverse_iterator<It>::operator>=(const common_reverse_iterator<It> &iter) const {
    return iter.m_iterator >= m_iterator;
}

template<typename T, typename Alloc>
template<typename It>
typename Vector<T, Alloc>::template common_reverse_iterator<It>
Vector<T, Alloc>::common_reverse_iterator<It>::operator+(ptrdiff_t n) {
    typename Vector<T, Alloc>::template common_reverse_iterator<It> iter = *this;
    return iter += n;
}

template<typename T, typename Alloc>
template<typename It>
typename Vector<T, Alloc>::template common_reverse_iterator<It>
Vector<T, Alloc>::common_reverse_iterator<It>::operator-(ptrdiff_t n) {
    typename Vector<T, Alloc>::template common_reverse_iterator<It> iter = *this;
    return iter -= n;
}

//=================================== Sequential List implementation ===================================

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Alloc &allocator)
        : m_capacity(MAX_SIZE), m_size(0), m_allocator(allocator),
          m_array(AllocTraits::allocate(m_allocator, m_capacity)) {}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc> &obj)
        : m_capacity(obj.m_capacity), m_size(obj.m_size), m_allocator(obj.m_allocator),
          m_array(AllocTraits::allocate(m_allocator, m_capacity)) {
    // variable is initialized here for catch block
    size_t i = 0;
    try {
        for (i = 0; i < m_size; ++i) {
            // try to contruct
            AllocTraits::construct(m_allocator, m_array + i, obj.m_array[i]);
        }
    }
    catch (...) {
        // destroy all already constructed objects
        for (; i > 0; --i) {
            AllocTraits::destroy(m_allocator, m_array + i - 1);
        }
        // deallocate memory
        AllocTraits::deallocate(m_allocator, m_array, m_capacity);
        // rethrow caught exception
        throw;
    }
}


template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector<T, Alloc> &&obj) noexcept
        :m_capacity(obj.m_capacity), m_size(obj.m_size), m_allocator(obj.m_allocator), m_array(obj.m_array) {
    obj.m_capacity = 0;
    obj.m_size = 0;
    obj.m_array = nullptr;
}

template<typename T, typename Alloc>
Vector<T, Alloc> &Vector<T, Alloc>::operator=(const Vector<T, Alloc> &obj) &{
    if (this != &obj) {
        Vector<T, Alloc> copy_obj(obj);
        this->swap(copy_obj);
    }
    return *this;
}

template<typename T, typename Alloc>
Vector<T, Alloc> &Vector<T, Alloc>::operator=(Vector<T, Alloc> &&obj) & noexcept {
    if (this != &obj) {
        Vector<T, Alloc> moved_obj = std::move(obj);
        this->swap(moved_obj);
    }
    return *this;
}

template<typename T, typename Alloc>
Vector<T, Alloc>::~Vector() {
    destroy();
}

template<typename T, typename Alloc>
template<typename... Args>
void Vector<T, Alloc>::emplace_back(Args &&... args) {
    // reserve more capacity if current is fullfilled
    if (is_full()) {
        reserve(2 * m_capacity);
    }
    AllocTraits::construct(m_allocator, m_array + m_size, std::forward<Args>(args)...);
    ++m_size;
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::push_back(const value_type &elem) {
    emplace_back(elem);
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::push_back(value_type &&elem) {
    emplace_back(std::move(elem));
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::pop_back() {
    if (!empty()) {
        --m_size;
        AllocTraits::destroy(m_allocator, m_array + m_size);
    }
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::reserve(size_t n) {
    // if there already present enought memory, just return
    if (n <= m_capacity) return;

    // allocate and construct new memory
    value_type *new_array = AllocTraits::allocate(m_allocator, n);
    // variable initialized here for catch block
    size_t i = 0;
    try {
        for (i = 0; i < m_size; ++i) {
            AllocTraits::construct(m_allocator, new_array + i, std::move_if_noexcept(m_array[i]));
        }
    }
    catch (...) {
        // destroy all already constructed objects
        for (; i > 0; --i) {
            // if move assignment is safe, then move in old memory
            if constexpr (std::is_nothrow_move_constructible_v<value_type>
                          && std::is_nothrow_move_assignable_v<value_type>) {
                m_array[i] = new_array[i];
                continue;
            } else {
                // else destroy the element
                AllocTraits::destroy(m_allocator, new_array + i - 1);
            }
        }
        // deallocate memory
        AllocTraits::deallocate(m_allocator, new_array, n);
        // rethrow caught exception
        throw;
    }

    destroy();

    m_capacity = n;
    m_array = new_array;
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::begin() {
    return iterator(m_array);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::end() {
    return iterator(m_array + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::cbegin() const {
    return const_iterator(m_array);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::cend() const {
    return const_iterator(m_array + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::begin() const {
    return const_iterator(m_array);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::end() const {
    return const_iterator(m_array + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reverse_iterator Vector<T, Alloc>::rbegin() {
    return reverse_iterator(m_array + m_size - 1);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reverse_iterator Vector<T, Alloc>::rend() {
    return reverse_iterator(m_array - 1);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::crbegin() const {
    return const_reverse_iterator(m_array + m_size - 1);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::crend() const {
    return const_reverse_iterator(m_array - 1);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::rbegin() const {
    return const_reverse_iterator(m_array + m_size - 1);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::rend() const {
    return const_reverse_iterator(m_array - 1);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::value_type &Vector<T, Alloc>::operator[](size_t ind) {
    return m_array[ind];
}

template<typename T, typename Alloc>
const typename Vector<T, Alloc>::value_type &Vector<T, Alloc>::operator[](size_t ind) const {
    return m_array[ind];
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::swap(Vector<T, Alloc> &obj) {
    std::swap(m_capacity, obj.m_capacity);
    std::swap(m_size, obj.m_size);
    std::swap(m_array, obj.m_array);
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::destroy() {
    // destroy elements and deallocate memory
    for (size_t i = m_size; i > 0; --i) {
        AllocTraits::destroy(m_allocator, m_array + i - 1);
    }
    AllocTraits::deallocate(m_allocator, m_array, m_capacity);
}

template<typename T, typename Alloc>
std::ostream &operator<<(std::ostream &out, const Vector<T, Alloc> &obj) {
    for (auto it = obj.cbegin(); it != obj.cend(); ++it) {
        std::cout << *it << ' ';
    }
    out << std::endl;
    return out;
}

#endif // SEQUENTIAL_LIST_H