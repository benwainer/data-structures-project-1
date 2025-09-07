#ifndef AVL_H
#define AVL_H
#include <cstdlib>
#include <string>
#include <iostream>
#include <memory>

namespace ds {
    template<typename T>
    class Avl {
    private:
        template<typename S>
        class Node {
        private:
            S data; //owner
            Node<S> *father;
            Node<S> *right;
            Node<S> *left;
            int height;

            Node<S>() : father(NULL), right(NULL), left(NULL), height(0) {}
            explicit Node<S>(const S& data) : data(data), father(NULL), right(NULL), left(NULL),
                                              height(0) {}
            //const S& getData() const { return data; }
            static void swapNodeNextInOrder(Node<S>* first, Node<S>* second) {
                Node<S> *tmp = first;
                Node<S> *tmp_father = first->father;
                Node<S> *tmp_first_right_son = first->right;
                Node<S> *tmp_first_left_son = first->left;
                Node<S> *tmp_second_father = second->father;
                Node<S> *tmp_second_right_son = second->right;
                int first_height = first->height;
                int second_height = second->height;

                first = second;
                first->height = first_height;
                first->right = tmp_first_right_son;
                tmp_first_right_son->father = first;
                first->left = tmp_first_left_son;
                tmp_first_left_son->father = first;
                first->father = tmp_father;
                if (tmp_father != NULL && *tmp_father > *first) {
                    tmp_father->left = first;
                } else if (tmp_father != NULL && *tmp_father < *first)
                    tmp_father->right = first;

                second = tmp;
                second->height = second_height;
                second->right = tmp_second_right_son;
                second->left = NULL;
                if (tmp_second_right_son != NULL)
                    tmp_second_right_son->father = second;
                second->father = tmp_second_father;
                tmp_second_father->left = second;
            }

            static Node<T>* neighborSwapNodeNextInOrder(Node<S>* first, Node<S>* second)
            {
                Node<S>* tmp = first;
                Node<S>* tmp_father = first->father;
                Node<S>* tmp_first_left_son = first->left;

                first = second;
                first->left = tmp_first_left_son;
                tmp_first_left_son->father = first;
                first->father = tmp_father;
                if (tmp_father != NULL && *(tmp_father) > *first) {
                    tmp_father->left = first;
                }
                else if (tmp_father != NULL && *(tmp_father) < *first)
                    tmp_father->right = first;
                tmp->right = NULL;
                tmp->left = NULL;
                tmp_father = NULL;
                return tmp;
            }
        public:
            Node<S>(const Node<S> &other) = delete;

            virtual ~Node<S>(){}

            Node<S> &operator=(const Node<S> &other) = delete;

            bool operator<(const Node<S> &other)
            {
                return data < other.data;
            }
            bool operator>(const Node<S> &other)
            {
                return data > other.data;
            }
            bool operator==(const Node<S> &other)
            {
                if(other == NULL){
                    return this == NULL;
                }
                return data == other.data;
            }

            Node<T>* getMin(Node<T>* node)
            {
                if (node == NULL)
                    return NULL;
                Node<T> *min = node;
                while (min->left != NULL)
                    min = min->left;
                return min;
            }

            Node<T>* removeNode(const T& received_data, Node<T>** root) {
                Node<T>* to_return = NULL;
                if (received_data < data && left != NULL)
                    to_return = left->removeNode(received_data,root);
                else if (received_data > data && right != NULL)
                    to_return = right->removeNode(received_data,root);
                else {
                    Node<T> *to_replace = NULL;
                    if (left == NULL && right == NULL) { //has no sons
                        if (father != NULL && *father > *this)
                            father->left = NULL;
                        else if (father != NULL && !(*father > *this))
                            father->right = NULL;
                        to_return = this;
                        if (to_return == *root)
                            *root = NULL;
                    }
                    else if (left == NULL || right == NULL) { //has one son
                        if (left == NULL) {
                            if(father != NULL){
                                if (father->data > data)
                                    father->left = right;
                                else
                                    father->right = right;
                            }
                            right->father = father;
                            if (this == *root )
                                *root = right;
                        } else {
                            if(father != NULL) {
                                if (father->data > data)
                                    father->left = left;
                                else
                                    father->right = left;
                            }
                            left->father = father;
                            if (this == *root)
                                *root = left;
                        }
                        to_return = this;
                    } else { //has two sons
                        to_replace = right->getMin(right);
                        if (this == *root)
                            *root = to_replace;
                        if (to_replace == this->right) {
                            to_return = neighborSwapNodeNextInOrder(this, to_replace);
                        }
                        else {
                            if (!(to_replace == this))
                                swapNodeNextInOrder(this, to_replace);
                            to_return = to_replace->right->removeNode(data, root);
                        }
                    }
                }
                if (*root) {
                    (*root)->height = 1 + std::max(Avl<T>::height((*root)->right), Avl<T>::height((*root)->left));
                    (*root)->doBalance(root);
                }
                else {
                    height = 1 + std::max(Avl<T>::height(right), Avl<T>::height(left));
                    doBalance(root);
                }

                return to_return;
            }

