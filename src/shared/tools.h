#ifndef TOOLS_H
#define TOOLS_H

#include "common.h"

void stringToLower(std::string& s);
void stringToUpper(std::string s);

bool fileHasExtension(const char *fn, const char *ext);

void HexStrToByteArray(char *dst, const char *str);
bool WildcardMatch(const char *str, const char *pattern);


std::string GetDateString(void);
std::string GetTimeString(void);


template <class T> void StrSplit(const std::string &src, const std::string &sep, T& container, bool keepEmpty = false)
{
    std::string s;
    for (std::string::const_iterator i = src.begin(); i != src.end(); i++)
    {
        if (sep.find(*i) != std::string::npos)
        {
            if (keepEmpty || s.length())
                container.push_back(s);
            s = "";
        }
        else
        {
            s += *i;
        }
    }
    if (keepEmpty || s.length())
        container.push_back(s);
}

template <class IT> IT CompactRange(IT rd, IT end)
{
    IT w = rd;
    while(rd < end && *rd)
        ++rd;
    for( ; rd < end; ++rd)
    {
        if(*rd)
        {
            *w = *rd;
            *rd = 0;
            ++w;
            ++rd;
        }
    }
    return w;
}

#endif
