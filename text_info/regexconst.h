
#ifndef REGEXCONST
#define REGEXCONST

namespace reg
{

// countSentEnd()
    // Monograms will return a false negative.
    // sentences ending with a single letter, happens far less often
    // than 'Annie B. Cochran' etc. 
const std::regex initialCase("([\\w][\\.])+");
const std::regex elipsesCase("(.*?)\\.{3}$");
const std::regex titleCase(
    "(([sSdDmMjJ][r|R])|(([mM][rR])|[Mm])[sS])[\\.\\?\\!]");
const std::regex nonsenseCase("[_-]+[\\.\\?\\!]");

// cleanArt()
const std::regex charArt("[_-]+");

}

#endif