            void ll_roll(Node<T>** root)
            {
                Node<T> *temp = left;
                left = temp->right;
                if (temp->right != NULL)
                    temp->right->father = this;
                temp->right = this;
                temp->father = father;
                if (father != NULL) {
                    if (father->data < temp->data)
                        father->right = temp;
                    else {
                        father->left = temp;
                    }
                }
                father = temp;
                height = 1+ std::max(Avl<T>::height(right), Avl<T>::height(left));
                temp->height = 1+ std::max(Avl<T>::height(temp->right), Avl<T>::height(temp->left));
                if (this == *root){
                    *root = temp;
                }
            }

            void rr_roll(Node<T>** root) {
                Node<T> *temp = right;
                right = temp->left;
                if (temp->left != NULL)
                    temp->left->father = this;
                temp->left = this;
                temp->father = father;
                if (father != NULL) {
                    if (father->data < temp->data)
                        father->right = temp;
                    else {
                        father->left = temp;
                    }
                }
                father = temp;
                height = 1+ std::max(Avl<T>::height(right), Avl<T>::height(left));
                temp->height = 1+ std::max(Avl<T>::height(temp->right), Avl<T>::height(temp->left));
                if (this == *root){
                    *root = temp;
                }
            }

            void lr_roll(Node<T>** root) {
                Node<T> *temp = left;
                temp->rr_roll(root);
                ll_roll(root);
            }

            void rl_roll(Node<T>** root) {
                Node<T> *temp = right;
                temp->ll_roll(root);
                rr_roll(root);
            }

            void doBalance(Node<T> **root) {
                int dif = difference();
                if (dif == 2) {
                    if (left->difference() >= 0) {
                        ll_roll(root);
                    } else {
                        lr_roll(root);
                    }
                }
                if (dif == -2) {
                    if (right->difference() <= 0) {
                        rr_roll(root);
                    } else {
                        rl_roll(root);
                    }
                }
            }

            int difference() {
                if (!left && !right)
                    return 0;
                else if (!left)
                    return -1 - right->height;
                else if (!right)
                    return 1+ left->height;
                return left->height - right->height;
            }

            friend class Avl;
        };


        Node<T> *root;

        void insertNode(const T& data, Node<T>* input_root, Node<T>* intended_father, Node<T>** real_root);
        Node<T> *findNode(const T& data, Node<T> *node);
        static int height(const Node<T>* node);
        Node<T> *getMax(Node<T> *node);
        Node<T> *findCloserFromTopNode(const T& data, Node<T>* node, Node<T>* last_bigger);
        Node<T> *findCloserFromButtomNode(const T& data, Node<T>* node, Node<T>* last_smaller);
        Node<T> *findNextInOrderNode(const T& data, Node<T>* node, Node<T>* last_bigger);
        Node<T> *findPrevInOrderNode(const T& data, Node<T>* node, Node<T>* last_bigger);

