#pragma once

#include <iostream>
#include <stdexcept>

namespace mtm {

    template<class T>
    class Node {
    public:
        T m_data;
        Node *m_next;

        explicit Node(const T &data);

        Node(const Node &toCopy) = default;
        Node &operator=(const Node &node) = default;
        ~Node() = default;

        Node *nodeGetNext() const;
        T &nodeGetData();
        bool operator==(const Node<T> &other);
    };

    template<class T>
    bool Node<T>::operator==(const Node<T> &other) {
        return this->m_data == other.m_data;
    }

    template<class T>
    Node<T>::Node(const T &data) : m_data(data), m_next(nullptr) {}

    template<class T>
    Node<T> *Node<T>::nodeGetNext() const {
        return this->m_next;
    }

    template<class T>
    T &Node<T>::nodeGetData() {
        return this->m_data;
    }

    template<class T>
    class SortedList {
    public:
        class ConstIterator;

        ConstIterator begin() const;
        ConstIterator end() const;

        SortedList();
        SortedList(const SortedList &other);
        SortedList &operator=(const SortedList &other);
        ~SortedList();

        void insert(const T &insert_value);
        void remove(const SortedList<T>::ConstIterator &iterator);
        int length() const;

        template<class Condition>
        SortedList<T> filter(Condition condition) const;

        template<class Operation>
        SortedList<T> apply(Operation operation) const;

        bool operator==(const SortedList &other);

    private:
        Node<T> *m_head;
        Node<T> *m_end;
        int m_length;

        void deleteAllNodes();
    };

    template<class T>
    void SortedList<T>::deleteAllNodes() {
        while (m_head != nullptr) {
            Node<T> *toDelete = m_head;
            m_head = m_head->m_next;
            delete toDelete;
        }
        m_end = nullptr;
        m_length = 0;
    }

    template<class T>
    bool SortedList<T>::operator==(const SortedList<T> &other) {
        if (m_length != other.m_length)
            return false;

        Node<T> *current_this = m_head;
        Node<T> *current_other = other.m_head;
        while (current_this != nullptr) {
            if (current_this->nodeGetData() != current_other->nodeGetData()) {
                return false;
            }
            current_this = current_this->m_next;
            current_other = current_other->m_next;
        }
        return true;
    }

    template<class T>
    SortedList<T>::SortedList() : m_head(nullptr), m_end(nullptr), m_length(0) {}

    template<class T>
    SortedList<T>::SortedList(const SortedList &other) : m_head(nullptr), m_end(nullptr), m_length(0) {
        try {
            Node<T> *current = other.m_head;
            while (current != nullptr) {
                insert(current->nodeGetData());
                current = current->m_next;
            }
        } catch (...) {
            deleteAllNodes();
            throw;
        }
    }

    template<class T>
    SortedList<T> &SortedList<T>::operator=(const SortedList<T> &other) {
        if (this == &other) {
            return *this;
        }

        SortedList<T> temp;
        try {
            Node<T> *current = other.m_head;
            while (current != nullptr) {
                temp.insert(current->nodeGetData());
                current = current->m_next;
            }
        } catch (...) {
            temp.deleteAllNodes();
            throw;
        }

        deleteAllNodes();

        m_head = temp.m_head;
        m_end = temp.m_end;
        m_length = temp.m_length;

        temp.m_head = nullptr;
        temp.m_end = nullptr;
        temp.m_length = 0;

        return *this;
    }

    template<class T>
    SortedList<T>::~SortedList() {
        deleteAllNodes();
    }

    template<class T>
    void SortedList<T>::insert(const T &insert_value) {
        auto *new_node = new Node<T>(insert_value);
        try {
            if (m_head == nullptr || !(m_head->m_data > insert_value)) {
                new_node->m_next = m_head;
                m_head = new_node;
                if (m_end == nullptr) {
                    m_end = new_node;
                }
            } else {
                Node<T> *current = m_head;
                while (current->m_next != nullptr && (current->m_next->m_data > insert_value)) {
                    current = current->m_next;
                }
                new_node->m_next = current->m_next;
                current->m_next = new_node;
                if (new_node->m_next == nullptr) {
                    m_end = new_node;
                }
            }
            m_length++;
        } catch (...) {
            delete new_node;
            throw;
        }
    }

