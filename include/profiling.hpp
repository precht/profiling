/**
 *  MIT License
 *
 *  Copyright (c) 2020 Jakub Precht
 *
 *  Source https://github.com/precht/profiling
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#pragma once

#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <sstream>
#include <cmath>
#include <type_traits>
#include <execution>
#include <utility>
#include <queue>
#include <ostream>
#include <cassert>

namespace profiling {

/////////////////////////////////////////////////////////////////////////////// DECLARATION RED_BLACK_TREE /////////////

namespace jp::ds {

/**
 * This class implements __gnu_pbds::tree API
 *
 * PBDS docs:
 * https://gcc.gnu.org/onlinedocs/libstdc++/ext/pb_ds/tree_based_containers.html
 *
 * Red black tree implementation based on 'Introduction to Algorithms, Third Edition' by T. H. Cormen
 */
template<
        typename Key,
        typename CmpFn = std::less<Key>
        >
class RedBlackTree
{
    struct Node
    {
        size_t size;
        Node* left;
        Node* right;
        Node* parent;
        Key key;
        bool color;

        Node() = delete;
        explicit Node(const Key& key, size_t size, Node* left, Node* right, Node* parent, bool color);
        void print(std::ostream& out) const;
    };

public:
    class ConstIterator : public std::iterator<std::bidirectional_iterator_tag, Node>
    {
    public:
        ConstIterator(const RedBlackTree* tree, Node* node);
        ConstIterator(const ConstIterator& other);
        ConstIterator& operator++();
        ConstIterator operator++(int);
        ConstIterator& operator--();
        ConstIterator operator--(int);
        bool operator==(const ConstIterator& other) const;
        bool operator!=(const ConstIterator& other) const;
        const Key* operator->();
        const Key& operator*();
    private:
        const RedBlackTree* m_tree;
        Node* m_node;
    };

    RedBlackTree();
    RedBlackTree(const RedBlackTree& other);
    ~RedBlackTree();
    std::pair<ConstIterator, bool> insert(const Key& key);
    ConstIterator erase(const Key& key);
    size_t orderOfKey(const Key& key) const;
    ConstIterator find(const Key& key) const;
    ConstIterator findByOrder(size_t order) const;
    ConstIterator min() const;
    ConstIterator max() const;
    ConstIterator begin() const;
    ConstIterator end() const;
    size_t size() const;
    bool empty() const;
    void clear();

    template<typename T, typename C>
    friend std::ostream& operator<<(std::ostream& out, const RedBlackTree<T, C>& tree);

private:
    bool equal(const Key& lhs, const Key& rhs) const;
    bool notEqual(const Key& lhs, const Key& rhs) const;
    Node* successor(Node* x) const;
    Node* predecessor(Node* x) const;
    Node* min(Node* x) const;
    Node* max(Node* x) const;
    Node* search(const Key& key) const;
    void eraseTree(Node* root);
    void rotateLeft(Node* x);
    void rotateRight(Node* x);
    void transplant(Node* u, Node* v);
    void updateSize(Node* start, Node* end, size_t value);
    ConstIterator erase(Node* z);
    void fixUpErase(Node* x);
    void fixUpInsert(Node* z);
    void print(std::ostream& out, Node* x, size_t level) const;

    static constexpr bool RED = 0;
    static constexpr bool BLACK = 1;
    static constexpr CmpFn CMP = CmpFn();
    Node* m_nil;
    Node* m_root;
};

} //!jp::ds

/////////////////////////////////////////////////////////////////////////////// DECLARATION STOPWATCH //////////////////

class Stopwatch
{
public:
    void start();

    void lap();

    template<typename Duration>
    int64_t last() const;

    template<typename Duration>
    int64_t avg() const;

    template<typename Duration>
    int64_t median();

    template<typename Duration>
    int64_t stdev();

    template<typename Duration>
    std::string string();

    void clear();

private:
    using Clock = std::chrono::steady_clock;
    using Pair = std::pair<Clock::duration, double>;

    Clock::time_point m_timepoint{};
    Clock::duration m_sum{};
    std::vector<Pair> m_array{};
    jp::ds::RedBlackTree<Clock::duration> m_tree{};
};

/////////////////////////////////////////////////////////////////////////////// DEFINITION STOPWATCH ///////////////////

inline void Stopwatch::start()
{
    m_timepoint = Clock::now();
}

inline void Stopwatch::clear()
{
    m_timepoint = {};
    m_sum = {};
    m_array.clear();
    m_tree.clear();
}

inline void Stopwatch::lap()
{
    auto tp = Clock::now();
    m_array.emplace_back(std::pair<Clock::duration, double>(tp - m_timepoint, 0.0));
    m_timepoint = tp;
    m_tree.insert(m_array.back().first);
    m_sum += m_array.back().first;
}

