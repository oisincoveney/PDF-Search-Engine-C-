#pragma once

#include <AVLTree/node.h>
#include <algorithm>

template<class T>
/**
 * The AvlTree class contains the functionality for a self-balancing binary
 * tree, based on the premise of removing imbalance through rotations
 * which makes AVL trees unique. The templated class contains a node,
 * which serves as the head of the tree, and will rotate if there
 * are any imbalances after the insertion of elements.
 *
 * Made with the help of Prof. Fontenot's AVL Tree handout by Nate Weiss
 *
 * By: Oisin Coveney
 */
class AvlTree
{
        friend class HashTable;
        friend class AVLIndex;

    public:

        Node<T>* head;
        int numElements;

        T& insert(T& data, Node<T>*& node);
        Node<T>* copy(Node<T>* other);

        //functions needed for rotation
        void rotate(Node<T>*& node);
        void rightRot(Node<T>*& a1);
        void leftRot(Node<T>*& a1);

        //Calculations for rotation
        int getHeight(Node<T>*& node);
        int getBalance(Node<T>*& node);


    public:

        //constructors
        AvlTree() : head{nullptr}, numElements{0} {}
        AvlTree(const AvlTree& other)
            : head{copy(other.head)}, numElements{other.numElements} {}
        AvlTree& operator=(const AvlTree& other);
        ~AvlTree();

        //general functions
        T& insert(T& data);
        T& insert(T&& data);
        T& find(const T& data);
        void clear();

        //overloaded ostream operator
        template<class S>
        friend std::ostream& operator<<(std::ostream& o, const AvlTree<S>& avl);
        std::ostream& print(std::ostream& o, const Node<T>* tree) const;

        Node<T>* getHead() const;
};

/**
 * getHead()
 *
 * @return a pointer to the head of the tree
 */
template<class T>
Node<T>* AvlTree<T>::getHead() const
{
    return head;
}



/**
 * insert(T& data, Node<T>*&)
 *
 * Helper function for insert, adding the data to the tree when the
 * given node is nullptr. After the node is inserted, the tree
 * will pop all instances of this function off the stack, while checking
 * if the tree needs to be rotated to retain the balance that AVL trees
 * require. If so, the tree will be rotated.
 *
 * @param data  - the data to add to the tree
 * @param node  - a pointer reference to the node in the tree
 * @return      - a reference to the data after it has been added to the tree
 */
template<class T>
T& AvlTree<T>::insert(T& data, Node<T>*& node)
{
    //Add to the tree if the node is nullptr
    if(node == nullptr)
    {
        node = new Node<T>(data);
        numElements++;
        return node->element();
    }
    //Otherwise, continue iterating
    else
    {
        if(data < node->element())
        {
            //Move left and rotate along this path
            T& temp = insert(data, node->left);
            rotate(node);
            return temp;
        }
        else if(node->element() < data)
        {
            //Move right and rotate along this path
            T& temp = insert(data, node->right);
            rotate(node);
            return temp;
        }
        else
            //Don't rotate if no elements are added
            return node->element();
    }
}


template<class T>
/**
 * copy(Node<T>*)
 *
 * Returns a tree containing the exact same data and structure
 * as the tree given by other using recursion
 *
 * @param other - a pointer to the head of the tree to copy from
 * @return      - a node, which is the head of the copied tree
 */
Node<T>* AvlTree<T>::copy(Node<T>* other)
{
    if(other == nullptr)
        return nullptr;
    return new Node<T>(other->data, copy(other->left), copy(other->right));
}


template<class T>
/**
 *
 * rotate(Node<T>*&)
 *
 * Rotates the AVL tree based on four different cases of imbalance within the
 * tree, where there could be imbalances within:
 * 1) the left subtree of the left child
 * 1) the right subtree of the left child
 * 1) the left subtree of the right child
 * 1) the right subtree of the right child
 *
 * @param node  - the pointer reference to a node that may be rotated
 */
void AvlTree<T>::rotate(Node<T>*& node)
{
    //Gets the balance of the node
    int balance = getBalance(node);
    //right side imbalance
    if(balance == -2)
    {
        //left subtree
        if(getBalance(node->right) > 0)
            leftRot(node->right);
        //right subtree
        rightRot(node);

    }
    //left side imbalance
    else if (balance == 2)
    {
        //right subtree
        if(getBalance(node->left) < 0)
            rightRot(node->left);
        //left subtree
        leftRot(node);
    }
    node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;

}

template<class T>
/**
 * rightRot(Node<T>*&)
 *
 * Rotation of right subtree, using the method outlined in SMU CSE 2341
 *
 * @param a1    - the head of the subtree to rotate
 */
