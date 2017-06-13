#include "docdetails.h"


/**
 * Constructor with a single string
 *
 * Uses the information within the string to construct the three variables
 * of the DocDetails object. The string should contain three elements
 * delimited by '\t' characters.
 *
 * @param docInfo   - a string containing the document information
 */
DocDetails::DocDetails(std::__cxx11::string docInfo)
    : name{}, frequency{}, ranking{}
{
    int delim0 = docInfo.find('\t');
    int delim1 = docInfo.find('\t', delim0 + 1);
    int delim2 = docInfo.rfind('\t');

    name = docInfo.substr(delim0 + 1, delim1-1);
    frequency = std::stoi(docInfo.substr(delim1 + 1, delim2));
    ranking = std::stod(docInfo.substr(delim2 + 1));
}

/**
 * Destructor
 */
DocDetails::~DocDetails()
{
    frequency = 0;
    ranking = 0.0;
}


/**
 *
 * calculateRanking(int, double)
 *
 * Changes the ranking to reflect the idf and totalWordsInDoc provided,
 * and a new calculation of the term frequency.
 *
 * @param totalWordsInDoc   - the total number of words in the document
 * @param idf               - the inverse document frequency of the document
 */
void DocDetails::calculateRanking(int totalWordsInDoc, double idf)
{
    ranking = (((double) frequency) / totalWordsInDoc) * idf * 10000;
}

/** Operator functions */

bool operator==(const DocDetails& lhs, const std::__cxx11::basic_string<char> rhs)
{
    return lhs.name == rhs;
}

bool DocDetails::operator==(const DocDetails& rhs)
{
    return ranking < rhs.ranking;
}

bool DocDetails::operator<=(const DocDetails& rhs)
{
    return ranking <= rhs.ranking;
}

bool DocDetails::operator!=(const DocDetails& rhs)
{
    return ranking != rhs.ranking;
}

bool DocDetails::operator<(const DocDetails& rhs)
{
    return ranking < rhs.ranking;
}

bool DocDetails::operator>(const DocDetails& rhs)
{
    return ranking > rhs.ranking;
}

bool DocDetails::operator==(const std::__cxx11::string& rhs)
{
    return name == rhs;
}

bool DocDetails::operator<=(const std::__cxx11::string& rhs)
{
    return name <= rhs;
}

bool DocDetails::operator!=(const std::__cxx11::string& rhs)
{
    return name != rhs;
}

bool DocDetails::operator<(const std::__cxx11::string& rhs)
{
    return name < rhs;
}

bool DocDetails::operator>(const std::__cxx11::string& rhs)
{
    return name > rhs;
}
