//
// Created by 郑文鑫 on 2019-03-09.
//

#include "utility.hpp"
#include <functional>
#include <cstddef>
#include "exception.hpp"
#include <map>
namespace sjtu {
    const int maxn = 4096;
    int ID;
    template <class Key, class Value, class Compare = std::less<Key> >
    class BTree {
    public:
        typedef pair<Key, Value> value_type;
        //typedef ssize_t node_t;
        //typedef ssize_t offset_t;
        class iterator;
        class const_iterator;
    private:
        //the offset store the information of the bpt
        const int bpt_info_offset = 0;
        //the number of key in one block
        static const int M = sizeof(Key) > maxn / 5 ?
                             5 : maxn / sizeof(Key);

        //the number of pair(key,value) in one block
        static const int L = sizeof(value_type) > maxn / 5?
                             5 : maxn / sizeof(value_type);
        //static const int M = 4;
        //static const int L = 4;
        //static const int minL = (L + 1) / 2;
        //static const int minM = (M + 1) / 2;

        struct fileName{
            char *name;
            fileName(){name = new char[20];}
            ~fileName(){if(name) delete name;}

            void nameName(int ID){
                ///if(ID < 0 || ID > 99) I don't know,
                /// maybe nothing can be done.
                name[0] = 'd';
                name[1] = 'a';
                name[2] = 't';
                name[3] = '0';
                name[4] = '.';
                name[5] = 'd';
                name[6] = 'a';
                name[7] = 't';
                name[8] = '\0';
            }
        };

        struct bpt_Info{
            ssize_t root;   //root of the tree
            ssize_t head;   //head of the leaf
            ssize_t tail;   //tail of the leaf
            size_t size;    //size of leaf of the tree
            ssize_t eof;    //end of file
            //construct
            bpt_Info(){
                root = 0;
                head = 0;
                tail = 0;
                size = 0;
                eof = 0;
            }
            //destruct
            ~bpt_Info(){}
        };

        ///       key[   k1 k2 k3 k4 k5 .....]
        ///           ^  ^  ^  ^  ^  ^
        ///  child[  c1 c2 c3 c4 c5 c6 .....]
        struct internalNode{
            ssize_t offset; //address in file
            ssize_t parent; //pointer to parent
            ssize_t child[M + 1];   //pointer_array to children
            Key key[M + 1]; //key
            int num;    ///number of the key???
            int type;
            //construct
            ///if type is true, this node's child is leaf,
            ///else its child is not leaf.
            internalNode(){
                num = 0;
                offset = 0;
                parent = 0;
                type = 1;
                //for(int i = 0;i <= M;i++)
                //    child[i] = 0;
            }
            //~internalNode(){}
        };

        struct leafNode{
            ssize_t offset; //offset
            ssize_t parent; //pointer to parent
            ssize_t prev;   //pointer to previous leaf
            ssize_t next;   //pointer to next leaf
            int num;    //number of the pair(key,value)
            value_type data[L + 1]; //data_array
            //construct
            leafNode(){
                offset = 0;
                parent = 0;
                prev = 0;
                next = 0;
                num = 0;
            }
            ~leafNode(){}
        };

        FILE *fp;
        fileName fp_name;
        bpt_Info info;
        bool fp_open;

        bool exist;

        /**
         * some file operation
         * open(): open the file,
         * if it has already been opened, nothing should be done
         * close(): close the file
         * if it has already been closed, nothing should be done
         * read(): read information from the file to the storage address
         * write(): write information from the storage address to the file
         */

        /**  FILE * fopen(const char * path, const char * mode);
            *   mode is the type to open file;
            *   rb+ is Open a binary file in read/write mode, allowing only read/write data.
            *   w is Open the write-only file, if the file exists, the file length is zero,
            *   that is, the contents of the file will disappear, if the file does not exist,
             create the file.
            */
        void open(){
            //file has already been opened
            //std::cout<<"file opening\n";
            if(!fp_open){
                //std::cout<<"file hasn't benn opened\n";
                fp = fopen(fp_name.name,"rb+");
                if(fp == NULL){
                    fp = fopen(fp_name.name,"w");
                    fclose(fp);
                    fp = fopen(fp_name.name,"rb+");
                    exist = false;
                }
                else{
                    exist = true;
                    read(&info,bpt_info_offset,1, sizeof(bpt_Info));
                }
                fp_open = true;
            }
            //std::cout<<"open-operation complete\n";
        }

