#ifndef TEXTINFOFUNC
#define TEXTINFOFUNC


namespace pre
{

    extern long int g_charCount;
    extern long int g_letterCount;
    extern long int g_numberCount;
    extern long int g_specialCount;
    extern long int g_sentenceCount;
    extern long int g_lineCount;
    extern long int g_wordCount;
    extern long int g_uniqueCount;
    extern long int g_highestUse;

    void resetGlobals();

    std::vector<std::string> vectorizeFile(const std::string& fileName);

    std::vector<std::pair<std::string, int> > zipAlphList(
        const std::vector<std::string>& vec);
    

    std::vector<std::pair<std::string, int> > zipCountList(
        const std::vector<std::pair<std::string, int> >& zippedList, 
        int highesUse=g_highestUse);

    void printZipStrInt(std::string fileName, 
        std::vector<std::pair<std::string, int> > list, 
        char delimiter=':', int trunc=20);

}


#endif