template<typename Duration> inline
int64_t Stopwatch::last() const
{
    if (m_tree.empty())
        return 0;
    return std::chrono::duration_cast<Duration>(m_array.back().first).count();
}

template<typename Duration> inline
int64_t Stopwatch::avg() const
{
    if (m_array.empty())
        return 0;
    return std::chrono::duration_cast<Duration>(m_sum).count() / m_array.size();
}

template<typename Duration> inline
int64_t Stopwatch::median()
{
    if (m_tree.empty())
        return 0;
    uint64_t sz = m_tree.size();
    uint64_t idx1 = sz >> 1;
    uint64_t idx2 = idx1 - (!(sz & 1) && (sz & ~uint64_t{1}));
    return std::chrono::duration_cast<Duration>(*m_tree.findByOrder(idx1) + *m_tree.findByOrder(idx2)).count() >> 1;
}

template<typename Duration> inline
int64_t Stopwatch::stdev()
{
    if (m_array.empty())
        return 0;
    int64_t avgRep = avg<Clock::duration>();
    std::for_each(std::execution::par_unseq, m_array.begin(), m_array.end(), [&](auto& p) {
            double mp = p.first.count() - avgRep;
            p.second = mp * mp;
        });
    double stdev = std::reduce(std::execution::par_unseq,
                               m_array.begin(), m_array.end(), Pair{}, [](const auto& lhs, const auto& rhs) {
            auto tmp = lhs;
            tmp.second += rhs.second;
            return tmp;
        }).second;
    stdev = std::sqrt(stdev / m_array.size());
    return std::chrono::duration_cast<Duration>(Clock::duration(static_cast<uint64_t>(stdev))).count();
}

template<typename Duration> inline
std::string Stopwatch::string()
{
    const auto unit = [&]() {
        if constexpr (std::is_same<Duration, std::chrono::hours>::value)
            return "h";
        else if constexpr (std::is_same<Duration, std::chrono::minutes>::value)
            return "min";
        else if constexpr (std::is_same<Duration, std::chrono::seconds>::value)
            return "s";
        else if constexpr (std::is_same<Duration, std::chrono::milliseconds>::value)
            return "ms";
        else if constexpr (std::is_same<Duration, std::chrono::microseconds>::value)
            return "us";
        else if constexpr (std::is_same<Duration, std::chrono::nanoseconds>::value)
            return "ns";
        else
            return "";
    };
    std::stringstream ss{};
    ss << "laps: " << m_array.size() << ", "
       << "last: " << last<Duration>() << unit() << ", "
       << "avg: " << avg<Duration>() << unit() << ", "
       << "median: " << median<Duration>() << unit() << ", "
       << "stdev: " << stdev<Duration>() << unit()
          ;
    return ss.str();
}

/////////////////////////////////////////////////////////////////////////////// DEFINITION RED_BLACK_TREE //////////////