        void close(){
            if(fp_open){
                fclose(fp);
                fp_open = false;
            }
            else;   //file has already been closed
        }

        /**
         *
         * @param place : none-type pointer to address,
         * where store the information copy from the file.
         * @param offset : offset
         * @param num : maybe one byte...
         * @param size : size of data-type
         */
        void read(void* place,ssize_t offset,size_t num,size_t size){
            /**
             *  int fseek(FILE *stream, long offset, int fromwhere)
             *  If the execution succeeds, the stream points to a position
             *  with fromwhere as the base, offset offset (pointer offset) bytes,
             *  and the function returns 0.
             *  If execution fails, the position to which the stream points
             *  is not changed, and the function returns a non-0 value.
             */

            /**
             *  size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
             *  this function return the true number of read, if it doesn't match count ,
             *  there must be some errors...
             *  Buffer : Memory address used to receive data
             *  Size : The number of bytes per data item to read, in bytes
             *  Count : To read the Count data item, each data item is a size byte.
             *  Stream : Input stream
             */
            //open();
            if(fseek(fp,offset,SEEK_SET))
                std::cout<<"read-failure\n";///something should be done?I don't know.
            fread(place,size,num,fp);
            //size_t result = fread(place,num,size,fp);
            //if(result != size); ///there must be some errors
            ///something should be done, but I don't know.
        }

        void write(void* place,ssize_t offset,size_t num,size_t size){
            /**
             * Size_t fwrite (const void buffer, size_t size, size_t count, FILE Stream);
             * Return value: Returns the number of blocks of data actually written
             * Buffer: is a pointer, for Fwrite, is to get the address of the data;
             * Size: The number of single bytes to write to the content;
             * Count: The number of data items to be written to the size byte;
             * Stream: target file pointer;
             * Returns the count of the number of data items actually written.
             */
            //std::cout<<"write operating\n";
            if(fseek(fp,offset,SEEK_SET));///something should be done, but I don't know.
            fwrite(place,size,num,fp);
            //std::cout<<"write-operation complete\n";
            //size_t result = fwrite(place,num,size,fp);
            //if(result != size);///there must be some errors
            ///something should be done, but I don't know.
        }

        /**initialize the bpt
         * there would be one information place,
         * one root and one leaf
         */
        void init(){
            if(exist){
                //read(&info,bpt_info_offset,1, sizeof(bpt_Info));
                return;
            }
            //std::cout<<"initializing\n";
            info.size = 0;  //size of the tree is zero
            info.eof = bpt_info_offset;
            info.eof += sizeof(bpt_Info);
            internalNode root;
            info.root = root.offset = info.eof;
            info.eof += sizeof(internalNode);
            leafNode leaf;
            info.head = info.tail = leaf.offset = info.eof;
            info.eof += sizeof(leafNode);
            root.num = 0;
            root.type = 1;
            root.parent = 0;
            //std::cout<<"leaf.offset is "<<leaf.offset<<std::endl;
            root.child[0] = leaf.offset;
            leaf.parent = root.offset;
            leaf.prev = leaf.next = 0;
            leaf.num = 0;
            write(&info,bpt_info_offset,1, sizeof(bpt_Info));
            write(&root,root.offset,1, sizeof(internalNode));
            write(&leaf,leaf.offset,1, sizeof(leafNode));
            //std::cout<<"root.offset: "<<root.offset<<std::endl;
            //std::cout<<"root.type is "<<root.type<<std::endl;
            //std::cout<<"leaf.offset: "<<leaf.offset<<std::endl;
            //read(&root,info.root,1, sizeof(internalNode));
            //std::cout<<"root.offset is "<<root.offset<<' '<<"root.child[0] is "<<root.child[0]<<std::endl;
            //internalNode tmp;
            //read(&tmp,info.root,1, sizeof(internalNode));
            //std::cout<<"read again root.offset is "<<tmp.offset<<" root.child[0] is "<<tmp.child[0]<<std::endl;
        }
        ///            0  1  2  3  4  5
        ///       key[   k1 k2 k3 k4 k5 .....]
        ///           ^  ^  ^  ^  ^  ^
        ///  child[  c1 c2 c3 c4 c5 c6 .....]
        ssize_t find_leaf(const Key &key,ssize_t offset){
            //std::cout<<"in find_leaf operation:\n";

            internalNode tmp;
            read(&tmp,offset,1, sizeof(internalNode));
            //std::cout<<"tmp.offset is "<<tmp.offset<<std::endl;
            //std::cout<<"tmp.type is "<<tmp.type<<std::endl;

            if(tmp.type == 1){
                int pos = 0;
                //std::cout<<"tmp.num is "<<tmp.num<<'\n';
                while(pos < tmp.num && key >= tmp.key[pos + 1])
                    pos++;
                //std::cout<<"pos is "<<pos<<std::endl;
                //std::cout<<"tmp.child[pos] is "<<tmp.child[pos]<<std::endl;
                return tmp.child[pos];
            }
            else{
                int pos = 0;
                while(pos < tmp.num && key >= tmp.key[pos + 1])
                    pos++;
                //if(pos == tmp.num + 1){
                //    std::cout<<"memory error!!!\n";
                //    return 0;
                //}

                return find_leaf(key,tmp.child[pos]);
            }
        }

