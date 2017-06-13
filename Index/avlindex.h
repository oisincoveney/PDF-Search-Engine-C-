#pragma once

#include <AVLTree/avltree.h>
#include <Index/indexinterface.h>


/**
 *
 * AVLIndex is a subclass of IndexInterface, and is an implementation
 * of its parent class with an AVL tree.
 *
 * By: Oisin Coveney
 *
 */
class AVLIndex : public IndexInterface
{
    private:

        AvlTree<Word> tree;
        bool isEmpty;

        //Helper for recalculating the ranking of all elements
        void recalcRankingHelper(Node<Word>*& node);
    public:

        //Constructor
        AVLIndex() : tree{}, isEmpty{true} {}

        //Insertion
        Word& insert(std::string& word) override;
        void insert(std::__cxx11::string& word, std::__cxx11::string& document) override;
        void insert(Word word) override;

        //Check if empty
        bool empty();

        //Return element
        Word& get(const std::__cxx11::string& word) override;

        //Clear index
        void clear() override;

        //Load and print to file
        bool load() override;
        void printToFile() override;

        //Destructor
        ~AVLIndex();

        //Load tree from istream
        Node<Word>* loadTree(Node<Word>*& node, std::istream& is);

        //operator << overload
        friend std::ostream& operator<<(std::ostream& o, const AVLIndex& index);

        //Frequency List functions
        void addToFrequencyList(Word& word) override;
        void loadFrequencyList(Node<Word>*& head);
        void printFrequencyList() override;

        //Recalculate ranking
        void recalculateRanking() override;

        //get directory from file
        std::string getDirectoryFromFile() override;

        //Returns "AVL tree"
        std::string getDataType();
};
