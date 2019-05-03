#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {


    const int nodeLength = 1024;
    template<class T>
    class deque {
    public:
        int length;

        struct node{
            node *prev,*next;
            T** data;
            int nodeSize;

            node(int nodelen,node *p = NULL,node *n = NULL){
                data = new T*[nodelen];
                nodeSize = 0;
                prev = p;
                next = n;
            }

            node(const node &rhs){
                prev = next = NULL;
                nodeSize = rhs.nodeSize;
                data = new T*[nodeLength];
                for(int i = 0;i < rhs.nodeSize;i++){
                    data[i] = new T(*rhs.data[i]);
                }
            }

            ~node(){
                if(data){
                    for(int i = 0;i < nodeSize;i++){
                        if(data[i]) {
                            delete data[i];
                            data[i] = NULL;
                        }
                    }
                    delete []data;
                    data = NULL;
                }
            }
        };


        node *head,*tail;
        class const_iterator;
        class iterator {
        public:
            deque<T> *que;
            node *currentNode;
            int nodePos;

            iterator(deque<T> *q = NULL,node *cn = NULL,int np = -1){
                que = q;
                currentNode = cn;
                nodePos = np;
            }
            iterator(const iterator &rhs){
                que = rhs.que;
                currentNode = rhs.currentNode;
                nodePos = rhs.nodePos;
            }

        public:

            iterator operator=(const iterator &rhs){
                que = rhs.que;
                currentNode = rhs.currentNode;
                nodePos = rhs.nodePos;
                return rhs;
            }
            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, the behaviour is **undefined**.
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                if(currentNode == NULL) throw invalid_iterator();

                iterator tmp(*this);
                if(n >= 0){
                    int count = n;
                    while(count != 0 && tmp.currentNode->nodeSize - tmp.nodePos <= count) {
                        count -= tmp.currentNode->nodeSize - tmp.nodePos;
                        tmp.currentNode = tmp.currentNode->next;
                        if(tmp.currentNode == NULL) throw index_out_of_bound();
                        tmp.nodePos = 0;
                    }
                    tmp.nodePos = count;
                    return tmp;
                }
                else{
                    int count = -n;
                    while(tmp.nodePos < count && tmp.currentNode != tmp.que->head){
                        count -= tmp.nodePos + 1;
                        tmp.currentNode = tmp.currentNode->prev;
                        if(tmp.currentNode == NULL) throw index_out_of_bound();
                        tmp.nodePos = tmp.currentNode->nodeSize - 1;
                    }
                    tmp.nodePos -= count;
                    std::cout<<"OK\n";
                    return tmp;
                }
            }
            iterator operator-(const int &n) const {
                return operator+(-n);
            }

            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if(que != rhs.que) throw invalid_iterator();
                //this is before rhs
                int count = 0;
                iterator tmp(*this);
                while(tmp.currentNode != rhs.currentNode && tmp.currentNode != NULL){
                    count += tmp.currentNode->nodeSize - tmp.nodePos;
                    tmp.nodePos = 0;
                    tmp.currentNode = tmp.currentNode->next;
                }
                if(tmp.currentNode != NULL){
                    count += rhs.nodePos;
                    return  count;
                }
                //this is after rhs
                count = 0;
                tmp = rhs;
                while(tmp.currentNode != currentNode && tmp.currentNode != NULL){
                    count += tmp.currentNode->nodeSize - tmp.nodePos;
                    tmp.nodePos = 0;
                    tmp.currentNode = tmp.currentNode->next;
                }
                if(tmp.currentNode != NULL){
                    count += nodePos;
                    return count;
                }
            }
            iterator operator+=(const int &n) {
                *this = *this + n;
                return *this;
            }
            iterator operator-=(const int &n) {
                *this = *this - n;
                return *this;
            }
            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator tmp(*this);
                *this += 1;
                return tmp;
            }
            /**
             * TODO ++iter
             */
            iterator& operator++() {
                *this += 1;
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator tmp(*this);
                *this -= 1;
                return tmp;
            }
            /**
             * TODO --iter
             */
            iterator& operator--() {
                *this -= 1;
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const {
                if(currentNode == NULL) throw invalid_iterator();
                if(nodePos > currentNode->nodeSize
                   || nodePos < 0) throw invalid_iterator();

                return *currentNode->data[nodePos];
            }
            /**
             * TODO it->field
             */
            T* operator->() const noexcept {
                if(currentNode == NULL) throw invalid_iterator();
                if(nodePos > currentNode->nodeSize
                   || nodePos < 0) throw invalid_iterator();

                return currentNode->data[nodePos];
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return (que == rhs.que && currentNode == rhs.currentNode
                        && nodePos == rhs.nodePos);
            }
            bool operator==(const const_iterator &rhs) const {
                return (que == rhs.que && currentNode == rhs.currentNode
                        && nodePos == rhs.nodePos);
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
        };
        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        public:
            deque<T> *que;
            node *currentNode;
            int nodePos;

            const_iterator(const deque<T> *q = NULL,node *cn = NULL,int np = -1){
                que = q;
                currentNode = cn;
                nodePos = np;
            }
            const_iterator(const iterator &rhs){
                que = rhs.que;
                currentNode = rhs.currentNode;
                nodePos = rhs.nodePos;
            }

        public:
            const_iterator operator=(const iterator &rhs){
                que = rhs.que;
                currentNode = rhs.currentNode;
                nodePos = rhs.nodePos;
                return rhs;
            }

            const_iterator operator+(const int &n) const {
                const_iterator tmp(*this);
                if(n > 0){
                    int count = n;
                    while(tmp.currentNode->nodeSize - tmp.nodePos <= count) {
                        count -= tmp.currentNode->nodeSize - tmp.nodePos;
                        tmp.currentNode = tmp.currentNode->next;
                        if(tmp.currentNode == NULL) throw index_out_of_bound();
                        tmp.nodePos = 0;
                    }
                    tmp.nodePos = count;
                    return tmp;
                }
                else{
                    int count = -n;
                    while(tmp.nodePos < count){
                        count -= tmp.nodePos + 1;
                        tmp.currentNode = tmp.currentNode->next;
                        if(tmp.currentNode = NULL) throw index_out_of_bound();
                        tmp.nodePos = tmp.currentNode->nodeSize - 1;
                    }
                    tmp.nodePos -= count;
                    return tmp;
                }
            }
            const_iterator operator-(const int &n) const {
                return operator+(-n);
            }

            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if(que != rhs.que) throw invalid_iterator();
                //this is before rhs
                int count = 0;
                const_iterator tmp(*this);
                while(tmp.currentNode != rhs.currentNode && tmp.currentNode != NULL){
                    count += tmp.currentNode->nodeSize - tmp.nodePos;
                    tmp.nodePos = 0;
                    tmp.currentNode = tmp.currentNode->next;
                }
                if(tmp.currentNode != NULL){
                    count += rhs.nodePos;
                    return  count;
                }
                //this is after rhs
                count = 0;
                tmp = rhs;
                while(tmp.currentNode != currentNode && tmp.currentNode != NULL){
                    count += tmp.currentNode->nodeSize - tmp.nodePos;
                    tmp.nodePos = 0;
                    tmp.currentNode = tmp.currentNode->next;
                }
                if(tmp.currentNode != NULL){
                    count += nodePos;
                    return count;
                }
            }
            const_iterator operator+=(const int &n) {
                *this = *this + n;
                return *this;
            }
            const_iterator operator-=(const int &n) {
                *this = *this - n;
                return *this;
            }
            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                iterator tmp(*this);
                *this += 1;
                return tmp;
            }
            /**
             * TODO ++iter
             */
            const_iterator& operator++() {
                *this += 1;
                return *this;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                iterator tmp(*this);
                *this -= 1;
                return tmp;
            }
            /**
             * TODO --iter
             */
            const_iterator& operator--() {
                *this -= 1;
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const {
                if(currentNode == NULL) throw invalid_iterator();
                if(nodePos > currentNode->nodeSize
                   || nodePos < 0) throw invalid_iterator();

                return *currentNode->data[nodePos];
            }
            /**
             * TODO it->field
             */
            T* operator->() const noexcept {
                if(currentNode == NULL) throw invalid_iterator();
                if(nodePos > currentNode->nodeSize
                   || nodePos < 0) throw invalid_iterator();

                return currentNode->data[nodePos];
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return (que == rhs.que && currentNode == rhs.currentNode
                        && nodePos == rhs.nodePos);
            }
            bool operator==(const const_iterator &rhs) const {
                return (que == rhs.que && currentNode == rhs.currentNode
                        && nodePos == rhs.nodePos);
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
        };
        /**
         * TODO Constructors
         */
        deque() {
            length = 0;
            head = new node(nodeLength,NULL,NULL);
            tail = new node(1,head,NULL);
            head->next = tail;
        }
        deque(const deque &other) {
            length = other.length;
            node *tmp = head = new node(*other.head);
            tmp->next = NULL;
            tail = new node(1);
            tail->nodeSize = 0;
            node *p = other.head->next;
            while(p->next != NULL){
                tmp->next = new node(*p);
                tmp->next->prev = tmp;
                tmp = tmp->next;
                p = p->next;
            }
            tmp->next = tail;
            tail->prev = tmp;
        }
        /**
         * TODO Deconstructor
         */
        ~deque() {
            clear();
            delete head;
            delete tail;
        }
        /**
         * TODO assignment operator
         */
        deque &operator=(const deque &other) {
            clear();
            delete head;
            delete tail;

            length = other.length;
            node *tmp = head = new node(*other.head);
            tmp->next = NULL;
            tail = new node(1);
            node *p = other.head->next;
            while(p->next != NULL){
                tmp->next = new node(*p);
                tmp->next->prev = tmp;
                tmp = tmp->next;
                p = p->next;
            }
            tmp->next = tail;
            tail->prev = tmp;
        }
        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T & at(const size_t &pos) {
            if(pos >= length || pos < 0) throw index_out_of_bound();
            iterator tmp(this,head,0);
            tmp += pos;
            return *tmp;
        }
        const T & at(const size_t &pos) const {
            if(pos >= length || pos < 0) throw index_out_of_bound();
            iterator tmp(this,head,0);
            tmp += pos;
            return *tmp;
        }
        T & operator[](const size_t &pos) {
            if(pos >= length || pos < 0) throw index_out_of_bound();
            iterator tmp(this,head,0);
            tmp += pos;
            return *tmp;
        }
        const T & operator[](const size_t &pos) const {
            if(pos >= length || pos < 0) throw index_out_of_bound();
            iterator tmp(this,head,0);
            tmp += pos;
            return *tmp;
        }
        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T & front() const {
            return *head->data[0];
        }
        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T & back() const {
            return *tail->prev->data[tail->prev->nodeSize-1];
        }
        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            iterator tmp(this,head,0);
            return tmp;
        }
        const_iterator cbegin() const {
            iterator tmp(this,head,0);
            return tmp;
        }
        /**
         * returns an iterator to the end.
         */
        iterator end() {
            iterator tmp(this,tail,0);
            return tmp;
        }
        const_iterator cend() const {
            iterator tmp(this,tail,0);
            return tmp;
        }
        /**
         * checks whether the container is empty.
         */
        bool empty() const {
            return length == 0;
        }
        /**
         * returns the number of elements
         */
        size_t size() const {
            return  length;
        }
        /**
         * clears the contents
         */
        void clear() {
            node *tmp = head->next,*p;
            delete head;
            head = new node(nodeLength,NULL,tail);
            tail->prev = head;
            while(tmp != tail){
                p = tmp->next;
                delete tmp;
                tmp = p;
            }
            length = 0;
        }
        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {
            if(pos.que != this) throw invalid_iterator();
            if(pos.currentNode == NULL) throw invalid_iterator();



            length++;

            if(pos == end()){
                pos.currentNode = pos.currentNode->prev;
                pos.nodePos = pos.currentNode->nodeSize;
                pos.currentNode->data[pos.currentNode->nodeSize] = new T(value);
                T* pla = pos.currentNode->data[pos.currentNode->nodeSize];
                pos.currentNode->nodeSize++;
                iterator search(pos.que,pos.currentNode,0);
                adjust(pos.currentNode);
                while(search.currentNode->data[search.nodePos] != pla){
                    search++;
                }
                return search;
            }

            if(pos.nodePos >= pos.currentNode->nodeSize
            || pos.nodePos < 0) throw invalid_iterator();

            for(int i = pos.currentNode->nodeSize - 1;i >= pos.nodePos;i--){
                pos.currentNode->data[i + 1] = pos.currentNode->data[i];
                pos.currentNode->data[i] = NULL;
            }
            pos.currentNode->data[pos.nodePos] = new T(value);
            pos.currentNode->nodeSize++;
            T* pla = pos.currentNode->data[pos.nodePos];
            adjust(pos.currentNode);
            iterator search(pos.que,pos.currentNode,0);
            while(search.currentNode->data[search.nodePos] != pla){
                search++;
            }
            return search;
        }
        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            if(pos.que != this) throw invalid_iterator();
            if(pos.currentNode == NULL) throw invalid_iterator();
            if(pos.nodePos >= pos.currentNode->nodeSize
               ||pos.nodePos < 0) throw invalid_iterator();
            if(pos.currentNode == tail) throw invalid_iterator();
            if(empty()) throw container_is_empty();

            delete pos.currentNode->data[pos.nodePos];
            length--;
            for(int i = pos.nodePos;i < pos.currentNode->nodeSize;i++){
                pos.currentNode->data[i] = pos.currentNode->data[i+1];
                pos.currentNode->data[i+1] = NULL;
            }
            if(pos.currentNode->next == tail && pos.nodePos == pos.currentNode->nodeSize - 1)
                return end();
            else if(pos.currentNode->next == tail && pos.nodePos != pos.currentNode->nodeSize - 1){
                return pos;
            }
            else {
                T *pla = (pos + 1).currentNode->data[(pos + 1).nodePos];
                iterator search(pos.que,pos.currentNode,0);
                adjust(pos.currentNode);
                while(search.currentNode->data[search.nodePos] != pla){
                    search++;
                }
                return  search;
            }
        }
        /**
         * adds an element to the end
         */
        void push_back(const T &value) {
            insert(end(),value);
        }
        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {
            if(length == 0) throw container_is_empty();
            erase(end());
        }
        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {
            insert(begin(),value);
        }
        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {
            if(length == 0) throw container_is_empty();
            erase(begin());
        }

        void adjust(node *&tmp) {
            if (tmp == tail) {
                return;
            }
            if (tmp->nodeSize < nodeLength / 2) {
                merge(tmp);
            }
            if (tmp->nodeSize >= nodeLength) {
                split(tmp);
            }
        }

        void split(node *&tmp) {
            node *p = new node(nodeLength);
            p->prev = tmp;
            p->next = tmp->next;
            tmp->next->prev = p;
            tmp->next = p;

            int temp = tmp->nodeSize / 2;
            for (int i = temp; i < tmp->nodeSize; i++) {
                p->data[i - temp] = tmp->data[i];
                tmp->data[i] = NULL;
            }
            p->nodeSize = tmp->nodeSize - temp;
            tmp->nodeSize = temp;
        }

        void merge(node *&tmp) {
            while (tmp->next->next != NULL &&
                   tmp->nodeSize + tmp->next->nodeSize < nodeLength) {
                if(tmp->next->next != NULL && tmp->nodeSize == 0) {
                    node *p = tmp;
                    tmp = tmp->next;
                    delete p;
                    continue;
                }

                node *p = tmp->next;
                for (int i = tmp->nodeSize; i < tmp->nodeSize + p->nodeSize; i++) {
                    tmp->data[i] = p->data[i - tmp->nodeSize];
                    p->data[i-tmp->nodeSize] = NULL;
                }
                tmp->nodeSize += p->nodeSize;
                p->nodeSize = 0;
                tmp->next = p->next;
                tmp->next->prev = tmp;
                delete p;
                continue;
            }

            if (tmp->next->next != NULL && tmp->nodeSize < nodeLength/ 2) {
                node *p = tmp->next;
                int temp = (nodeLength / 2) - tmp->nodeSize;

                for (int i = 0; i < temp; i++) {
                    tmp->data[tmp->nodeSize + i] = tmp->next->data[i];
                }

                tmp->nodeSize += temp;
                for (int i = 0; i < temp; i++) {
                    p->data[i] = p->data[i + temp];
                    p->data[i+temp] = NULL;
                }
                p->nodeSize -= temp;

            }
        }
    };


}

#endif
