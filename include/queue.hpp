#pragma once

#include <memory_resource>
#include <vector>
#include <concepts>
#include <memory>
#include <exception>
#include <algorithm>

template <class ElementType, class Alloc>
class CustomQueue;
template <class ElementType, class Alloc>
    requires std::is_default_constructible_v<ElementType> && std::is_same_v<Alloc, std::pmr::polymorphic_allocator<ElementType>>

class CustomQueue {
private:
    struct Node {
        ElementType value;
        Node *next_ptr;

        Node() : value(), next_ptr(nullptr) {}
        Node(const ElementType &val) : value(val), next_ptr(nullptr) {}
        Node(ElementType &&val) : value(std::move(val)), next_ptr(nullptr) {}
        ~Node() = default;
    };

    using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    
    NodeAlloc node_alloc;
    Node *first_node = nullptr;
    Node *last_node = nullptr;
    size_t count = 0;

public:
    Traverser begin() { return start(); }
    Traverser end() { return finish(); }
    Traverser begin() const { return start(); }
    Traverser end() const { return finish(); }
    explicit CustomQueue(const Alloc &alloc = {}) : node_alloc(alloc) {}

    void enqueue(const ElementType &val) {
        Node *new_node = node_alloc.allocate(1);
        try {
            node_alloc.construct(new_node, val);
        } catch (...) {
            node_alloc.deallocate(new_node, 1);
            throw;
        }

        if (!last_node) {
            first_node = last_node = new_node;
        } else {
            last_node->next_ptr = new_node;
            last_node = new_node;
        }
        count++;
    }

    void enqueue(ElementType &&val) {
        Node *new_node = node_alloc.allocate(1);
        try {
            node_alloc.construct(new_node, std::move(val));
        } catch (...) {
            node_alloc.deallocate(new_node, 1);
            throw;
        }

        if (!last_node) {
            first_node = last_node = new_node;
        } else {
            last_node->next_ptr = new_node;
            last_node = new_node;
        }
        count++;
    }

    void remove_front() {
        if (!first_node) { return; }

        Node *temp = first_node;
        first_node = first_node->next_ptr;

        if (!first_node) { last_node = nullptr; }

        node_alloc.destroy(temp);
        node_alloc.deallocate(temp, 1);
        count--;
    }

    size_t length() const { return count; }

    bool empty() const { return count == 0; }

    ElementType& get_front() { return first_node->value; }
    const ElementType& get_front() const { return first_node->value; }

    ElementType& get_back() { return last_node->value; }
    const ElementType& get_back() const { return last_node->value; }
    
    void purge() {
        while (!empty()) { remove_front(); }
    }

    ~CustomQueue() {
        purge();
    }

    class Traverser {
    public:
        using category = std::forward_iterator_tag;
        using value_type = ElementType;
        using difference = std::ptrdiff_t;
        using pointer = ElementType*;
        using reference = ElementType&;

        Traverser() : current_node(nullptr) {}
        explicit Traverser(Node *node) : current_node(node) {}

        reference operator*() const{ return current_node->value; }
        pointer operator->() const { return &current_node->value; }

        Traverser &operator++() {
            if (current_node) { current_node = current_node->next_ptr; }
            return *this;
        }

        Traverser operator++(int) {
            Traverser temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Traverser &other) const { return current_node == other.current_node; }
        bool operator!=(const Traverser &other) const { return !(*this == other); }

    private:
        Node *current_node;
    };


};
