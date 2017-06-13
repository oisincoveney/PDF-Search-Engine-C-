#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <fstream>

#include <Word/docdetails.h>
#include <Parser/parser.h>


#define NUM_DOCS 15

/**
 * The Word class is an object that encapsulates a word within an index
 * for the PDF search engine. The Word object contains a vector
 * of DocDetails objects, which contain the data of each document
 * where the word exists. The Word class also contains the functions
 * for a union, intersection, and difference of a Word object,
 * which are useful within the QueryProcessor class.
 */
class Word
{
    private:

        //member variables
        std::string word;
        std::vector<DocDetails> docs;
        int numDocs;
        int totalFrequency;


    public:

        //Constructors and destructor
        Word() : word{}, docs{}, numDocs{}, totalFrequency{} {}
        Word(const Word& other)
            : word{other.word}, docs{other.docs},
              numDocs{other.numDocs}, totalFrequency{other.totalFrequency} {}
        Word& operator=(const Word& other);
        Word(std::string word)
            : word{word}, docs{}, numDocs{}, totalFrequency{} {}
        Word(std::string& name, std::istream& is);
        ~Word();


        //Add document to word and calculate the ranking of all documents
        void addDoc(std::__cxx11::string document);
        void calculateRanking();

        //Check if word == ""
        bool empty();

        friend std::ostream& operator<<(std::ostream& o, const Word& word);

        //union, intersection, difference function
        Word& combine(const Word& other);
        Word& intersect(const Word& other);
        Word& difference(const Word& other);


        //operators
        bool operator==(const Word& rhs);
        bool operator<=(const Word& rhs);
        bool operator!=(const Word& rhs);
        bool operator<(const Word& rhs);
        bool operator>(const Word& rhs);

        bool operator==(std::string& rhs);
        bool operator<=(std::string& rhs);
        bool operator!=(std::string& rhs);
        bool operator<(std::string& rhs);
        bool operator>(std::string& rhs);

        //getters and setters
        std::__cxx11::string getWord() const
        {
            return word;
        }
        void setWord(const std::string& value)
        {
            word = value;
        }
        int getTotalFrequency() const
        {
            return totalFrequency;
        }
        void setTotalFrequency(int value)
        {
            totalFrequency = value;
        }
        std::vector<DocDetails> getDocs() const
        {
            return docs;
        }
        void setDocs(const std::vector<DocDetails>& value)
        {
            docs = value;
        }
};