        std::pair<iterator,OperationResult> insert_leaf(leafNode &leaf,const Key &key,const Value &value){
            //std::cout<<"in insert_leaf operation:\n";
            int pos = 0;
            //std::cout<<"leaf.num is "<<leaf.num<<'\n';
            while(pos < leaf.num && key > leaf.data[pos].first)
                pos++;
            for(int i = leaf.num - 1;i >= pos;i--){
                leaf.data[i + 1].first = leaf.data[i].first;
                leaf.data[i + 1].second = leaf.data[i].second;
                //leaf.data[i + 1] = leaf.data[i];
            }
            //std::cout<<"leaf.offset is "<<leaf.offset<<std::endl;
            //leaf.data[pos] = pair<Key,Value>(key,value);
            leaf.data[pos].first = key;
            leaf.data[pos].second = value;
            //std::cout<<" after fuzhi key is "<<leaf.data[pos].first<<" value is "<<leaf.data[pos].second<<std::endl;
            leaf.num++;
            //read(&info,bpt_info_offset,1, sizeof(bpt_Info));
            info.size++;
            iterator result(this,leaf.offset,pos);
            write(&info,bpt_info_offset,1, sizeof(bpt_Info));
            if(leaf.num <= L - 1){
                //std::cout<<"leaf.num <= L, write\n";
                //std::cout<<"leaf.offset is "<<leaf.offset<<'\n';
                write(&leaf,leaf.offset,1, sizeof(leafNode));
            }
            else{
                //std::cout<<"leaf.num is "<<leaf.num<<'\n';
                //std::cout<<"call split_leaf function'\n";
                /*if(pos < leaf.num / 2){

                }
                else{
                    result.pos -= leaf.num / 2;
                }*/
                split_leaf(leaf,result,key);
            }

            //std::cout<<"insert_leaf complete\n";
            //read(&leaf,leaf.offset,1, sizeof(leafNode));
            //std::cout<<"after insert key-value is "<<leaf.data[pos].first<<' '<<leaf.data[pos].second<<std::endl;
            return std::pair<iterator,OperationResult>(result,Success);
        }

        /*
        make_pair<iterator,OperationResult,bool,Key>
                insert_leaf_me(leafNode &leaf,const Key &key,const Value &value){
            int pos = 0;
            while(pos < leaf.num && key >= leaf.data[pos].first)
                pos++;
            for(int i = leaf.num - 1;i >= pos;i--){
                leaf.data[i + 1] = leaf.data[i];
            }
            leaf.data[pos] = pair<Key,Value>(key,value);
            leaf.num++;
            info.size++;
            iterator result(this,pos.leaf.offset);
            write(&info,bpt_info_offset,1, sizeof(bpt_Info));
            if(leaf.num <= L){
                write(&leaf,leaf.offset,1, sizeof(leafNode));
                return make_pair<iterator,OperationResult,bool,Key>
                        (result,Success,false,key);
            }
            else{
                result.place -= (leaf.num / 2);
                int tmp_key = leaf.data[leaf.num / 2].first;
                split_leaf(leaf,result);
                return make_pair<iterator,OperationResult,bool,Key>
                        (result,Success,true,tmp_key);
            }
        }
        */

