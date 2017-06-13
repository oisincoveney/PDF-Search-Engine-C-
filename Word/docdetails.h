#pragma once
#include <string>
#include <cmath>

/**
 * The DocDetails struct contains data that describes a document and
 * the frequency of a word within the class. The class also holds
 * the tf-idf ranking of a document within the word, which ranks
 * its relevant through the term frequency, inverse document frequency
 * fraction.
 *
 * By: Oisin
 */
struct DocDetails
{
        //Member variables
        std::string name;
        int frequency;
        double ranking;

        //Constructors
        DocDetails() : name{}, frequency{0}, ranking{0.0} {}
        DocDetails(std::string docInfo);
        DocDetails(std::string docName, int freq, double rank)
            : name{docName}, frequency{freq}, ranking{rank} {}
        ~DocDetails();

        //Get the rankings
        void calculateRanking(int totalWordsInDoc, double idf);

        //operators
        bool operator==(const DocDetails& rhs);
        bool operator<=(const DocDetails& rhs);
        bool operator!=(const DocDetails& rhs);
        bool operator<(const DocDetails& rhs);
        bool operator>(const DocDetails& rhs);

        bool operator==(const std::string& rhs);
        bool operator<=(const std::string& rhs);
        bool operator!=(const std::string& rhs);
        bool operator<(const std::string& rhs);
        bool operator>(const std::string& rhs);

        friend bool operator==(const DocDetails& lhs, const std::__cxx11::basic_string<char> rhs);
};


