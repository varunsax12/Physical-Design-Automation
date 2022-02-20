#ifndef __HELPERFUNCS_H__
#define __HELPERFUNCS_H__

#include <string>
#include <algorithm>
#include <vector>
#include <locale>

/*
* Function to convert string to lower case
* @param inStr -> string to process
*/
inline std::string str_to_lower(std::string inStr)
{
    std::locale loc;
    std::string outStr;
    for (size_t i = 0; i < inStr.length(); ++i)
    {
        outStr += std::tolower(inStr[i], loc);
    }
    return outStr;
}

/*
* Comparator function for comparing two spaces
*/
inline bool check_space(char str1, char str2)
{
    return (str1 == ' ' && str2 == ' ');
}

/*
* Function to remove continuous multiple spaces
* @param inStr -> string to process
*/
inline void remove_multiple_spaces(std::string& inStr)
{
    std::string::iterator endPt = std::unique(inStr.begin(), inStr.end(), check_space);
    inStr.erase(endPt, inStr.end());
}

/*
* Function to remove tabs
* @param inStr -> string to process
*/
inline void remove_tabs(std::string& inStr)
{
    inStr.erase(std::remove(inStr.begin(), inStr.end(), '\t'), inStr.end());
}

/*
* Function to remove the newline character
* Updates the reference directly
*/
inline void remove_newline(std::string& inStr)
{
    if (!inStr.empty() && inStr[inStr.length() - 1] == '\n') {
        inStr.erase(inStr.length() - 1);
    }
}

/*
* Function to split string across delimiter
* @param inStr -> string to process
* @param delimiter -> to split the string across
*/
inline std::vector<std::string> split_str(std::string inStr, char delimiter = ' ')
{
    std::vector<std::string> strParts;
    std::string newWord = "";

    // add space in the end to make the logic more generic
    // so that if it in the last word 
    inStr += " ";

    // Remove random consecutive spaces
    //remove_multiple_spaces(inStr);
    for (unsigned int i = 0; i < inStr.length(); ++i)
    {
        // if equal to delimiter
        if (inStr[i] == delimiter)
        {
            strParts.push_back(newWord);
            newWord = "";
        }
        else
        {
            newWord += inStr[i];
        }
    }
    return strParts;
}


/*
* Function to print vector
*/
template <typename T>
inline void print_vector(std::vector<T> inVec)
{
    for (auto x : inVec)
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

#endif