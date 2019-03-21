#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdint.h>
#include <fstream>
#include "textinfofunc.h"


int main(int argc, char* argv[])
{
    int i = argc - 1;
    for ( ; i > 0; i--) {

	std::string fileName = argv[i];
        
        // Collect all words
	std::vector<std::string> allWords = pre::vectorizeFile(fileName);

        // Zip unique words and their occurences in vector
	std::vector<std::pair<std::string, int> > alphUniq;
	alphUniq = pre::zipAlphList(allWords);

        // re-order alphabetized zip by occurences
	std::vector<std::pair<std::string, int> > numbUniq;
	numbUniq = pre::zipCountList(alphUniq);

        // Print .txt of each zip
	std::string out = fileName.substr(0, fileName.size() - 4);
	pre::printZipStrInt("./lists/" + out + "_alph.txt", alphUniq);
	pre::printZipStrInt("./lists/" + out + "_freq.txt", numbUniq);    

        // Tacky text-word analysis for console print
	std::cout  << fileName;
	std::cout << '\n' << "words:          " << pre::g_wordCount;
	std::cout << '\n' << "unique words:   " << pre::g_uniqueCount;
	std::cout << '\n' << "sentences:      " << pre::g_sentenceCount;
	std::cout << '\n' << "Most used word: " 
                  << '\'' << numbUniq[0].first << '\'' 
                  << ' ' << numbUniq[0].second;

        // Complimentary, tacky text-character analysis for console print
	std::cout << '\n' << "characters:     " << pre::g_charCount;
	std::cout << '\n' << "letters:        " << pre::g_letterCount;
	std::cout << '\n' << "numbers:        " << pre::g_numberCount;
	std::cout << '\n' << "specials:       " << pre::g_specialCount;
	std::cout << '\n' << "lines:          " << pre::g_lineCount;

        // Globals must be reset for next iteration; in place of object init
	pre::resetGlobals();

    }

    return 0;
}
