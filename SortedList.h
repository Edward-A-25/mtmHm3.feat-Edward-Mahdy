#pragma once

#include <iostream>
#include <stdexcept>

namespace mtm {
    //                                              helper class
    template<typename T>
    class Block {
    private:
        T block_data;
        Block *next_block;

    public:
        //default constructor ---- to check is there any need for explicit??
        explicit Block(const T &newMember): block_data(newMember), next_block(nullptr) {
        };

        //copy constructor
        Block(const Block &other) = default;

        ~Block() = default;

        //getters to access private parts of the Block
         const T &getData() const;

        Block *getNextPointer() const;

        //setters to edit private parts of the Block
        void setData(const T &data);

        void setNextPointer(Block *other);

        //assignment
        Block &operator=(const Block &other);
    };

    //implementing methods and operators
    template<typename T>
    Block<T> &Block<T>::operator=(const Block &other) {
        this->block_data = other.block_data;
        this->next_block = other.next_block;
        return *this;
    }

    template<typename T>
    const T &Block<T>::getData() const{
        return (this->block_data);
    }

    template<typename T>
    Block<T> *Block<T>::getNextPointer() const {
        return (this->next_block);
    }

    template<typename T>
    void Block<T>::setData(const T &data) {
        this->block_data = data;
    }

    template<typename T>
    void Block<T>::setNextPointer(Block *other) {
        this->next_block = other;
    }

    //helper function to use in constructor and = operator and insert function

    //                                           main class - Sorted List
    template<typename T>
    class SortedList {
        Block<T> *head_block;
        Block<T> *tail_block;
        int list_len;

        //helper functions
        void deleteBlocks();

    public:
        //iterator
        class ConstIterator;

        ConstIterator begin() const;

        ConstIterator end() const;

        //default constructor
        SortedList(): head_block(nullptr), tail_block(nullptr), list_len(0) {
        };

        //copy constructor
        SortedList(const SortedList &other);

        SortedList &operator=(const SortedList &other);

        //destructor
        ~SortedList();

        //Declaring Member Functions
        void insert(const T &new_value); // check later what is const correctnesss
        void remove(const SortedList<T>::ConstIterator &iterator); // revise how entry is written
        int length() const;


        template<typename Condition>
        SortedList filter(Condition c) const;

        template<typename Operation>
        SortedList apply(Operation operation) const;
    };

    template<typename T>
    void SortedList<T>::deleteBlocks() {
        Block<T> *current_ptr = this->head_block;
        while (current_ptr != nullptr) {
            Block<T> *temp = current_ptr;
            current_ptr = current_ptr->getNextPointer();
            delete temp;
        }
        tail_block = nullptr;
        head_block = nullptr;
        list_len = 0;
    }

    //destructor
    template<typename T>
    SortedList<T>::~SortedList() {
        deleteBlocks();
    }


    template<typename T>
    void SortedList<T>::insert(const T &new_value) {
        Block<T> *ptrNewBlock = new Block<T>(new_value); //calls the constructor;

        try {
            //different implementation of mahdy's
            //if the list is empty
            if (head_block == nullptr && tail_block == nullptr) {
                head_block = ptrNewBlock;
                tail_block = ptrNewBlock;
            }
            //if the new value is the biggest in the list
            else if (new_value > head_block->getData()) {
                ptrNewBlock->setNextPointer(head_block);
                head_block = ptrNewBlock;
            }

            //if the new value is in the middle or at the end
            else {
                Block<T> *current_ptr = head_block;
                while (
                    current_ptr->getNextPointer() != nullptr && current_ptr->getNextPointer()->getData() >
                    new_value) {
                    current_ptr = (*current_ptr).getNextPointer();
                }
                ptrNewBlock->setNextPointer(current_ptr->getNextPointer());
                current_ptr->setNextPointer(ptrNewBlock);
                //if it is the smallest in the list -> update the tail
                if (ptrNewBlock->getNextPointer() == nullptr) {
                    tail_block = ptrNewBlock;
                }
            }
            list_len++;
        } catch (...) {
            delete ptrNewBlock;
            throw;
        }
    };

    //the copy constructor
    template<typename T>
    SortedList<T>::SortedList(const SortedList &other) : head_block(nullptr), tail_block(nullptr), list_len(0) {
        try {
            Block<T> *current = other.head_block;
            while (current != nullptr) {
                this->insert(current->getData());
                current = current->getNextPointer();
            }
        } catch (...) {
            deleteBlocks();
            throw;
        }
    };

    //assignment operator
    template<typename T>
    SortedList<T> &SortedList<T>::operator=(const SortedList &other) {
        if (this == &other) {
            return *this;
        }
        //storing the blocks in a temporary list
        SortedList<T> temporaryList;
        try {
            Block<T> *current = other.head_block;
            while (current != nullptr) {
                temporaryList.insert(current->getData());
                current = current->getNextPointer();
            }
        } catch (...) {
            temporaryList.deleteBlocks();
            throw;
        }
        deleteBlocks();
        this->head_block = temporaryList.head_block;
        this->tail_block = temporaryList.tail_block;
        this->list_len = temporaryList.list_len;

        temporaryList.head_block = nullptr;
        temporaryList.tail_block = nullptr;

        return *this;
    };