    public:
        Avl() : root(NULL) {}
        Avl(const Avl &other);
        ~Avl();
        Avl &operator=(const Avl &other) = delete;
        void insert(const T& data);
        void remove(const T& data);
        T* getMaxData();
        T* getFatherData(const T& data);
        T* getLeftData(const T& data);
        T* getRightData(const T& data);
        T* find(const T& data);
        void postOrderClear(Node<T> *node);
        Node<T>* getRoot() const;
        void createEmptyTree(int *final_tree_size, int h, int lower_full_tree_size);
        void recursiveCreateEmptyTree(int* size, int h, int* dif, Node<T>* node);
        void postOrderUpdateHeight(Node<T>* node);
        void recursivePutValuesInOrder(T* array, Node<T>* node, int arr_len, int* index);
        void avlInsertToArrayInOrder(T* array, int n, int* index,Node<T>* node);
        void insertToArrayInReverseOrder(T* array, int n, int *index, Node<T>* node);
        Node<T>* findCloserFromTop(const T& data);
        Node<T>* findCloserFromButtom(const T& data);
        T* findNextInOrder(const T& data);
        T* findPrevInOrder(const T& data);
        Node<T> *findHighestInBoundsNode(const T& lower,const T& higher, Node<T> *current_node);
        void makeArrayInBounds(T* array,int* index, int arr_len, Node<T>* root,T& lower,T& higher);
        class Exception : public std::exception {};
        class DoesNotExist : public Exception {};
        class AlreadyExists : public Exception {};
    };

    template<class T>
    Avl<T>::~Avl() {
        postOrderClear(root);
        root = NULL;
    }

    template<class T>
    typename Avl<T>:: template Node<T>* Avl<T>::getRoot() const
    {
        return root;
    }

    template<class T>
    void Avl<T>::insert(const T& data)
    {
        if (findNode(data, root) != NULL)
            throw AlreadyExists();
        if (root == NULL) {
            root = new Node<T>(data);
            return;
        }
        if ((root->left == NULL) && data < root->data) {
            root->left = new Node<T>(data);
            root->left->father = root;
            root->height = 1;
        }
        else if ((root->right == NULL) && data > root->data) {
            root->right = new Node<T>(data);
            root->right->father = root;
            root->height = 1;
        }
        else
            insertNode(data, root, NULL, &root);
    }

    template<class T>
    void Avl<T>::insertNode(const T& data, Node<T>* input_root, Node<T>* intended_father, Node<T>** real_root
    )
    {
        if (input_root == NULL) {
            Node<T> *node = new Node<T>(data);
            if (intended_father->data > data) {
                intended_father->left = node;
            }
            else {
                intended_father->right = node;
            }
            node->father = intended_father;
            return;
        }
        if (data < input_root->data) {
            insertNode(data, input_root->left, input_root, real_root);
            input_root->height = (input_root->right == NULL) ? 1 + height(input_root->left) :
                                 1 + std::max(height(input_root->left), height(input_root->right));
            input_root->doBalance(real_root);
        } else {
            insertNode(data, input_root->right, input_root, real_root);
            input_root->height = (input_root->left == NULL) ? 1 + height(input_root->right) :
                                 1 + std::max(height(input_root->left), height(input_root->right));
            input_root->doBalance(real_root);
        }
    }

    template<class T>
    void Avl<T>::remove(const T& data)
    {
        Node<T>* found = findNode(data, root);
        if (found == NULL) {
            throw DoesNotExist();
        }
        else{
            Node<T>* to_remove = root->removeNode(data, &root);
            delete to_remove;
        }
    }

//    template<class T>
//    typename Avl<T>::template Node<T> *Avl<T>::getMin(Node<T> *node)
//    {
//        if (node == NULL)
//            return NULL;
//        Node<T> *min = node;
//        while (min->left != NULL)
//            min = min->left;
//        return min;
//    }

    template<class T>
    T* Avl<T>::getMaxData()
    {
        Node<T>* get_max = getMax(root);
        if(root == NULL){
            return NULL;
        }
        return &(get_max->data);
    }
    template<class T>
    typename Avl<T>::template Node<T> *Avl<T>::getMax(Node<T> *node)
    {
        if (node == NULL)
            return NULL;
        Node<T> *max = node;
        while (max->right != NULL)
            max = max->right;
        return max;
    }
    template<class T>
    T* Avl<T>::getFatherData(const T& data)
    {
        Node<T>* found = findNode(data, root);
        if (found == NULL){
            throw DoesNotExist();
        }
        else if(found->father == NULL){
            return NULL;
        }
        else
            return &(found->father->data);
    }
    template<class T>
    T* Avl<T>::getLeftData(const T& data)
    {
        Node<T>* found = findNode(data, root);
        if (found == NULL){
            throw DoesNotExist();
        }
        else if(found->left == NULL){
            return NULL;
        }
        else
            return &(found->left->data);
    }
    template<class T>
    T* Avl<T>::getRightData(const T& data)
    {
        Node<T>* found = findNode(data, root);
        if (found == NULL){
            throw DoesNotExist();
        }
        else if(found->rightt == NULL){
            return NULL;
        }
        else
            return &(found->rightt->data);
    }
    template<class T>
    T* Avl<T>::find(const T& data)
    {
        Node<T>* found = findNode(data, root);
        if (found == NULL){
            throw DoesNotExist();
        }
        else
            return &(found->data);
    }
    template<class T>
    typename Avl<T>::template Node<T> *Avl<T>::findNode(const T& data, Node<T> *node) {
        if (node == NULL)
            return NULL;
        if(data == node->data)
            return node;
        if (data < node->data)
            return findNode(data, node->left);
        else /*(data > node->data)*/
            return findNode(data, node->right);
    }

