#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <array>

#include <AVLTree/avltree.h>
#include <Word/word.h>


#define LENGTH 10000


/**
 * The HashTable class contains a hash table implementation made specifically
 * for the Word class, serving as an effective hash table structure for the
 * PDF search engine. The hash table contains an array with a globally
 * defined LENGTH, with all elements containing AVL trees.
 *
 * By: Oisin Coveney
 */
class HashTable
{
        friend class AvlTree<Word>;
    public:

        //Array/vector of linked lists for chaining
        std::array<AvlTree<Word>, LENGTH> table;

    public:

        //Constructor
        HashTable() : table{} {}

        //Insertion
        Word& insert(std::string& key);
        Word& insert(std::string& key, std::string& document);
        Word& insert(Word& word);

        //Hash function
        int hash(const std::__cxx11::string& key) const;
        int hash(std::__cxx11::string& key);
        int hash(std::__cxx11::string&& key);

        //Clear
        void clear();

        //Destructor
        ~HashTable();

        //General functions
        AvlTree<Word>& operator[](int index);
        friend std::ostream& operator<<(std::ostream& o, const HashTable& tab);
        Word& getWord(const std::__cxx11::string& key);
        int getSize();

};