        /**
         * @param internal : information of this node
         * @param key : the key should be inserted
         * @param child : the key-child-tree-node offset
         */
        void insert_node(internalNode &internal,const Key &key,const ssize_t child){
            int pos = 1;
            while(pos <= internal.num && key >= internal.key[pos])
                pos++;
            for(int i = internal.num;i >= pos;i--){
                internal.key[i + 1] = internal.key[i];
                internal.child[i + 1] = internal.child[i];
            }
            internal.key[pos] = key;
            internal.child[pos] = child;
            internal.num++;
            //std::cout<<"internal.num is "<<internal.num<<'\n';
            if(internal.num < M - 1){
                write(&internal,internal.offset,1, sizeof(internalNode));
            }
            else{
                //std::cout<<"M is "<<M<<'\n';
                //std::cout<<"node.num is "<<internal.num<<'\n';
                //std::cout<<"node should be split!\ncall split_node function\n";
                split_node(internal);
            }
        }

        void split_leaf(leafNode &leaf,iterator &it,const Key &key){
            leafNode another;
            another.num = leaf.num - (leaf.num / 2);
            leaf.num /= 2;

            //read(&info,bpt_info_offset,1, sizeof(bpt_Info));
            //std::cout<<"info.eof is "<<info.eof<<'\n';
            another.offset = info.eof;
            another.parent = leaf.parent;
            another.next = leaf.next;
            another.prev = leaf.offset;
            leaf.next = another.offset;

            info.eof += sizeof(leafNode);

            for(int i = 0;i < another.num;i++){
                another.data[i].first = leaf.data[i + leaf.num].first;
                another.data[i].second = leaf.data[i + leaf.num].second;
                if(another.data[i].first == key){
                    it.offset = another.offset;
                    it.pos = i;
                }
            }

            if(another.next == 0){
                info.tail = another.offset;
            }
            else{
                leafNode next_leaf;
                read(&next_leaf,another.next,1, sizeof(leafNode));
                next_leaf.prev = another.offset;
                write(&next_leaf,another.next,1, sizeof(leafNode));
            }

            write(&info,bpt_info_offset,1, sizeof(bpt_Info));
            write(&leaf,leaf.offset,1, sizeof(leafNode));
            write(&another,another.offset,1,sizeof(leafNode));

            internalNode parent;
            //std::cout<<"leaf.parent is "<<leaf.parent<<std::endl;
            read(&parent,leaf.parent,1, sizeof(internalNode));
            insert_node(parent,another.data[0].first,another.offset);
        }

