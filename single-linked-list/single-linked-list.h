#pragma once

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value = Type{};
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_(node) {}

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return rhs.node_ == node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return rhs.node_ != node_;
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return rhs.node_ == node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return rhs.node_ != node_;
        }

        BasicIterator& operator++() noexcept {
            if (node_)
                node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto old_value(*this);
            ++(*this);
            return old_value;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr); // Add assertion to check for nullptr
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr); // Add assertion to check for nullptr
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return BasicIterator<Type>(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return BasicIterator<Type>(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return BasicIterator<Type>(head_.next_node);
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return BasicIterator<Type>(nullptr);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return BasicIterator<Type>(head_.next_node);
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return BasicIterator<Type>(nullptr);
    }

    SingleLinkedList() = default;
    SingleLinkedList(std::initializer_list<Type> values) {
        if (values.size()) {
            Node* next;
            bool first = true;
            for (auto value : values) {
                if (first) {
                    head_.next_node = new Node(value, nullptr);
                    next = head_.next_node;
                    ++size_;
                    first = false;
                } else {
                    next->next_node = new Node(value, nullptr);
                    next = next->next_node;
                    ++size_;
                }
            }
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        if (!other.IsEmpty()) {
            SingleLinkedList temp;
            Node* next;
            bool first = true;
            for (auto value : other) {
                if (first) {
                    temp.head_.next_node = new Node(value, nullptr);
                    next = temp.head_.next_node;
                    ++temp.size_;
                    first = false;
                } else {
                    next->next_node = new Node(value, nullptr);
                    next = next->next_node;
                    ++temp.size_;
                }
            }
            swap(temp);
        }
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (&rhs != this) {
            SingleLinkedList temp(rhs);
            swap(temp);
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return !size_;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            auto element = head_.next_node;
            head_.next_node = element->next_node;
            delete element;
        }
        size_ = 0;
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return BasicIterator<Type>(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_)); 
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr); // Add assertion to check for nullptr
        Node* add_elem = new Node(value, pos.node_->next_node);
        pos.node_->next_node = add_elem;
        ++size_;
        return BasicIterator<Type>(add_elem);
    }

    void PopFront() noexcept {
        if (!IsEmpty()) {
            auto erase_el = head_.next_node;
            head_.next_node = erase_el->next_node;
            delete erase_el;
            --size_;
        }
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (pos.node_ && pos.node_->next_node) {
            Node* del_elem = pos.node_->next_node;
            pos.node_->next_node = del_elem->next_node;
            delete del_elem;
            --size_;
        }
        return Iterator(pos.node_->next_node);
    }

    ~SingleLinkedList() {
        Clear();
    }

private:
    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs > lhs);
}