    template<typename T>
    int SortedList<T>::length() const {
        return list_len;
    }

    template<typename T>
    typename SortedList<T>::ConstIterator SortedList<T>::begin() const {
        return ConstIterator(this, this->head_block);
    }

    template<typename T>
    typename SortedList<T>::ConstIterator SortedList<T>::end() const {
        return ConstIterator(this, nullptr);
    }

    template<typename T>
    template<typename Condition>
    SortedList<T> SortedList<T>::filter(Condition c) const {
        SortedList<T> result;
        for (const auto &block: *this) {
            if (c(block)) {
                result.insert(block);
            }
        }
        return result;
    }

    template<typename T>
    template<typename Operation>
    SortedList<T> SortedList<T>::apply(Operation operation) const {
        SortedList<T> result;
        for (const auto &block: *this) {
            result.insert(operation(block));
        }
        return result;
    }

    template<typename T>
    void SortedList<T>::remove(const SortedList<T>::ConstIterator &iterator) {
        if (iterator.current_block == nullptr) {
            return;
        }
        Block<T> *current = this->head_block;
        Block<T> *previous = nullptr;
        Block<T> *temp_block = nullptr;

        //if we were to remove the head block
        if (iterator.current_block == current) {
            temp_block = head_block;
            head_block = head_block->getNextPointer();
            delete temp_block;
            list_len--;
            if (head_block == nullptr) {
                tail_block = nullptr;
            }
            return;
        }
        while (current != nullptr) {
            if (current == iterator.current_block) {
                previous->setNextPointer(current->getNextPointer());
                //if we remove the last block
                if (previous->getNextPointer() == nullptr) {
                    tail_block = previous;
                }
                delete current;
                list_len--;
                return;
            }
            previous = current;
            current = current->getNextPointer();
        }
    }

    //                                       4.2.2 Iterator implementation
    template<class T>
    class SortedList<T>::ConstIterator {
        const SortedList *list_ptr;
        Block<T> *current_block;

        //constructor
        ConstIterator(const SortedList *givenList, Block<T> *currentBlock): list_ptr(givenList),
                                                                            current_block(currentBlock) {
        }
        ;

        friend class SortedList;

    public:
        //prefix
        ConstIterator &operator++();

        //postfix
        ConstIterator operator++(int);

        //equality
        bool operator!=(const ConstIterator &other) const;

        bool operator==(const ConstIterator &other) const;

        //Derefrence
         const T& operator*() const;

        //big three
        ~ConstIterator() = default;

        ConstIterator(const ConstIterator &other) = default;

        ConstIterator &operator=(const ConstIterator &other) = default;
    };

    //prefix
    template<class T>
    typename SortedList<T>::ConstIterator& SortedList<T>::ConstIterator::operator++() {
        if (current_block == nullptr) {
            throw std::out_of_range("Iterator out of range"); //sdssssssssss check later!!
        }
        current_block = current_block->getNextPointer();
        return (*this);
    }

    //postfix
    template<class T>
    typename SortedList<T>::ConstIterator SortedList<T>::ConstIterator::operator++(int) {
        ConstIterator result = *this;
        ++(*this);
        return (result);
    }

    //derefrence
    template<class T>
    const T& SortedList<T>::ConstIterator::operator*() const {
        if (current_block == nullptr) {
            throw std::out_of_range("Iterator out of range"); //sdssssssssss check later!!
        }
        return (current_block->getData());
    }

    //equality
    template<class T>
    bool SortedList<T>::ConstIterator::operator!=(const ConstIterator &other) const {
        if (current_block == other.current_block && list_ptr == other.list_ptr) {
            return false;
        }
        return true;;
    }

    template<class T>
    bool SortedList<T>::ConstIterator::operator==(const ConstIterator &other) const {
        if (!(*this != other)) {
            return true;
        }
        return false;;
    }
}


/*
 *
 * the class should support the following public interface:s
 * if needed, use =defualt / =delete check later!!
 *
 * constructors and destructor:
 * 1. SortedList() - creates an empty list.
 * 2. copy constructor
 * 3. operator= - assignment operator
 * 4. ~SortedList() - destructor
 *
 * iterator:
 * 5. class ConstIterator;
 * 6. begin method
 * 7. end method
 *
 * functions:
 * 8. insert - inserts a new element to the list
 * 9. remove - removes an element from the list
 * 10. length - returns the number of elements in the list
 * 11. filter - returns a new list with elements that satisfy a given condition
 * 12. apply - returns a new list with elements that were modified by an operation
 */


/*
 * the class should support the following public interface:
 * if needed, use =defualt / =delete
 *
 * constructors and destructor:
 * 1. a ctor(or ctors) your implementation needs
 * 2. copy constructor
 * 3. operator= - assignment operator
 * 4. ~ConstIterator() - destructor
 *
 * operators:
 * 5. operator* - returns the element the iterator points to
 * 6. operator++ - advances the iterator to the next element
 * 7. operator!= - returns true if the iterator points to a different element
 *
 */

/*

template<typename T>
    void Block<T>::insertBlock(Block<T> &prev_block, Block<T> &new_block) {
        // 1. Point the new block to whatever was after the previous block
        new_block.m_next = prev_block.m_next;

        // 2. Point the previous block to the new block
        prev_block.m_next = &new_block;
    }
*/