        void split_node(internalNode &internal){
            //std::cout<<"call split_node function\n";
            //std::cout<<"split_node.num is "<<internal.num<<'\n';
            //read(&info,bpt_info_offset,1, sizeof(bpt_Info));
            //std::cout<<"in split_node function info.root is "<<info.root<<'\n';
            //std::cout<<"internal.offset is "<<internal.offset<<'\n';
            if(internal.offset != info.root) {
                //std::cout<<"split internalNode\n";
                //this node isn't root, perfect
                internalNode another;
                another.offset = info.eof;
                info.eof += sizeof(internalNode);

                another.num = internal.num - (internal.num / 2) - 1;
                internal.num /= 2;
                another.parent = internal.parent;
                another.type = internal.type;
                //std::cout<<"another.num is "<<another.num<<'\n';
                //std::cout<<"internal.num is "<<internal.num<<'\n';

                /**          0    1    2    3      4     5    6
                 *      key[     k1   k2   k3  |  k4  | k5   k6  ...]
                 *                             |______|
                 *          ^    ^    ^    ^   |   ^    ^    ^
                 *  child[ c1   c2   c3   c4   |  c5   c6   c7  ...]
                 *
                 * num is 6
                 */

                for (int i = 0;i <= another.num;i++){
                    another.child[i] = internal.child[i + 1 + internal.num];
                }
                for(int i = 1;i <= another.num;i++){
                    another.key[i] = internal.key[i + 1 + internal.num];
                }
                write(&internal,internal.offset,1, sizeof(internalNode));
                write(&another,another.offset,1,sizeof(internalNode));

                //change children's parent pointer
                internalNode tmp;
                leafNode leaf;
                for(int i = 0;i <= another.num;i++){
                    if(another.type == 1){
                        //if its child is leaf
                        read(&leaf,another.child[i],1, sizeof(leafNode));
                        leaf.parent = another.offset;
                        write(&leaf,another.child[i],1, sizeof(leafNode));
                    }
                    else{
                        //if its child isn't leaf
                        read(&tmp,another.child[i],1, sizeof(internalNode));
                        tmp.parent = another.offset;
                        write(&tmp,another.child[i],1, sizeof(internalNode));
                    }
                }

                //insert key and children pointer in parent node
                read(&tmp,internal.parent,1, sizeof(internalNode));
                insert_node(tmp,internal.key[internal.num + 1],another.offset);
            }
            else{
                //std::cout<<"split root\n";
                //this node is root, so terrible...

                /**solution_2
                 * one new node to store the half splited-root,
                 * the other new node is the new root
                 */
                //internalNode root;
                //read(&root,info.root,1,sizeof(internalNode));
                internalNode new_root;
                internalNode half;
                new_root.offset = info.eof;
                info.eof += sizeof(internalNode);
                half.offset = info.eof;
                info.eof += sizeof(internalNode);
                info.root = new_root.offset;
                internal.parent = info.root;
                half.parent = info.root;
                half.num = internal.num - internal.num / 2 - 1;
                for(int i = 1;i <= half.num;i++){
                    half.key[i] = internal.key[i + internal.num / 2 + 1];
                }
                for(int i = 0;i <= half.num;i++){
                    half.child[i] = internal.child[i + internal.num / 2 + 1];
                }



                new_root.num = 1;
                new_root.parent = 0;
                new_root.type = 0;
                half.type = internal.type;
                new_root.child[0] = internal.offset;
                new_root.child[1] = half.offset;
                new_root.key[1] = internal.key[internal.num / 2 + 1];
                internal.num /= 2;

                if(internal.type){
                //    std::cout<<"root's child is leaf\n";
                    leafNode change;

                    for(int i = 0;i <= half.num;i++){
                        read(&change,internal.child[i + internal.num + 1],1, sizeof(leafNode));
                        change.parent = half.offset;
                        write(&change,internal.child[i + internal.num + 1],1, sizeof(leafNode));
                    }
                }
                else{
                    internalNode change;
                    for(int i = 0;i <= half.num;i++){
                        read(&change,internal.child[i + internal.num + 1],1, sizeof(internalNode));
                        change.parent = half.offset;
                        write(&change,internal.child[i + internal.num + 1],1, sizeof(internalNode));
                    }
                }

                write(&info,bpt_info_offset,1, sizeof(bpt_Info));
                write(&internal,internal.offset,1, sizeof(internalNode));
                write(&new_root,new_root.offset,1, sizeof(internalNode));
                write(&half,half.offset,1, sizeof(internalNode));

                return;
            }
        }

    public:

