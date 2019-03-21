#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
#include "regexconst.h"


namespace pre
{


// Globals
unsigned int g_wordCount     = 0;
unsigned int g_sentenceCount = 0;
unsigned int g_uniqueCount   = 0;
unsigned int g_lineCount     = 0;
unsigned int g_highestUse    = 0;
unsigned int g_charCount     = 0;
unsigned int g_letterCount   = 0;
unsigned int g_numberCount   = 0;
unsigned int g_specialCount  = 0;

void resetGlobals()
{
    g_wordCount     = 0;
    g_sentenceCount = 0;
    g_uniqueCount   = 0;
    g_lineCount     = 0;
    g_highestUse    = 0;
    g_charCount     = 0;
    g_letterCount   = 0;
    g_numberCount   = 0;
    g_specialCount  = 0;
}


unsigned int _getFileSize(const std::string& fileName)
{
    using namespace std;
    
    ifstream endPos(fileName, ios::binary | ios::ate);
    return endPos.tellg();
}


void _countSent(const std::string& word)
{/* 
    Estimate whether or not each word-with-punctuation
    marks the end of a sentence.

    Proprietary syntax, citation formating syntax, haplography,
    and possible unusual full-stop use will likely render false
    positives and negatives.

    Regular expressions are stored in const namespace 'reg'
    in 'regexconst.h'. Generic typing is being used for efficiency.
 */
    using namespace std;

    // Qualifying condition; necessary for efficiency; regex is slow
    const char& last = word.back();
    if (last == '.' | last == '?' | last == '!'){

        // False positives, "Mr.", "...", "B.I.G." etc.
	if (!regex_match(word, reg::elipsesCase) &&
	    !regex_match(word, reg::initialCase) &&
	    !regex_match(word, reg::titleCase)   &&
        !regex_match(word, reg::nonsenseCase) ) {

	    g_sentenceCount += 1;
	}
    }
}


std::string& _sanitize(std::string& str)
{/*
    Lowers all caps and strips non-word special characters from a string
 */
    _countSent(str);
    int i = str.length() - 1;
    for ( ; i >= 0; i--){

	char& character = str.at(i);
        if (character >= '0' && character <= '9' ||
            character >= 'a' && character <= 'z' ||
            character == '_' || character == '-' 
           ) {
            continue; }

    // 0x20 is the difference between cases by ASCII
	else if (character >= 'A' && character <= 'Z') {
	    character |= 0x20;
            continue; }
        else {
            str.erase(i, 1); }
    }

    return str;
}


void _extractWords(std::vector<std::string>& vec, const std::string& line)
{/* 
    Append seperately each word in a string of words to 'vec'
 */
    using namespace std;
    
    int index = 0;
    int length = line.length();
    string word("");
    for (char character : line) {

        // Use white-space to delimit new words; When word completes:
        if (character <= ' ') {
            _sanitize(word);

            // Add remaining letters as string to 'vec'
            if (word != ""){
                g_wordCount++;
                vec.push_back(word);
                word = ""; } }
        
        // While word is incomplete: add letter to word
        else {
            word.push_back(character);
            g_charCount++;            
            
            // Increment appropriate character type counter
            if (character <= 'Z' && character >= 'A' ||
                character <= 'z' && character >= 'a' )
                g_letterCount++;
            else if (character >= '0' && character <= '9') {
                g_numberCount++; }
            else {
                g_specialCount++; } }
    }

        // The last iteration will be skipped in loop
	_sanitize(word);
	if (word != ""){
	    g_wordCount++;
	    vec.push_back(word);
	    word = ""; }
}            


std::vector<std::string> vectorizeFile(const std::string& fileName)
{/* 
    Extract contents of file into a vector of words
 */
    using namespace std;
    
    long int fileSize = _getFileSize(fileName);

    // Reserve space for worst-case file size
    vector<string> result;
    result.reserve(fileSize);

    // Parse each line; let 'extractwords' store in 'result'
    string line("");
    ifstream file(fileName);
    while (getline(file, line)){
        g_lineCount++;
        _extractWords(result, line); 
    }
    
    file.close();

    // global 'wordCount' == 'result.size()'
    result.resize(g_wordCount);
    return result;
}


int _getAlphIndex(const char &character)
{/*
    Enumerate character by it's index in the alphabet
    Numbers are enumerated with 0; 97 for ASCII 'a', less for numbers
    No checking is done for invalid characters '\' etc.
 */
    if (character < 97){
	return 0; }
    return character - 96;
}


auto _binaryPairFind(
    std::vector<std::pair<std::string, int> >::iterator start,
    std::vector<std::pair<std::string, int> >::iterator end,
    std::string& term)
{/* 
    Use a binary search to find a match between string and first member 
    of pair in vector of pairs.
    
    Return pointer to matching pair.
 */
    using namespace std;

    // Check for empty vector
    if (start == end) return start; 

    // Create an iterator to find the middle element of vector
    auto search = start;

    // An end will move up to the middle element after each 
    // iteration; continue until they have closed on each other
    // to within 1 iteration to spare an infinite loop
    while (start < end - 1) {

        // Check for match between term and the element at 'search'
        if (term == search->first) {
            return search; }

        // If the element is greater, 'end' moves back to 'search' and
        // 'search' is moved to the middle between 'start' and the 
        // new 'end'
        else if (term < search->first) {
            end = search;
            search = (end - start) / 2 + start; }

        // Else, the element must be less so 'start' moves up to 
        // 'search' and 'search' is still moved to the new middle
        else {
            start = search;
            search = (end - start) / 2 + start; }
    }

    // Since the while loop ends 1 iteration before checking the
    // first or last iteration, 'search' is checked a last time
    // to see if term is placed on or before the element.
    if (term <= search->first) {
        return search; }
    return search + 1;
}


auto _flatten2D(const std::vector<std::vector<std::pair<std::string, int> > >& vec,
               long int totalSize=g_uniqueCount)
{/* 
    Chain a 2D vector
 */
    using namespace std;

    vector<pair<string, int> > result;
    result.reserve(totalSize);

    int length = vec.size();
    for (int i=0; i < length; i++) {
        for (pair<string, int> eachPair : vec[i]) {
            result.push_back(eachPair); 
        }
    }

    return result;
}

auto _cleanArt(std::vector<std::pair<std::string, int> >& vec)
{/* 
    Iterate through vector of pairs, delete each iteration with pointer
    to non-word strings; delete remaining nonsense / art e.g. '-___-'

    Deleting afterwards looks like doing twice as much work, but really we are
    saving time by not calling a regex on every word in text file. We are doing
    twice, but only on a much smaller pool of strings.
 */
    using namespace std;
    
    auto iterator = vec.end() - 1;
    auto begin    = vec.begin();
    for ( ; iterator >= begin; iterator--) {
        if (regex_match(iterator->first, reg::charArt)) {
            g_wordCount -= iterator->second;
            g_uniqueCount -= 1;
            vec.erase(iterator); }
    }

    return vec;
}
        

std::vector<std::pair<std::string, int> > zipAlphList(
    const std::vector<std::string>& vec)
{/*
    Return a zip containing each word in 'vec' and that word's number of occurences
 */
    using namespace std;

    // Create a 2D vector with a vector for each letter and one for numbers 
    vector<vector<pair<string, int> > > alphVec(27);
    
    // Insert in order each unique element in 'vec' by first letter to alphVec 
    for (string word : vec){
        // Find container for 'word'; 'cod' c -> 3 for alphVec[3]...
        int index = _getAlphIndex(word.at(0));
        
        // Check for 'word's order as an iterator to its container
        auto begin = alphVec[index].begin();
        auto end   = alphVec[index].end();
        auto home  = _binaryPairFind(begin, end, word);
    
        // If 'home' already stores 'word' in a pair, then increase 'word's count
        if (home < end && home->first == word) {
            home->second += 1;
            int& occurences = home->second;
            g_highestUse = g_highestUse > occurences ? g_highestUse : occurences;
            continue; }
     
        // Add new unique word to alphVec[*];
        // BinaryPairFind() has already returned the correct iterator
        g_uniqueCount += 1;
        pair<string, int> zippedWord(word, 1);
        alphVec[index].insert(home, zippedWord);
    }

    // Remove strings of '-' and '_' without letters or numbers; erase nonsense
    if (!alphVec[0].empty()) {
        _cleanArt(alphVec[0]);}
    
    return _flatten2D(alphVec);
}
   

auto zipCountList(const std::vector<std::pair<std::string, int> >&  zippedList, 
                  int highestUse=g_highestUse)
{/*
    Return a new vector of 'zippedList's elements
    ordered by their second value
 */
    using namespace std;

    // Sort by creating a 2D vector with an index for all possible
    // occurences from 1 to the highest count for an occurence
    vector<vector<pair<string, int> > > numVec(highestUse);

    // Append each pair from the alphabetized list to the
    // correspondingly indexed vector
    for (pair<string, int>each : zippedList) {
        int index = highestUse - each.second;
        numVec[index].push_back(each);
    }
    
    return _flatten2D(numVec);
}

       
void printZipStrInt(std::string fileName, 
std::vector<std::pair<std::string, int> > list, char delimiter=':', int trunc=20)
{/*
    Print each pair in 'list' to 'fileName' as .txt
 */
    using namespace std;

    ofstream alphFile(fileName);
    for (pair<string, int> each : list) {
	string& word = each.first;
        int length = word.length();
	int truncateIndex = trunc > length ? length : trunc;
        string space(trunc - truncateIndex + 1, ' ');
	alphFile << word.substr(0, truncateIndex) << space
                 << delimiter  <<  each.second << endl;
    }

    alphFile.close();
}


} // end namespace
