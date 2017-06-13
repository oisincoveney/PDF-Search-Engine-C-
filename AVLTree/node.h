#pragma once

#include <iostream>

template<class T>
/**
 * The Node class are used as the main objects for the AVL tree. Nodes
 * contain data and pointers to two different subnodes, known as children,
 * and contain a height variable which contains the maximum height the tree.
 *
 * This class will be used for the PDF Search Engine final project
 * in SMU CSE 2341.
 *
 * By: Oisin Coveney
 *
 */
class Node
{

        template <class U> friend class AvlTree;
    public:

        //Data and pointers to next nodes
        T data;
        Node<T>* left;
        Node<T>* right;
        int height;

    public:

        //Constructors
        Node() :
            data{}, left{nullptr}, right{nullptr}, height{0}{}

        Node(T data) :
            data{data}, left{nullptr}, right{nullptr}, height{0}{}

        Node(T data, Node<T>* left, Node<T>* right) :
            data{data}, left{left}, right{right}, height{0}{}

        Node(const Node& other) :
            data{other.data}, left{other.left}, right{other.right},
            height{other.height}{}

        //operator= overload
        Node& operator=(const Node& other);

        ~Node();

        T& element();

        //Getter and setter for data
        T getData() const;
        void setData(const T& value);

        template<class S>
        friend std::ostream& operator<<(std::ostream&, const Node<T>&);
};

template <class T>
T Node<T>::getData() const
{
    return data;
}

template<class T>
void Node<T>::setData(const T& value)
{
    data = value;
}

template<class T>
/**
 *
 * operator= overload
 *
 * @param other - the Node reference to copy from
 * @return      - this Node with the new data
 */
Node<T>& Node<T>::operator=(const Node<T>& other)
{
    data = other.data;
    left = other.left;
    right = other.right;
    height = other.height;
    return *this;
}

/**
 * Destructor
 *
 * This destructor deletes the left and right subtrees. Thus, when
 * a node is deleted, ALL its children are deleted as well.
 *
 */
template<class T>
Node<T>::~Node()
{
    if(left != nullptr)
    {
        delete left;
        left = nullptr;
    }
    if(right != nullptr)
    {
        delete right;
        right = nullptr;
    }
}

template<class T>
/**
 * element()
 *
 * @return  - a reference to the data within the node
 */
T& Node<T>::element()
{
    return data;
}

template<class T>
/**
 *
 * operator<< overload
 *
 * @param o     - the ostream reference
 * @param node  - the node to add data to
 * @return      - the ostream reference with the new data
 */
std::ostream& operator<<(std::ostream& o, const Node<T>& node)
{
    o << "NODE:" << "  data: " << node.data << "\t height: " << node.height;
    o << "\t ADDRESS: " << &node;
    o << "\t left: " << node.left << "\t right: " << node.right;

    return o;
}