        class const_iterator;
        class iterator {
            friend class BTree;
        private:
            BTree* tree;
            ssize_t offset;
            int pos;
        public:
            bool modify(const Value& value){
                leafNode tmp;
                tree->read(&tmp,offset,1, sizeof(leafNode));
                tmp.data[pos].second = value;
                tree->write(&tmp,offset,1, sizeof(leafNode));
                return true;
            }
            iterator(BTree*t = NULL,ssize_t off = 0,int p = 0) : tree(t),offset(off),pos(p){}
            iterator(const iterator& other) {
                tree = other.tree;
                offset = other.offset;
                pos = other.pos;
            }
            // Return a new iterator which points to the n-next elements
            iterator operator++(int) {
                iterator pre = *this;
                if(pre == tree->end()){
                    tree = NULL;
                    offset = 0;
                    pos = 0;
                    return pre;
                }


                leafNode tmp;
                tree->read(&tmp,offset,1,sizeof(leafNode));

                if(pos == tmp.num - 1){
                    if(this->offset == this->tree->info.tail){
                        pos++;
                        return pre;
                    }
                    offset = tmp.next;
                    pos = 0;
                }
                else{
                    pos++;
                }
                return pre;
            }
            iterator& operator++() {
                if(*this == tree->end()){
                    return iterator();
                }

                leafNode tmp;
                tree->read(&tmp,offset,1,sizeof(leafNode));
                if(pos == tmp.num - 1){
                    if(offset == tree->info.tail){
                        pos++;
                        return *this;
                    }
                    offset = tmp.next;
                    pos = 0;
                }
                else{
                    pos++;
                }
                return *this;
            }
            iterator operator--(int) {
                iterator pre = *this;
                if(pre == tree->begin()){
                    tree = NULL;
                    offset = 0;
                    pos = 0;
                    return pre;
                }

                leafNode tmp;
                tree->read(&tmp,offset,1, sizeof(leafNode));

                if(pos == 0){
                    offset = tmp.prev;
                    tree->read(&tmp,tmp.prev,1, sizeof(leafNode));
                    pos =  tmp.num - 1;
                }
                else{
                    pos--;
                }

                return pre;
            }
            iterator& operator--() {
                if(*this == tree->begin()){
                    tree = NULL;
                    offset = 0;
                    pos = 0;
                    return *this;
                }

                leafNode tmp;
                tree->read(&tmp,offset,1, sizeof(leafNode));

                if(pos == 0){
                    offset = tmp.prev;
                    tree->read(&tmp,tmp.prev,1, sizeof(leafNode));
                    pos =  tmp.num - 1;
                }
                else{
                    pos--;
                }
                return *this;
            }
            // Overloaded of operator '==' and '!='
            // Check whether the iterators are same
            bool operator==(const iterator& rhs) const {
                return tree == rhs.tree && offset == rhs.offset
                       && pos == rhs.pos;
            }
            bool operator==(const const_iterator& rhs) const {
                return tree == rhs.tree && offset == rhs.offset
                       && pos == rhs.pos;
            }
            bool operator!=(const iterator& rhs) const {
                return tree != rhs.tree || offset != rhs.offset
                       || pos != rhs.pos;
            }
            bool operator!=(const const_iterator& rhs) const {
                return tree != rhs.tree || offset != rhs.offset
                       || pos != rhs.pos;
            }
            Value getValue(){
                leafNode leaf;
                tree->read(&leaf,offset,1, sizeof(leafNode));
                return leaf.data[pos].second;
            }
        };
        class const_iterator {
            friend class BTree;
        private:
            BTree* tree;
            ssize_t offset;
            int pos;
        public:
            const_iterator(BTree*t = NULL,ssize_t off = 0,int p = 0) : BTree(t),offset(off),pos(p){}
            const_iterator(const const_iterator& other) {
                tree = other.tree;
                offset = other.offset;
                pos = other.pos;
            }
            const_iterator(const iterator& other) {
                tree = other.tree;
                offset = other.offset;
                pos = other.pos;
            }
            const_iterator operator++(int) {
                const_iterator pre = *this;
                if(pre == tree->end()){
                    tree = NULL;
                    offset = 0;
                    pos = 0;
                    return pre;
                }

                leafNode tmp;
                tree->read(&tmp,offset,1,sizeof(leafNode));
                if(pos == tmp.num - 1){
                    if(offset == tree->info.tail){
                        pos++;
                        return pre;
                    }
                    offset = tmp.next;
                    pos = 0;
                }
                else{
                    pos++;
                }
                return pre;
            }
            const_iterator& operator++() {
                if(*this == tree->end()){
                    return iterator();
                }

                leafNode tmp;
                tree->read(&tmp,offset,1,sizeof(leafNode));
                if(pos == tmp.num - 1){
                    if(offset == tree->info.tail){
                        pos++;
                        return *this;
                    }
                    offset = tmp.next;
                    pos = 0;
                }
                else{
                    pos++;
                }
                return *this;
            }
            const_iterator operator--(int) {
                const_iterator pre = *this;
                if(pre == tree->begin()){
                    tree = NULL;
                    offset = 0;
                    pos = 0;
                    return pre;
                }

                leafNode tmp;
                tree->read(&tmp,offset,1, sizeof(leafNode));

                if(pos == 0){
                    offset = tmp.prev;
                    tree->read(&tmp,tmp.prev,1, sizeof(leafNode));
                    pos =  tmp.num - 1;
                }
                else{
                    pos--;
                }

                return pre;
            }
            const_iterator& operator--() {
                if(*this == tree->begin()){
                    tree = NULL;
                    offset = 0;
                    pos = 0;
                    return *this;
                }

                leafNode tmp;
                tree->read(&tmp,offset,1, sizeof(leafNode));

                if(pos == 0){
                    offset = tmp.prev;
                    tree->read(&tmp,tmp.prev,1, sizeof(leafNode));
                    pos =  tmp.num - 1;
                }
                else{
                    pos--;
                }

                return *this;
            }
            // Overloaded of operator '==' and '!='
            // Check whether the iterators are same
            bool operator==(const iterator& rhs) const {
                return tree == rhs.tree && offset == rhs.offset
                       && pos == rhs.pos;
            }
            bool operator==(const const_iterator& rhs) const {
                return tree == rhs.tree && offset == rhs.offset
                       && pos == rhs.pos;
            }
            bool operator!=(const iterator& rhs) const {
                return tree != rhs.tree || offset != rhs.offset
                       || pos != rhs.pos;
            }
            bool operator!=(const const_iterator& rhs) const {
                return tree != rhs.tree || offset != rhs.offset
                       || pos != rhs.pos;
            }
        };
        // Default Constructor and Copy Constructor
        BTree() {
            fp_name.nameName(ID);
            fp = nullptr;
            fp_open = false;
            //std::cout<<M<<'\t'<<L<<std::endl;
            //std::cout<<"constructing\n";
            open();
            init();
            //internalNode root;
            //read(&root,info.root,1, sizeof(internalNode));
            //std::cout<<"root.type_after is "<<root.type<<std::endl;
        }
        BTree(const BTree& other) {
            // Todo Copy
        }
        BTree& operator=(const BTree& other) {
            // Todo Assignment
        }
        ~BTree() {
            close();
        }
        // Insert: Insert certain Key-Value into the database
        // Return a pair, the first of the pair is the iterator point to the new
        // element, the second of the pair is Success if it is successfully inserted
        pair<iterator, OperationResult> insert(const Key& key, const Value& value) {
            //std::cout<<"in insert operation:\n";
            open();
            //std::cout<<"call find_leaf operation:\n";
            //std::cout<<"key is "<<key<<"info.root is "<<info.root<<std::endl;
            //read(&info,bpt_info_offset,1, sizeof(bpt_Info));
            ssize_t offset = find_leaf(key,info.root);
            //std::cout<<"after find_leaf the offset is "<<offset<<std::endl;
            leafNode leaf;
            read(&leaf,offset,1, sizeof(leafNode));
            //std::cout<<"leaf.num is "<<leaf.num<<'\n';
            //std::cout<<"after read the leaf.offset is "<<leaf.offset<<std::endl;

            //if(leaf.num >= L)
            //std::cout<<"leaf.num is "<<leaf.num<<'\n';
            //std::cout<<"call insert_leaf function\n";
            std::pair<iterator,OperationResult> tmp;
            tmp = insert_leaf(leaf,key,value);

            pair<iterator,OperationResult> result;
            result.first = tmp.first;
            result.second = tmp.second;

            //std::cout<<"insert complete\n";
            //close();
            return result;
        }
        // Erase: Erase the Key-Value
        // Return Success if it is successfully erased
        // Return Fail if the key doesn't exist in the database
        OperationResult erase(const Key& key) {
            // TODO erase function
            return Fail;  // If you can't finish erase part, just remaining here.
        }
        // Return a iterator to the beginning
        iterator begin() {
            iterator tmp(this,info.head,0);
            return tmp;
        }
        const_iterator cbegin() const {
            const_iterator tmp(this,info.head,0);
            return tmp;
        }
        // Return a iterator to the end(the next element after the last)
        iterator end() {
            leafNode tail;
            read(&tail,info.tail,1, sizeof(leafNode));
            iterator tmp(this,tail.offset,tail.num);
            return tmp;
        }
        const_iterator cend() const {
            leafNode tail;
            read(&tail,info.tail,1, sizeof(leafNode));
            const_iterator tmp(this,tail.offset,tail.num);
            return tmp;
        }
        // Check whether this BTree is empty
        bool empty() const {
            return info.size == 0;
        }
        // Return the number of <K,V> pairs
        size_t size() const {
            return info.size;
        }
        // Clear the BTree
        void clear() {
            fp = fopen(fp_name.str,"w");
            fclose(fp);
            init();
        }
        // Return the value refer to the Key(key)
        Value at(const Key& key){
            iterator pt = find(key);
            if(pt.tree != NULL){
                leafNode leaf;
                read(&leaf,pt.offset,1, sizeof(leafNode));
                int pos = pt.pos;
                return leaf.data[pos].second;
            }
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key& key){
            return find(key) != end();
        }
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is
         * returned.
         */
        iterator find(const Key& key) {
            //std::cout<<"in find operation:\n";

            //internalNode root;
            //std::cout<<"info.root is "<<info.root<<std::endl;
            //read(&root,info.root,1, sizeof(internalNode));
            //std::cout<<"root:\n";
            //std::cout<<"root.offset "<<root.offset<<'\t'<<"root.child[0] is "<<root.child[0]<<std::endl;
            //std::cout<<"key is "<<key<<" info.root is "<<info.root<<std::endl;

            //bpt_info_offset,1, sizeof(bpt_Info));
            //std::cout<<"after read, info.root is "<<info.root<<'\n';
            ssize_t offset = find_leaf(key,info.root);
            //std::cout<<"find_leaf.offset is "<<offset;
            leafNode leaf;
            read(&leaf,offset,1, sizeof(leafNode));
            for(int i = 0;i < leaf.num;i++){
                if(leaf.data[i].first == key){
                    //std::cout<<"i is "<<i<<" key is "<<leaf.data[i].first<<" value is "<<leaf.data[i].second<<std::endl;
                    return iterator(this,offset,i);
                }

            }
            return end();
        }
        const_iterator find(const Key& key) const {
            //read(&info,bpt_info_offset,1, sizeof(bpt_Info));
            ssize_t offset = find_leaf(key,info.root);
            leafNode leaf;
            read(&leaf,offset,1, sizeof(leafNode));
            for(int i = 0;i < leaf.num;i++){
                if(leaf.data[i].first == key){
                    return const_iterator(this,offset,i);
                }

            }
            return cend();
        }

