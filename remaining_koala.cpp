#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {

template<class T>
class deque {
public:
    const int nodeLength = 256;  //set the max size of each node
    int length; //store the number of elements in dequeue

    //data module
    struct node {
        node *prev, *next;  //pointers, pointing to the previous and next node
        T** data;   //class T pointer array
        int nodeSize;   //the number of elements in this node
        //construction
        node(int s, node *p = NULL, node *n = NULL){
            //initialize
            data = new T*[s];
            nodeSize = 0;
            prev = p;
            next = n;
        }

        //destruction
        ~node(){
            if(this != NULL) {
                if(data) {
                    for (int i = 0; i < nodeSize; i++)
                        if (data[i]) {
                            delete data[i];
                            data[i] = NULL;
                        }
                }
                if(data) delete []data;
                data = NULL;
                prev = next = NULL;
            }
        }
    };

    node *head, *tail;  //pointers, pointing to the head-node and tail-node

    //clear all of the nodes
    void clearAll(){
        if(head == NULL) return;
        node *tmp = head;
        node *del;
        while(tmp){
            del = tmp;
            tmp = tmp->next;
            delete del;
        }
        head = tail = NULL;
        length = 0;
    }

    //search for the NO.rank+1 elements(whose subscript index is rank as well)
    void search(const int rank, node* &pos, int &nodePos){
        //if the rank exceeds  the scope of dequeue
        if(rank >= length){
            pos = NULL;
            nodePos = -1;
            return;
        }
        node *tmp = head;
        unsigned long long counter = rank + 1; //set a counter
        //to find the element in which node
        while(tmp->nodeSize < counter){
            counter -= tmp->nodeSize;
            tmp = tmp->next;
        }
        pos = tmp;
        nodePos = counter-1;
    }


public:
	class const_iterator;
	class iterator {
	    friend class deque;
	    friend class const_iterator;

	private:
	    node *currentNode;  //pointer, pointing to the current node
	    int nodePos;    //store the position in the node, starting from 0

	public:
	    //construction
	    iterator():currentNode(NULL),nodePos(-1){}
	    iterator(node *cn, int np){
	        currentNode = cn;
	        nodePos = np;
	    }
        iterator(const iterator &rhs){
	        currentNode = rhs.currentNode;
	        nodePos = rhs.nodePos;
	    }


	    //overload operator =
	    iterator &operator=(const iterator &other){
	        currentNode = other.currentNode;
	        nodePos = other.nodePos;
	        return *this;
	    }

		//return a new iterator which pointer n-next elements
		//even if there are not enough elements, throw invaild_iterator().
		iterator operator+(const int &n) const {
	        if(currentNode == NULL) throw invalid_iterator();
	        if(n < 0) return (*this)-(-n);
	        iterator tmp(*this);
	        int counter = n;
	        while(counter >= tmp.currentNode->nodeSize - tmp.nodePos){
	            if(tmp.currentNode->next == NULL) break;

	            counter -= (tmp.currentNode->nodeSize - tmp.nodePos);
	            //to the next block
	            tmp.nodePos = 0;
	            tmp.currentNode = tmp.currentNode->next;
	        }
	        tmp.nodePos += counter;
	        if(tmp.nodePos >= tmp.currentNode->nodeSize){
	            if(tmp.currentNode->next == NULL && tmp.nodePos == tmp.currentNode->nodeSize)
                    return tmp;
	            tmp.currentNode = NULL;
	            tmp.nodePos = -1;
	        }
	        return tmp;
		}

		//return a new iterator which pointer n-previous elements
		//even if there are not enough elements, throw invaild_iterator().
		iterator operator-(const int &n) const {
		    if(currentNode == NULL) throw invalid_iterator();
		    if(n < 0) return (*this)+(-n);
		    iterator tmp(*this);
		    int counter = n;
		    while(counter > tmp.nodePos){
		        if(tmp.currentNode->prev == NULL) break;

		        counter -= tmp.nodePos + 1;
		        //to the previous block
		        tmp.nodePos = tmp.currentNode->prev->nodeSize - 1;
		        tmp.currentNode = tmp.currentNode->prev;
		    }
            tmp.nodePos -= counter;
		    if(tmp.nodePos < 0){
		        tmp.currentNode = NULL;
		        tmp.nodePos = -1;
		    }
		    return  tmp;
		}