    template<class T>
    int Avl<T>::height(const Node<T> *node) {
        if (node == NULL)
            return -1;
        return node->height;
    }

    template<class T>
    typename Avl<T>::template Node<T> *Avl<T>::findHighestInBoundsNode(const T& lower,const T& higher, Node<T> *current_node)
    {
        if (current_node!= NULL && (current_node->data < lower || current_node->data > higher)){
            if (current_node->data < lower)
                return findHighestInBoundsNode(lower,higher,current_node->right);
            else
                return findHighestInBoundsNode(lower,higher,current_node->left);
        }
        return current_node;
    }
    template<class T>
    typename Avl<T>::template Node<T> *Avl<T>::findCloserFromTop(const T& data)
    {
        return findCloserFromTopNode(data,root, NULL);
    }
    template<class T>
    typename Avl<T>::template Node<T> *Avl<T>::findCloserFromButtom(const T& data)
    {
        return findCloserFromButtomNode(data,root, NULL);
    }
    template<class T>
    T* Avl<T>::findNextInOrder(const T &data)
    {
        Node<T>* found = findNextInOrderNode(data,root, NULL);
        if(found != NULL){
            return &(found->data);
        }
        else
            return NULL;
    }
    template<class T>
    T* Avl<T>::findPrevInOrder(const T& data)
    {
        Node<T>* found = findPrevInOrderNode(data,root, NULL);
        if(found != NULL){
            return &(found->data);
        }
        else
            return NULL;
    }
    template<class T>
    typename Avl<T>::template Node<T> *Avl<T>::findCloserFromTopNode(const T& data, Node<T>* node, Node<T>* last_bigger)
    {
        if (node == NULL)
            return last_bigger;
        if (data < node->data)
            return findCloserFromTopNode(data, node->left, node);
        else if(data > node->data)
            return findCloserFromTopNode(data, node->right, last_bigger);
        else
            return node;
    }
    template<class T>
    typename Avl<T>::template Node<T> *Avl<T>::findNextInOrderNode(const T& data, Node<T>* node, Node<T>* last_bigger)
    {
        if (node == NULL)
            return last_bigger;
        if (data < node->data)
            return findNextInOrderNode(data, node->left, node);
        else if(!(data < node->data))
            return findNextInOrderNode(data, node->right, last_bigger);
        else
            return node;
    }
    template<class T>
    typename Avl<T>::template Node<T> *Avl<T>::findCloserFromButtomNode(const T& data, Node<T>* node, Node<T>* last_smaller)
    {
        if (node == NULL)
            return last_smaller;
        if (data < node->data)
            return findCloserFromButtomNode(data, node->left, last_smaller);
        else if(data > node->data)
            return findCloserFromButtomNode(data, node->right, node);
        else
            return node;
    }
    template<class T>
    typename Avl<T>::template Node<T> *Avl<T>::findPrevInOrderNode(const T& data, Node<T>* node, Node<T>* last_smaller)
    {
        if (node == NULL)
            return last_smaller;
        if (!(data > node->data))
            return findPrevInOrderNode(data, node->left, last_smaller);
        else if(data > node->data)
            return findPrevInOrderNode(data, node->right, node);
        else
            return node;
    }
    template<class T>
    void Avl<T>::postOrderClear(Node<T> *node)
    {
        if (node == NULL) {
            return;
        }
        postOrderClear(node->left);
        postOrderClear(node->right);
        node->right = NULL;
        node->left = NULL;
        node->father = NULL;
        delete node;
    }