    public:
        void testmyself(){
            /*leafNode leaf;
            read(&leaf,info.head,1, sizeof(leafNode));
            while(leaf.offset != info.tail){
                for(int i = 0;i < leaf.num;i++){
                    std::cout<<leaf.data[i].second<<'\n';
                }
                read(&leaf,leaf.next,1, sizeof(leafNode));
            }*/
            int num = 0;
            iterator it = this->begin();
            while(it != this->end()){
                 std::cout<<num<<'\n';
                 num++;
                 it++;
            }

            std::cout<<"OK!";

            /*read(&leaf,info.tail,1, sizeof(leafNode));
            while(leaf.offset != info.head){
                for(int i = leaf.num;i >= 0;i--){
                    std::cout<<leaf.data[i].second<<'\n';
                }
                read(&leaf,leaf.prev,1, sizeof(leafNode));
            }*/

            /*internalNode tmp;
            read(&tmp,info.root,1, sizeof(internalNode));
            while(!tmp.type){
                std::cout<<"key[1] is "<<tmp.key[1]<<'\n';
                read(&tmp,tmp.child[0],1, sizeof(internalNode));
            }
            read(&leaf,tmp.child[0],1, sizeof(leafNode));
            for(int i = 0;i < leaf.num;i++){
                std::cout<<leaf.data[i].second<<'\n';
            }*/
        }
    };

}  // namespace sjtu