    template<class T>
    int SortedList<T>::length() const {
        return m_length;
    }

    template<class T>
    template<class Condition>
    SortedList<T> SortedList<T>::filter(Condition condition) const {
        SortedList<T> result;
        for (ConstIterator i = this->begin(); i != this->end(); ++i) {
            if (condition(*i)) {
                result.insert(*i);
            }
        }
        return result;
    }

    template<class T>
    template<class Operation>
    SortedList<T> SortedList<T>::apply(Operation operation) const {
        SortedList<T> result;
        Node<T> *current = m_head;
        while (current != nullptr) {
            result.insert(operation(current->nodeGetData()));
            current = current->m_next;
        }
        return result;
    }

    template<class T>
    typename SortedList<T>::ConstIterator SortedList<T>::begin() const {
        return ConstIterator(this, this->m_head);
    }

    template<class T>
    typename SortedList<T>::ConstIterator SortedList<T>::end() const {
        return ConstIterator(this, nullptr);
    }

    template<class T>
    class SortedList<T>::ConstIterator {
    public:
        ~ConstIterator() = default;

        ConstIterator(const ConstIterator &other) = default;
        ConstIterator &operator=(const ConstIterator &other) = default;

        const T &operator*() const;

        bool operator!=(const ConstIterator &other) const;
        ConstIterator &operator++();
        ConstIterator operator++(int);
        bool operator==(const ConstIterator &other) const;

    private:
        const SortedList *m_SortedList;
        Node<T> *m_node;

        ConstIterator(const SortedList *sortedList, Node<T> *node);
        friend class SortedList;
    };

    template<class T>
    SortedList<T>::ConstIterator::ConstIterator(const SortedList *sortedList, Node<T> *node) :
            m_SortedList(sortedList), m_node(node) {}

    template<class T>
    const T &SortedList<T>::ConstIterator::operator*() const {
        if (m_node == nullptr) {
            throw std::out_of_range("Iterator out of range");
        }
        return m_node->nodeGetData();
    }

    template<class T>
    typename SortedList<T>::ConstIterator &SortedList<T>::ConstIterator::operator++() {
        if (m_node == nullptr) {
            throw std::out_of_range("Iterator out of range");
        }
        m_node = m_node->nodeGetNext();
        return *this;
    }

    template<class T>
    typename SortedList<T>::ConstIterator SortedList<T>::ConstIterator::operator++(int) {
        ConstIterator result = *this;
        ++(*this);
        return result;
    }

    template<class T>
    bool SortedList<T>::ConstIterator::operator==(const ConstIterator &other) const {
        return (this->m_node == other.m_node && this->m_SortedList == other.m_SortedList);
    }

    template<class T>
    bool SortedList<T>::ConstIterator::operator!=(const ConstIterator &other) const {
        return !(*this == other);
    }

    template<class T>
    void SortedList<T>::remove(const SortedList<T>::ConstIterator &iterator) {
        if (iterator.m_node == nullptr) {
            return;
        }

        Node<T> *current = this->m_head;
        Node<T> *previous = nullptr;

        if (current == iterator.m_node) {
            Node<T> *toDelete = current;
            m_head = m_head->m_next;
            delete toDelete;
            m_length--;
            if (m_head == nullptr) {
                m_end = nullptr;
            }
            return;
        }

        while (current != nullptr && current != iterator.m_node) {
            previous = current;
            current = current->m_next;
        }

        if (current == nullptr) {
            return;
        }

        Node<T> *toDelete = current;
        previous->m_next = current->m_next;
        if (current == m_end) {
            m_end = previous;
        }
        delete toDelete;
        m_length--;
    }

} // namespace mtm