    template<class T>
    void Avl<T>::createEmptyTree(int *final_tree_size, int h, int lower_full_tree_size)
    {
        //check difference between full tree and our future tree
        int dif = *final_tree_size - lower_full_tree_size;

        //clean original tree
        postOrderClear(root);

        //create new root for the tree
        Node<T>* node = new Node<T>;
        root = node;
        root->height = h;
        (*final_tree_size)--;
        //fill the tree with empty nodes recursively - the tree won't be a full tree but a quite-full tree
        recursiveCreateEmptyTree(final_tree_size,h-1, &dif, root);
        postOrderUpdateHeight(root);
    }


    template<class T>
    void Avl<T>::postOrderUpdateHeight(Node<T>* node)
    {
        if (node == NULL) {
            return;
        }
        postOrderUpdateHeight(node->left);
        postOrderUpdateHeight(node->right);
        node->height = 1+ std::max(Avl<T>::height(node->right), Avl<T>::height(node->left));
    }
    template<class T>
    void Avl<T>::recursiveCreateEmptyTree(int* size, int h, int* dif, Node<T>* node) {
        if (*size < 0 || h <= -1)
            return;
        if (h == 0) {
            if ((*dif) > 0)
                (*dif)--;
            else
                return;
        }
        node->left = new Node<T>;
        //node->height = h; // todo:: return later
        (*size)--;
        node->left->father = node;
        recursiveCreateEmptyTree(size, h - 1, dif, node->left);
        if (h == 0) {
            if ((*dif) > 0)
                (*dif)--;
            else
                return;
        }
        if (*size > 0){
            node->right = new Node<T>;
            //node->height = h; // todo:: return later
            (*size)--;
            node->right->father = node;
            recursiveCreateEmptyTree(size, h - 1, dif, node->right);
        }
    }

    template<class T>
    void Avl<T>::recursivePutValuesInOrder(T* array, Node<T>* node, int arr_len, int* index)
    {
        if (node == NULL || *index >= arr_len)
            return;
        recursivePutValuesInOrder(array, node->left, arr_len, index);
        node->data = array[*index];
        (*index)++;
        recursivePutValuesInOrder(array, node->right, arr_len, index);
    }
    template<class T>
    void Avl<T>::avlInsertToArrayInOrder(T* array, int n, int* index, Node<T>* node)
    {
        if (node == NULL || *index >= n) {
            return;
        }
        avlInsertToArrayInOrder(array, n, index,node->left);
        if (*index >= n)
            return;
        array[*index] = node->data;
        (*index)++;
        avlInsertToArrayInOrder(array, n, index, node->right);
    }
    template<class T>
    void Avl<T>::insertToArrayInReverseOrder(T* array, int n, int *index, Node<T>* node)
    {
        if (node == NULL || *index >= n) {
            return;
        }
        insertToArrayInReverseOrder(array, n, index,node->right);
        if (*index >= n)
            return;
        array[*index] = node->data;
        (*index)++;
        insertToArrayInReverseOrder(array, n, index, node->left);
    }

    template<class T>
    void Avl<T>:: makeArrayInBounds(T* array,int* index, int arr_len, Node<T>* root,T& lower,T& higher)
    {
        Node<T>* common_father = findHighestInBoundsNode(lower, higher, root);
        if(common_father == NULL){
            return;
        }
        Node<T>* lowest = findCloserFromTop(lower);
        Node<T>* highest = findCloserFromButtom(higher);
        Node<T>* current_highest = common_father->right;
        Node<T>* current_lowest = lowest;
        while(current_lowest!=common_father)
        {
            if(current_lowest->data<lower)
            {
                current_lowest=current_lowest->father;
                continue;
            }
            array[*index] = current_lowest->data;
            (*index)++;
            avlInsertToArrayInOrder(array,arr_len,index,current_lowest->right);
            current_lowest=current_lowest->father;
        }
        array[*index] = common_father->data;
        (*index)++;
        while(current_highest!= nullptr)
        {
            if(current_highest->data>higher)
            {
                current_highest=current_highest->left;
                continue;
            }
            avlInsertToArrayInOrder(array,arr_len,index,current_highest->left);
            array[*index] = current_highest->data;
            (*index)++;
            current_highest=current_highest->right;
        }






    }
}
#endif