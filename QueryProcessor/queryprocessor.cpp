#include "queryprocessor.h"


/**
 *
 * process(std::string, IndexInterface)
 *
 * Takes a simple prefix boolean query containing the operands AND, OR, or NOT
 * to create a Word object that contains a search from the index provided
 * through the parameter.
 *
 * If no operand is provided, the default operation used with be the OR
 * operation.
 *
 * @param query - a simple prefix boolean query
 * @param index - the IndexInterface pointer reference containing
 *                  the index to search
 * @return      - a Word object containing the data found from the search
 */
Word QueryProcessor::process(std::__cxx11::string& query, IndexInterface*& index)
{

    //The object to return
    Word word{};

    //A stringstream to containing the query
    std::stringstream queryStream(query);

    //Default operation
    std::string operation{"OR"};
    std::string token;
    while(queryStream >> token)
    {
        //Search for the word in the index if they aren't operands
        if(token != "AND" && token != "OR" && token != "NOT")
        {
            if(Parser::invalidLength(token))
            {
                std::cout << "The word \"" << token << "\" is too short and"
                     << " has been deemed irrelevant to the search. The "
                     << "word will be omitted from the search.\n";
            }
            else if(Parser::isStopWord(token))
            {
                std::cout << "The word \"" << token << "\" is a common word that"
                     << " has been deemed irrelevant to the search. The "
                     << "word will be omitted from the search.\n";
            }
            else
            {
                //Stems the word and makes it lowercase
                Porter2Stemmer::stem(token);
                for(auto it = token.begin(); it != token.end(); ++it)
                    *it = tolower(*it);

                //Catches the out_of_bounds error if the word isn't found
                try
                {
                    Word& reference = index->get(token);

                    //The operations performed on the word
                    if(operation == "AND")
                        word.empty() ? word = reference : word.intersect(reference);
                    else if(operation == "OR")
                        word.empty() ? word = reference : word.combine(reference);
                    else if(operation == "NOT" && !word.empty())
                        word.difference(reference);

                }
                catch (std::out_of_range) {}

            }
        }
        else
        {
            //Set the new operation to the token
            operation = token;
        }
    }
    return word;
}