		// return the distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			int counter = 0;
			node *p = currentNode, *q = rhs.currentNode;
			if(p == q) return nodePos - rhs.nodePos;

			//if p is before q
			while(p != NULL && q != NULL){
                if(p == q) break;
			    counter += p->nodeSize;
                p = p->next;
			}
			if(p != NULL) return -(counter + rhs.nodePos - nodePos);

			//if p is after q
			p = currentNode, q = rhs.currentNode;
			while(p != NULL && q != NULL){
			    if(p == q) break;
			    counter += q->nodeSize;
			    q = q->next;
			}
			if(q != NULL) return counter + rhs.nodePos - nodePos;

			throw invalid_iterator();
		}
		iterator operator+=(const int &n) {
            (*this) = (*this) + n;
            return *this;
		}
		iterator operator-=(const int &n) {
			*this = (*this) - n;
			return *this;
		}
		//iterator++
		iterator operator++(int) {
	        iterator tmp(*this);
            (*this) += 1;
            return tmp;
	    }
		//++iterator
		iterator& operator++() {
            (*this) += 1;
            return *this;
	    }
		iterator operator--(int) {
	        iterator tmp(*this);
            (*this) -= 1;
            return tmp;
	    }
		iterator& operator--() {
            (*this) -= 1;
            return *this;
	    }
	    //iter->field
		T& operator*() const {
	        if(currentNode == NULL) throw invalid_iterator();
	        if(nodePos >= currentNode->nodeSize) throw invalid_iterator();
	        if(nodePos < 0) throw invalid_iterator();
	        return *(currentNode->data[nodePos]);
	    }

		//iter->field
		T* operator->() const noexcept {
            if (currentNode == NULL) throw invalid_iterator();
            if (nodePos >= currentNode->nodeSize) throw invalid_iterator();
            if (nodePos < 0) throw invalid_iterator();
            return (currentNode->data[nodePos]);
        }