void AvlTree<T>::rightRot(Node<T>*& a1)
{

    //imbalance on right subtree of right node

    Node<T>* a2{a1->right};
    a1->right = a2->left;
    a2->left = a1;
    a1->height = std::max(getHeight(a1->left), getHeight(a1->right)) + 1;
    a1 = a2;                //The new head
}

template<class T>
/**
 * leftRot(Node<T>*&)
 *
 * Rotation of left subtree, using the method outlined in SMU CSE 2341
 *
 * @param a1    - the head of the subtree to rotate
 */
void AvlTree<T>::leftRot(Node<T>*& a1)
{
    //Imbalance on left subtree of left node

    Node<T>* a2{a1->left};
    a1->left = a2->right;
    a2->right = a1;
    a1->height = std::max(getHeight(a1->left), getHeight(a1->right)) + 1;
    a1 = a2;
}

template<class T>
/**
 * getHeight(Node<T>*&)
 *
 * Gets the height of the subtree. If the node is nullptr, -1 is returned
 *
 * @param node  - the node to check the height of
 * @return      - -1 if nullptr, the height if not
 */
int AvlTree<T>::getHeight(Node<T>*& node)
{
    return (node == nullptr) ? -1 : node->height;
}

template<class T>
/**
 *
 * getBalance(Node<T>*&)
 *
 * Gets the balance of the node using its two child nodes, using getHeight().
 *
 * @param node  - the node to check the balance of
 * @return      - 0 if nullptr, the difference of its child heights if not
 */
int AvlTree<T>::getBalance(Node<T>*& node)
{
    return (node) ? getHeight(node->left) - getHeight(node->right) : 0;
}

template<class T>
/**
 * operator= overload
 *
 * @param other - the data to copy from
 * @return      - a reference to this AVL tree containing the new data
 */
AvlTree<T>& AvlTree<T>::operator=(const AvlTree& other)
{
    clear();
    head = copy(other.head);
    numElements = other.numElements;
    return *this;
}

template<class T>
/**
 * Destructor
 */
AvlTree<T>::~AvlTree()
{
    clear();
}


template<class T>
/**
 * insert(T&)
 *
 * Inserts the data into the tree, returning a reference to that data
 * once inserted.
 *
 * @param data  - the data to insert
 * @return      - a reference to the data after insertion
 */
T& AvlTree<T>::insert(T& data)
{
    return insert(data, head);
}

/**
 * insert(T&& data)
 *
 * Inserts the data into the tree, returning a reference to that data
 * once inserted.
 *
 * @param data  - an rvalue to add to the tree
 * @return      - a reference to the data after insertion
 */
template<class T>
T& AvlTree<T>::insert(T&& data)
{
    return insert(data);
}

template<class T>
/**
 *
 * Iteratively searches the the tree and returns a reference to the
 * data within the tree if it is found. Otherwise, an out_of_range
 * exception is thrown.
 *
 * @param data  - the data to search for within the tree
 * @return      - a reference to the data within the tree
 * @exception   - out_of_range if the data doesn't exist within the tree
 */
T& AvlTree<T>::find(const T& data)
{
    Node<T>* current{head};
    while(current != nullptr && current->data != data)
    {
        if(current->data > data)
            current = current->left;
        else
            current = current->right;
    }
    if(current == nullptr)
        throw std::out_of_range("The element does not exist in the tree");
    else
        return current->element();
}

template<class T>
/**
 * clear()
 *
 * Deletes all elements within the tree
 */
void AvlTree<T>::clear()
{
    if (head != nullptr)
    {
        delete head;
    }
}



template<class T>
/**
 *
 * print(std::ostream&,const Node<T>*)
 *
 * Helper function for the operator<<, outputting data in a *preorder*
 * fashion for easier retrieval from a text file. nullptr is referenced
 * as '$$$'
 *
 * @param o     - ostream reference to add data to
 * @param tree  - a tree to be outputted in preorder fashion
 * @return      - the ostream containing the tree's data
 */
std::ostream& AvlTree<T>::print(std::ostream& o, const Node<T>* tree) const
{
    if(tree == nullptr)
    {
        //nullptr delimiter
        o << "$$$" << std::endl;
    }
    else
    {
        //Preorder transversal of the data
        o << tree->data;
        print(o, tree->left);
        print(o, tree->right);
    }
    return o;
}

template<class S>
/**
 *
 * operator<< overload
 *
 * @param o     - ostream reference
 * @param avl   - AVL tree to take data from
 * @return      - ostream reference with the new data from the AVL tree
 */
std::ostream& operator<<(std::ostream& o, const AvlTree<S>& avl)
{
    //"start" and 31 dashes
    o << "start-------------------------------" << std::endl;
    if(avl.getHead() != nullptr)
        avl.print(o, avl.getHead());
    else
        o << "$$$" << std::endl;
    //"end" and 33 dashes
    o << "end---------------------------------" << std::endl;
    return o;
}