namespace jp::ds {

template<typename Key, typename CmpFn> inline
RedBlackTree<Key, CmpFn>::Node::Node(const Key& key, size_t size, Node* left, Node* right, Node* parent, bool color)
    : size(size)
    , left(left)
    , right(right)
    , parent(parent)
    , key(key)
    , color(color)
{ }

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::Node::print(std::ostream& out) const
{
    out << "(k:" << key << ",s:" << size << ",c:" << color << ')';
}

template<typename Key, typename CmpFn> inline
RedBlackTree<Key, CmpFn>::ConstIterator::ConstIterator(const RedBlackTree* tree, Node* node)
    : m_tree(tree)
    , m_node(node)
{ }

template<typename Key, typename CmpFn> inline
RedBlackTree<Key, CmpFn>::ConstIterator::ConstIterator(const RedBlackTree::ConstIterator& other)
    : m_tree(other.m_tree)
    , m_node(other.m_node)
{ }

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator& RedBlackTree<Key, CmpFn>::ConstIterator::operator++()
{
    m_node = m_tree->successor(m_node);
    return *this;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::ConstIterator::operator++(int)
{
    ConstIterator tmp{*this};
    operator++();
    return tmp;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator& RedBlackTree<Key, CmpFn>::ConstIterator::operator--()
{
    m_node = m_tree->predecessor(m_node);
    return *this;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::ConstIterator::operator--(int)
{
    ConstIterator tmp{*this};
    operator--();
    return tmp;
}

template<typename Key, typename CmpFn> inline
bool RedBlackTree<Key, CmpFn>::ConstIterator::operator==(const RedBlackTree::ConstIterator& other) const
{
    return m_node == other.m_node;
}

template<typename Key, typename CmpFn> inline
bool RedBlackTree<Key, CmpFn>::ConstIterator::operator!=(const RedBlackTree::ConstIterator& other) const
{
    return m_node != other.m_node;
}

template<typename Key, typename CmpFn> inline
const Key* RedBlackTree<Key, CmpFn>::ConstIterator::operator->()
{
    return &(m_node->key);
}

template<typename Key, typename CmpFn> inline
const Key& RedBlackTree<Key, CmpFn>::ConstIterator::operator*()
{
    return m_node->key;
}

template<typename Key, typename CmpFn> inline
RedBlackTree<Key, CmpFn>::RedBlackTree()
    : m_nil(new Node{Key{}, 0, nullptr, nullptr, nullptr, BLACK})
    , m_root(m_nil)
{
    m_nil->left = m_nil;
    m_nil->right = m_nil;
    m_nil->parent = m_nil;
}

template<typename Key, typename CmpFn> inline
RedBlackTree<Key, CmpFn>::RedBlackTree(const RedBlackTree& other)
    : RedBlackTree()
{
    if (other.m_root == other.m_nil)
        return;
    std::queue<Node*> buffor{};
    buffor.push(other.m_root);
    while (!buffor.empty()) {
        Node* x = buffor.front();
        buffor.pop();
        insert(x->key);
        if (x->left != other.m_nil)
            buffor.push(x->left);
        if (x->right != other.m_nil)
            buffor.push(x->right);
    }
}

template<typename Key, typename CmpFn> inline
RedBlackTree<Key, CmpFn>::~RedBlackTree()
{
    eraseTree(m_root);
    delete m_nil;
}

template<typename Key, typename CmpFn> inline
std::pair<typename RedBlackTree<Key, CmpFn>::ConstIterator, bool> RedBlackTree<Key, CmpFn>::insert(const Key& key)
{
    Node* x = m_root;
    Node* y = m_nil;
    while (x != m_nil) {
        y = x;
        if (equal(key, x->key))
            return std::make_pair(ConstIterator{this, x}, false);
        if (CMP(key, x->key))
            x = x->left;
        else
            x = x->right;
    }
    Node* z = new Node(key, 1, m_nil, m_nil, y, RED);
    if (y == m_nil)
        m_root = z;
    else if (CMP(z->key, y->key))
        y->left = z;
    else
        y->right = z;
    updateSize(z->parent, m_nil, 1);
    fixUpInsert(z);
    return std::make_pair(ConstIterator{this, z}, true);
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::erase(const Key& key)
{
    Node* z = search(key);
    if (z == m_nil)
        return ConstIterator{this, m_nil};
    return erase(z);
}

template<typename Key, typename CmpFn> inline
size_t RedBlackTree<Key, CmpFn>::orderOfKey(const Key& key) const {
    size_t current = m_root->size;
    Node* x = m_root;
    while (x != m_nil && notEqual(key, x->key)) {
        if (CMP(key, x->key)) {
            current--;
            if (x->right != m_nil)
                current -= x->right->size;
            x = x->left;
        } else {
            x = x->right;
        }
    }
    if (x->right != m_nil)
        current -= x->right->size;
    if (x != m_nil)
        --current;
    return current;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::find(const Key& key) const
{
    return ConstIterator{this, search(key)};
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::findByOrder(size_t order) const
{
    size_t current = m_root->left->size;
    Node* x = m_root;
    while (x != m_nil && current != order) {
        if (current > order) {
            current -= x->left->size;
            x = x->left;
            current += x->left->size;
        } else {
            x = x->right;
            current += 1 + x->left->size;
        }
    }
    return ConstIterator{this, x};
}

template<typename Key, typename CmpFn> inline
size_t RedBlackTree<Key, CmpFn>::size() const
{
    return m_root->size;
}

template<typename Key, typename CmpFn> inline
bool RedBlackTree<Key, CmpFn>::empty() const
{
    return m_root->size == 0;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::min() const
{
    return ConstIterator{this, min(m_root)};
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::max() const
{
    return ConstIterator{this, max(m_root)};
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::begin() const
{
    return findByOrder(0);
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::end() const
{
    return ConstIterator{this, m_nil};
}

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::clear()
{
    eraseTree(m_root);
    m_root = m_nil;
}

template<typename Key, typename CmpFn> inline
std::ostream& operator<<(std::ostream& out, const RedBlackTree<Key, CmpFn>& tree)
{
    if (tree.m_root != tree.m_nil)
        tree.print(out, tree.m_root, 0);
    return out;
}

template<typename Key, typename CmpFn> inline
bool RedBlackTree<Key, CmpFn>::equal(const Key& lhs, const Key& rhs) const
{
    return (!CMP(lhs, rhs)) & (!CMP(rhs, lhs));
}

template<typename Key, typename CmpFn> inline
bool RedBlackTree<Key, CmpFn>::notEqual(const Key& lhs, const Key& rhs) const
{
    return CMP(lhs, rhs) | CMP(rhs, lhs);
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::Node* RedBlackTree<Key, CmpFn>::successor(Node* x) const
{
    if (x->right != m_nil)
        return min(x->right);
    while (x != m_nil) {
        if (x == x->parent->left)
            return x->parent;
        x = x->parent;
    }
    return m_nil;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::Node* RedBlackTree<Key, CmpFn>::predecessor(Node* x) const
{
    if (x->left != m_nil)
        return max(x->left);
    while (x != m_nil) {
        if (x == x->parent->right)
            return x->parent;
        x = x->parent;
    }
    return m_nil;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::Node* RedBlackTree<Key, CmpFn>::min(Node* x) const
{
    if (x != m_nil)
        while (x->left != m_nil)
            x = x->left;
    return x;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::Node* RedBlackTree<Key, CmpFn>::max(Node* x) const
{
    if (x != m_nil)
        while (x->right != m_nil)
            x = x->right;
    return x;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::Node* RedBlackTree<Key, CmpFn>::search(const Key& key) const
{
    Node* x = m_root;
    while (x != m_nil && notEqual(key, x->key)) {
        if (CMP(key, x->key))
            x = x->left;
        else
            x = x->right;
    }
    return x;
}

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::eraseTree(Node* root)
{
    if (root == m_nil)
        return;
    std::queue<Node*> buffor;
    buffor.push(root);
    while (!buffor.empty()) {
        Node* x = buffor.front();
        buffor.pop();
        if (x->left != m_nil)
            buffor.push(x->left);
        if (x->right != m_nil)
            buffor.push(x->right);
        delete x;
    }
}

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::rotateLeft(Node* x)
{
    Node* y = x->right;
    x->right = y->left;
    if (y->left != m_nil)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == m_nil)
        m_root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
    if(x->left != m_nil)
        y->size += x->left->size;
    y->size++;
    if (y->right != m_nil)
        x->size -= y->right->size;
    x->size--;
}

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::rotateRight(Node* x)
{
    Node* y = x->left;
    x->left = y->right;
    if (y->right != m_nil)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == m_nil)
        m_root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->right = x;
    x->parent = y;
    if(x->right != m_nil)
        y->size += x->right->size;
    y->size++;
    if (y->left!= m_nil)
        x->size -= y->left->size;
    x->size--;
}

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::transplant(Node* u, Node* v)
{
    if (u->parent == m_nil)
        m_root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::updateSize(Node* start, Node* end, size_t value)
{
    while (start != end) {
        start->size += value;
        start = start->parent;
    }
}

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::fixUpInsert(Node* z)
{
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            Node* y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateRight(z->parent->parent);
            }
        } else {
            Node* y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateLeft(z->parent->parent);
            }
        }
    }
    m_root->color = BLACK;
}

template<typename Key, typename CmpFn> inline
typename RedBlackTree<Key, CmpFn>::ConstIterator RedBlackTree<Key, CmpFn>::erase(Node* z)
{
    updateSize(z->parent, m_nil, -1);
    auto it = ConstIterator{this, successor(z)};
    Node* y = z;
    Node* x = nullptr;
    bool y_original_color = y->color;
    if (z->left == m_nil) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == m_nil) {
        x = z->left;
        transplant(z, z->left);
    } else {
        y = min(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            updateSize(y->parent, z, -1);
            y->size -= y->right->size;
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
            y->size += y->right->size;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
        y->size += y->left->size;
    }
    if (y_original_color == BLACK)
        fixUpErase(x);
    m_nil->parent = m_nil;
    assert(m_nil->color == BLACK);
    delete z;
    return it;
}

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::fixUpErase(Node* x)
{
    while (x != m_root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateLeft(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rotateRight(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rotateLeft(x->parent);
                x = m_root;
            }
        } else {
            Node* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateRight(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rotateLeft(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rotateRight(x->parent);
                x = m_root;
            }
        }
    }
    x->color = BLACK;
}

template<typename Key, typename CmpFn> inline
void RedBlackTree<Key, CmpFn>::print(std::ostream& out, Node* x, size_t level) const
{
    x->print(out);
    out << '\n';
    if (x->left != m_nil) {
        for (size_t i = 0; i < level; i++) out << "     ";
        out << "  ->L";
        print(out, x->left, level+1);
    }
    if (x->right != m_nil) {
        for (size_t i = 0; i < level; i++) out << "     ";
        out << "  ->R";
        print(out, x->right, level+1);
    }
}

} //!jp::ds

} //!profiling