        //a operator to check whether two iterators are same
		// (pointing to the same memory).
		bool operator==(const iterator &rhs) const {
            return currentNode == rhs.currentNode && nodePos == rhs.nodePos;
	    }
		bool operator==(const const_iterator &rhs) const {
            return currentNode == rhs.currentNode && nodePos == rhs.nodePos;
	    }
	    //some other operator for iterator.
		bool operator!=(const iterator &rhs) const {
	        return !(currentNode == rhs.currentNode && nodePos == rhs.nodePos);
	    }
		bool operator!=(const const_iterator &rhs) const {
            return !(currentNode == rhs.currentNode && nodePos == rhs.nodePos);
	    }
	};
	class const_iterator {
	    friend class iterator;
		private:
            node *currentNode;
            int nodePos;
		public:
            const_iterator():currentNode(NULL),nodePos(-1){}
            const_iterator(node *cn, int np){
                currentNode = cn;
                nodePos = np;
            }
			const_iterator(const const_iterator &other) {
				currentNode = other.currentNode;
				nodePos = other.nodePos;
			}
			const_iterator(const iterator &other) {
                currentNode = other.currentNode;
                nodePos = other.nodePos;
			}

        //overload operator =
        const_iterator &operator=(const const_iterator &other){
            currentNode = other.currentNode;
            nodePos = other.nodePos;
            return *this;
        }

        //return a new const_iterator which pointer n-next elements
        //even if there are not enough elements, throw invaild_iterator.
        const_iterator operator+(const int &n) const {
            if(currentNode == NULL) throw invalid_iterator();
            if(n < 0) return (*this)-(-n);
            iterator tmp(*this);
            int counter = n;
            while(counter >= tmp.currentNode->nodeSize - tmp.nodePos){
                if(tmp.currentNode->next == NULL) break;

                counter -= (tmp.currentNode->nodeSize - tmp.nodePos);
                //to the next block
                tmp.nodePos = 0;
                tmp.currentNode = tmp.currentNode->next;
            }
            tmp.nodePos += counter;
            //if(tmp.nodePos >= tmp.currentNode->nodeSize){
            //    if(tmp.currentNode == tail)
            //        return tmp;
            //    tmp.currentNode = NULL;
            //    tmp.nodePos = -1;
            //}
            return tmp;
        }
        //return a new iterator which pointer n-previous elements
        //even if there are not enough elements, throw invaild_iterator().
        const_iterator operator-(const int &n) const {
            if(currentNode == NULL) throw invalid_iterator();
            if(n < 0) return (*this)+(-n);
            iterator tmp(*this);
            int counter = n;
            while(counter > tmp.nodePos){
                if(tmp.currentNode->prev == NULL) break;

                counter -= tmp.nodePos + 1;
                //to the previous block
                tmp.nodePos = tmp.currentNode->prev->nodeSize - 1;
                tmp.currentNode = tmp.currentNode->prev;
            }
            tmp.nodePos -= counter;
            //if(tmp.nodePos < 0){
            //    tmp.currentNode = NULL;
            //    tmp.nodePos = -1;
            //}
            return  tmp;
        }

        // return the distance between two iterator,
        // if these two iterators points to different vectors, throw invaild_iterator().
        int operator-(const const_iterator &rhs) const {
            int counter = 0;
            node *p = currentNode, *q = rhs.currentNode;
            if(p == q) return nodePos - rhs.nodePos;

            //if p is before q
            while(p != NULL && q != NULL){
                if(p == q) break;
                counter += p->nodeSize;
                p = p->next;
            }
            if(p != NULL) return -(counter + rhs.nodePos - nodePos);

            //if p is after q
            p = currentNode, q = rhs.currentNode;
            while(p != NULL && q != NULL){
                if(p == q) break;
                counter += q->nodeSize;
                q = q->next;
            }
            if(q != NULL) return counter + rhs.nodePos - nodePos;

            throw invalid_iterator();
        }
        const_iterator operator+=(const int &n) {
            (*this) = (*this) + n;
            return *this;
        }
        const_iterator operator-=(const int &n) {
            *this = (*this) - n;
            return *this;
        }
        //iterator++
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            (*this) += 1;
            return tmp;
        }
        //++iterator
        const_iterator& operator++() {
            (*this) += 1;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp(*this);
            (*this) -= 1;
            return tmp;
        }
        const_iterator& operator--() {
            (*this) -= 1;
            return *this;
        }
        //iter->field
        T& operator*() const {
            if(currentNode == NULL) throw invalid_iterator();
            if(nodePos >= currentNode->nodeSize) throw invalid_iterator();
            if(nodePos < 0) throw invalid_iterator();
            return *(currentNode->data[nodePos]);
        }

        //iter->field
        T* operator->() const noexcept {
            if (currentNode == NULL) throw invalid_iterator();
            if (nodePos >= currentNode->nodeSize) throw invalid_iterator();
            if (nodePos < 0) throw invalid_iterator();
            return (currentNode->data[nodePos]);
        }

        //a operator to check whether two iterators are same
        // (pointing to the same memory).
        bool operator==(const const_iterator &rhs) const {
            return currentNode == rhs.currentNode && nodePos == rhs.nodePos;
        }
        bool operator==(const iterator &rhs) const {
            return currentNode == rhs.currentNode && nodePos == rhs.nodePos;
        }
        //some other operator for iterator.
        bool operator!=(const const_iterator &rhs) const {
            return !(currentNode == rhs.currentNode && nodePos == rhs.nodePos);
        }
        bool operator!=(const iterator &rhs) const {
            return !(currentNode == rhs.currentNode && nodePos == rhs.nodePos);
        }

	};
	//construction
	deque() {
	    head = tail = new node(nodeLength,NULL,NULL);
	    length = 0;
	}
	deque(const deque &other) {
	    node *p = head =new node(nodeLength,NULL,NULL);
	    node *q = other.head;
	    length = other.length;
	    p->nodeSize = q->nodeSize;
	    if(length == 0) {tail = head; return;}
	    while(q != NULL){
	        p->nodeSize = q->nodeSize;
           for(int i = 0;i < q->nodeSize;i++){
                p->data[i] = new T(*(q->data[i]));
           }
	        p = p->next = new node(nodeLength,p,NULL);
	        q = q->next;
	    }
	    tail = p->prev;
	    tail->next = NULL;
	    delete p;
	}
	//destruction
	~deque() {if(this != NULL) clearAll();}
	//overload operator =
	deque &operator=(const deque &other) {
        if(this->head == other.head) return *this;
        clearAll();
        node *p = head =new node(nodeLength,NULL,NULL);
        node *q = other.head;
        length = other.length;
        p->nodeSize = q->nodeSize;
        if(length == 0) {tail = head; return *this;}
        while(q != NULL){
            p->nodeSize = q->nodeSize;
            for(int i = 0;i < p->nodeSize;i++){
                p->data[i] = new T(*(q->data[i]));
            }
            p = p->next = new node(nodeLength,p,NULL);
            q = q->next;
        }
        tail = p->prev;
        delete p;
        return *this;
	}

	//access specified element with bounds checking
	//throw index_out_of_bound if out of bound.
	T & at(const size_t &pos) {
	    if(pos >= length) throw index_out_of_bound();
	    node *currentNode;
	    int nodePos;
	    search(pos,currentNode,nodePos);
	    return *(currentNode->data[nodePos]);
	}
	const T & at(const size_t &pos) const {
        if(pos >= length) throw index_out_of_bound();
        node *currentNode;
        int nodePos;
        search(pos,currentNode,nodePos);
        return *(currentNode->data[nodePos]);
	}
	T & operator[](const size_t &pos) {
        if(pos >= length) throw index_out_of_bound();
        node *currentNode;
        int nodePos;
        search(pos,currentNode,nodePos);
        return *(currentNode->data[nodePos]);
	}
	const T & operator[](const size_t &pos) const {
        if(pos >= length) throw index_out_of_bound();
        node *currentNode;
        int nodePos;
        search(pos,currentNode,nodePos);
        return *(currentNode->data[nodePos]);
	}
    //access the first element
    // throw container_is_empty when the container is empty
	const T & front() const {
	    if(length == 0) throw container_is_empty();
	    return *(head->data[0]);
	}
	//access the last element
	//throw container_is_empty when the container is empty.
	const T & back() const {
	    if(length == 0) throw container_is_empty();
	    return *(tail->data[tail->nodeSize-1]);
	}
	//returns an iterator to the beginning.
	iterator begin() {
	    iterator it(head,0);
	    return it;
	}
	const_iterator cbegin() const {
	    iterator it(head,0);
	    return it;
	}
	//returns an iterator to the end.
	iterator end() {
	    iterator it(tail,tail->nodeSize);
	    return it;
	}
	const_iterator cend() const {
	    iterator it(tail,tail->nodeSize);
	    return it;
	}

	//checks whether the container is empty.
	bool empty() const {return length == 0;	}
	//returns the number of elements
	size_t size() const {return	length;}

	//clears the contents
	void clear() {clearAll();}

	 //inserts elements at the specified location on in the container.
	 //inserts value before pos
	 //returns an iterator pointing to the inserted value
	 //throw if the iterator is invalid or it point to a wrong place.
	iterator insert(iterator pos, const T &value) {
	    if(pos.currentNode == NULL) throw invalid_iterator();
	    node* tmp = pos.currentNode;
	    while(tmp->prev != NULL)
	        tmp = tmp->prev;
	    if(tmp != head) throw invalid_iterator();
        if(pos.currentNode != tail && pos.nodePos >= pos.currentNode->nodeSize) throw invalid_iterator();
        if(pos.currentNode == tail && pos.nodePos > pos.currentNode->nodeSize) throw invalid_iterator();
        if(pos.nodePos < 0) throw invalid_iterator();

        length++;

        if(tail->nodeSize == nodeLength && pos == this->end()){
            tail = tail->next = new node(nodeLength, tail, NULL);
            tail->data[0] = new T(value);
            tail->nodeSize++;
            pos.currentNode = tail;
            pos.nodePos = 0;
            return pos;
        }
        else if(tail->nodeSize != nodeLength && pos == this->end()){
            tail->data[tail->nodeSize] = new T(value);
            tail->nodeSize++;
            return pos;
        }
        else;

        node *tmpNode = pos.currentNode;
        int tmpNodePos = pos.nodePos;
        if(tmpNode->nodeSize == nodeLength){
            tmpNode->next = new node(nodeLength,tmpNode,tmpNode->next);
            tmpNode->next->next->prev =tmpNode->next;
            for(int i = tmpNodePos;i < tmpNode->nodeSize;i++){
                tmpNode->next->data[i-tmpNodePos] = new T(*tmpNode->data[i]);
                delete tmpNode->data[i];
                tmpNode->data[i] = NULL;
            }
            tmpNode->next->nodeSize = tmpNode->nodeSize - tmpNodePos;
            tmpNode->nodeSize = tmpNodePos + 1;
            //if(tmpNode->data[tmpNodePos]) delete tmpNode->data[tmpNodePos];
            tmpNode->data[tmpNodePos] = new T(value);
            return pos;
        }

        for(int i = tmpNode->nodeSize-1;i >= tmpNodePos;i--){
            tmpNode->data[i+1] = new T(*(tmpNode->data[i]));
            delete tmpNode->data[i];
        }
        tmpNode->nodeSize++;
        tmpNode->data[tmpNodePos] = new T(value);
        return pos;
	}
	 //removes specified element at pos.
	 //removes the element at pos.
	 //returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 // throw if the container is empty, the iterator is invalid or it points to a wrong place.
	iterator erase(iterator pos) {
	    if(pos.currentNode == NULL) throw invalid_iterator();
	    node* tmp = pos.currentNode;
	    while(tmp->prev != NULL)
	        tmp = tmp->prev;
	    if(tmp != head) throw invalid_iterator();
        if(pos.nodePos >= pos.currentNode->nodeSize) throw invalid_iterator();
        if(pos.nodePos < 0) throw invalid_iterator();

        length--;
        node *tmpNode = pos.currentNode;
        int tmpNodePos = pos.nodePos;
        if(tmpNode->nodeSize == 1){
            if(length == 0){
                delete head->data[0];
                head->nodeSize = 0;
                return begin();
            }
            if(tmpNode == head){
                head = head->next;
                delete tmpNode;
                head->prev = NULL;
                return begin();
            }
            if(tmpNode == tail){
                tail = tail->prev;
                delete tmpNode;
                tail->next = NULL;
                return end();
            }
            tmpNode->prev->next = tmpNode->next;
            tmpNode->next->prev = tmpNode->prev;
            iterator it(tmpNode->next,0);
            delete tmpNode;
            return it;
        }
        for(int i = tmpNodePos;i < tmpNode->nodeSize - 1;i++){
            delete tmpNode->data[i];
            tmpNode->data[i] = new T(*(tmpNode->data[i+1]));
        }
        tmpNode->nodeSize--;
        delete tmpNode->data[tmpNode->nodeSize];
         tmpNode->data[tmpNode->nodeSize] = NULL;
        return pos;
	}

	// adds an element to the end
	void push_back(const T &value) {
	    length++;
	    if(tail->nodeSize == nodeLength){
	        tail = tail->next = new node(nodeLength,tail,NULL);
	        tail->nodeSize = 1;
	        tail->data[0] = new T(value);
	        return;
	    }
	    tail->data[tail->nodeSize] = new T(value);
	    tail->nodeSize++;
	}
	 //removes the last element
	 //throw when the container is empty.
	void pop_back() {
	    if(length == 0) throw container_is_empty();
	    if(length == 1){
	        if(tail->data[0]) delete tail->data[0];
	        tail->nodeSize = 0;
	        tail->data[0] = NULL;
	        length = 0;
	        return;
	    }
        length--;
	    if(tail->nodeSize == 1){
	        node *tmp=tail;
	        tail = tail->prev;
	        tail->next = NULL;
	        if(tmp) delete tmp;
	        return;
	    }
	    delete tail->data[tail->nodeSize-1];
	    tail->data[tail->nodeSize-1] = NULL;
	    tail->nodeSize--;
	}
	//inserts an element to the beginning.
	void push_front(const T &value) {
	    length++;
	    if(head->nodeSize < nodeLength){
	        for(int i = head->nodeSize-1;i >= 0;i--){
	            head->data[i+1] = new T(*(head->data[i]));
	            delete head->data[i];
	        }
	        head->data[0] = new T(value);
	        head->nodeSize++;
	        return;
	    }
	    head = head->prev = new node(nodeLength,NULL,head);
	    head->data[0] = new T(value);
	    head->nodeSize++;
	}
	//removes the first element.
	//throw when the container is empty.
	void pop_front() {
	    if(length == 0) throw container_is_empty();
	    if(length == 1){
	        head->nodeSize = 0;
	        delete head->data[0];
	        head->data[0] = NULL;
	        length = 0;
	        return;
	    }
	    length--;
	    if(head->nodeSize == 1){
	        node *tmp = head;
	        head = head->next;
	        head->prev = NULL;
	        delete tmp;
	        return;
	    }
	    for(int i = 0;i < head->nodeSize-1;i++){
	        delete head->data[i];
	        head->data[i] = new T(*(head->data[i+1]));
	    }
	    head->nodeSize--;
	    delete head->data[head->nodeSize];
	    head->data[head->nodeSize] = NULL;
	}
};

}

#endif
