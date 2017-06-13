#include <Word/word.h>


/**
 * operator<< overload
 *
 * Outputs the Word object to the ostream reference using the data provided
 * by the Word reference.
 *
 * Example output:
 *
 * aircraft :: 2 documents :: 5 times
 *  /home/oisin/Downloads/corpus/bok%3A978-0-387-88698-5.pdf	4	12.22
 *  /home/oisin/Downloads/corpus/5018.pdf	1	4.83
 *
 * @param o     - the ostream reference to pass data to
 * @param word  - the Word object reference
 * @return      - the ostream reference containing the Word data
 */
std::ostream& operator<<(std::ostream& o, const Word& word)
{
    o << std::endl;
    o << word.getWord() << " :: " << word.numDocs << " documents";
    o << " :: " << word.totalFrequency << " times" << std::endl;
    for(int i{}; i < (int) word.docs.size(); i++)
    {
        o << "\t";
        o << word.docs[i].name << "\t" << word.docs[i].frequency << "\t";
        o << std::setprecision(2) << std::fixed << word.docs[i].ranking;
        o << std::endl;
    }
    return o;
}



/**
 * operator= overload
 *
 * @param other - the Word reference to pull data from
 * @return      - a reference to this object
 */
Word&Word::operator=(const Word& other)
{
    word = other.word;
    docs = other.docs;
    numDocs = other.numDocs;
    totalFrequency = other.totalFrequency;
    return *this;
}

/**
 *
 * Constructor with std::string and std::istream&
 *
 * Constructs the word object by iterating through the istream reference
 * and pulling data into the Word object
 *
 * @param name  - the string of the word
 * @param is    - the istream reference
 */
Word::Word(std::string& name, std::istream& is)
    : word{name}, docs{}, numDocs{}, totalFrequency{}
{
    is.ignore(4);
    is >> numDocs;
    is.ignore(14);
    is >> totalFrequency;
    is.ignore(7);

    //Gets all the documents found within the file
    std::string docInfo;
    for(int i{}; i < numDocs && i < NUM_DOCS; i++)
    {
        getline(is, docInfo);
        docs.push_back(DocDetails(docInfo));
    }
}

/**
 * Destructor
 */
Word::~Word()
{
    docs.clear();
    numDocs = 0;
}

/**
 *
 * addDoc(std::string)
 *
 *
 *
 * @param document
 */
void Word::addDoc(std::string document)
{
    // Finds the document within the list of documents
    auto it = std::find_if(docs.begin(), docs.end(), [document](DocDetails d)
    {
              return d.name == document;
});
    //If the document exists, add to frequency
    if(it != docs.end())
    {
        it->frequency++;
    }
    else
    {
        //Adds the document to the doc list
        numDocs++;
        docs.push_back(DocDetails(document, 1, 0.0));
    }

    totalFrequency++;
}

/**
 * calculateRanking()
 *
 * Calculates the tf-idf ranking for all files within the list of documents.
 */
void Word::calculateRanking()
{
    // idf ranking, normalized so no fractions must be divided by 0
    double idf = 1 + log2((double) Parser::getNumFiles() / numDocs);

    //Iterates through the list calculating the ranking
    for(DocDetails& d : docs)
    {
        d.calculateRanking(Parser::totalWordsInDoc[d.name], idf);
    }

    //Sorts the list based on ranking
    std::sort(docs.rbegin(), docs.rend(), [](DocDetails& lhs, DocDetails& rhs)
    {
        return lhs.ranking < rhs.ranking;
    });

    //Pops all documents that are small and outside the bounds of what
    // is needed for the project
    while(docs.size() > NUM_DOCS)
    {
        docs.pop_back();
    }
}



/**
 * empty()
 * @return true if the word does not contain any information, false if not
 */
bool Word::empty()
{
    return word == "";
}


/**
 *
 * combine()
 *
 * Combines two words and their documents, using the premise of the OR operator.
 * The documents within the other word are added to the document list
 * within this Word. If documents match, their frequencies are added together.
 *
 * The words are concatenated with an ' or ' between the two words.
 *
 * @param other - the Word reference to combine with
 * @return      - this Word object containing the new data
 */
Word& Word::combine(const Word& other)
{
    word += (" or " + other.getWord());



    for(DocDetails od : other.docs)
    {
        //Finds the document within this doc list
        auto it = std::find(docs.begin(), docs.end(), od.name);

        //If found, the frequencies are added together
        if(it != docs.end())
        {
            it->frequency += od.frequency;
        }
        //If not, then the document is added to the list
        else
        {
            numDocs++;
            docs.push_back(od);
        }

    }

    //Calculate the new tf-idf rankings of the list
    calculateRanking();
    return *this;
}

/**
 *
 * intersect()
 *
 * Puts the two words together based on the premise of intersection,
 * in which only the elements that the two Words have in common with
 * each other will remain. Thus, only documents that both Word objects
 * have will be retained.
 *
 * The words are concatenated with an ' and ' between the two words.
 *
 * @param other - the Word reference to intersect with
 * @return      - this Word object containing the new data
 */
Word& Word::intersect(const Word& other)
{
    word += (" and " + other.getWord());

    //Iterate through all of this Word's documents
    for (auto it = docs.begin(); it != docs.end();)
    {

        //If no documents match, remove from docs list
        auto otherIt = std::find(other.docs.begin(), other.docs.end(), it->name);
        if (otherIt == other.docs.end())               //if no elements match
        {
            numDocs--;
            it = docs.erase(it++);
        }
        //Otherwise, get the minimum frequency and make that the new frequency
        else
        {
            it->frequency = std::min(it->frequency, otherIt->frequency);
            ++it;
        }
    }

    calculateRanking();
    return *this;
}

/**
 *
 * difference()
 *
 * Removes any common elements between the two words from this Word object.
 * Thus, any document that this Word may contain will be removed if it
 * matches with the documents of the other Word object
 *
 * The words are concatenated with ' not ' between the two words.
 *
 * @param other - the Word reference containing elements to remove
 * @return      - the Word object without any common elements with other
 */
Word& Word::difference(const Word& other)
{
    word += (", not " + other.getWord());

    for (auto it = docs.begin(); it != docs.end();)
    {
        //If the documents match, remove from this Document list
        auto otherIt = std::find(other.docs.begin(), other.docs.end(), it->name);
        if (otherIt != other.docs.end())               //elements match
        {
            numDocs--;
            it = docs.erase(it++);
        }
        //Otherwise, simply iterate
        else
        {
            ++it;
        }
    }

    calculateRanking();
    return *this;
}


/* Operator functions */

bool Word::operator==(const Word& rhs)
{
    return word == rhs.getWord();
}
bool Word::operator<=(const Word& rhs)
{
    return word <= rhs.getWord();
}
bool Word::operator!=(const Word& rhs)
{
    return word != rhs.getWord();
}
bool Word::operator<(const Word& rhs)
{
    return word < rhs.getWord();
}
bool Word::operator>(const Word& rhs)
{
    return word > rhs.getWord();
}
bool Word::operator==(std::string& rhs)
{
    return word == rhs;
}
bool Word::operator<=(std::string& rhs)
{
    return word <= rhs;
}
bool Word::operator!=(std::string& rhs)
{
    return word != rhs;
}
bool Word::operator<(std::string& rhs)
{
    return word < rhs;
}
bool Word::operator>(std::string& rhs)
{
    return word > rhs;
}
