#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <Word/word.h>
#include <AVLTree/avltree.h>
class Word;

#define MAX_FREQ 50


/**
 *
 * IndexInterface is an abstract class containing the functions for
 * a PDF index. The interface outlines the functions for a typical PDF
 * search index, which is implemented with an AVL tree and hash table
 * in the AVLIndex and HashIndex classes.
 *
 * By: Oisin Coveney
 *
 */
class IndexInterface
{
        friend class GUI;
        friend class AVLIndex;
        friend class HashIndex;

    private:

        //persistent index
        std::string indexFile{"index.txt"};

        //table of documents and their word counts
        std::string wordCount{"wordcount.txt"};

        //Table of most frequent words
        std::vector<Word*> frequentWords{};

        //Directory of the words
        std::string indexDirectory;

    public:

        //note:a word object only needs to have 15 documents = fixed array


        //inserting words into index
        virtual Word& insert(std::string& word) = 0;
        virtual void insert(std::string& word, std::string& document) = 0;
        virtual void insert(Word word) = 0;

        //Add word to the list of most frequent words
        virtual void addToFrequencyList(Word& word) = 0;

        //retrieving word from index - will not return void
        virtual Word& get(const std::__cxx11::string &word) = 0;

        //checks if index is empty
        virtual bool empty() = 0;

        //load index from a file
        virtual bool load() = 0;

        //clear index
        virtual void clear() = 0;

        //output index to a file
        virtual void printToFile() = 0;

        //destructor
        virtual ~IndexInterface() {}

        //Print the list of most frequent words
        virtual void printFrequencyList() = 0;

        //Calculate ranking of all elements in the index
        virtual void recalculateRanking() = 0;

        //Get the data type of the index
        virtual std::string getDataType() = 0;

        //Get directory from the index file
        virtual std::string getDirectoryFromFile() = 0;


        //Getters and setters
        std::string getIndexFile() const { return indexFile; }
        std::string getIndexDirectory() const { return indexDirectory;}
        void setIndexDirectory(const std::string& value) { indexDirectory = value; }
        std::vector<Word*> getFrequentWords() const { return frequentWords; }
        std::string getWordCount() const { return wordCount; }
};
