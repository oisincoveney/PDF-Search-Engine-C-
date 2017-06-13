#pragma once

#include <Index/indexinterface.h>
#include <HashTable/hashtable.h>

/**
 *
 * HashIndex is a subclass of IndexInterface, and is an implementation
 * of its parent class with a hash table. The hash table contains an
 * array of AVL trees.
 *
 * By: Oisin Coveney
 *
 */
class HashIndex : public IndexInterface
{

    private:
        HashTable table;
        bool isEmpty;

        //Helper for recalculating the ranking of all elements
        void recalcRankingHelper(Node<Word>*& node);

    public:
        //Constructor
        HashIndex() : table{}, isEmpty{true} {}

        //Insertion functions
        Word& insert(std::__cxx11::string& word) override;
        void insert(std::__cxx11::string& word, std::__cxx11::string& document) override;
        void insert(Word word);

        //Check if empty
        bool empty();

        //Return reference to Word
        Word& get(const std::__cxx11::string& word) override;

        //Clear index
        void clear() override;

        //Load and print to file
        bool load() override;
        void printToFile() override;

        //Destructor
        ~HashIndex();

        //Getter
        HashTable getTable() const;

        //operator << overload
        friend std::ostream& operator<<(std::ostream& o, const HashIndex& index);

        //Load tree from istream
        Node<Word>*loadTree(Node<Word>* node, std::istream& is);
        int getHeight(Node<Word>*& node);

        //Frequency List functions
        void addToFrequencyList(Word& word) override;
        void loadFrequencyList(Node<Word>* head);
        void loadFrequencyList();
        void printFrequencyList() override;

        //Recalculate ranking
        void recalculateRanking() override;

        //get directory from file
        std::string getDirectoryFromFile() override;

        //Returns "hash table"
        std::string getDataType();

};
