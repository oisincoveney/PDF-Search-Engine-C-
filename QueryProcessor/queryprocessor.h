#ifndef QUERYPROCESSOR_H
#define QUERYPROCESSOR_H


#include <sstream>
#include <Parser/parser.h>

/**
 * The QueryProcessor is a static class that takes processes a string
 * query, usually inputted by the user, and searches the IndexInterface
 * to find the appropriate word.
 *
 * By: Oisin Coveney
 */
class QueryProcessor
{
    public:
        static Word process(std::__cxx11::string& query, IndexInterface*& index);
};

#endif // QUERYPROCESSOR_H
