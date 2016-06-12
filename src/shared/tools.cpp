#include "tools.h"
#include "tinyrng.h"
#include <time.h>
#include <algorithm>

// not thread safe!
static TinyRNG rng;

void rand_seed(unsigned s)
{
    rng.init(s, 6278291);
}

const float RMULT = 1.0f / float(UINT_MAX);
bool chance(float p)
{
    return ((float)rng() * RMULT) < p;
}


void stringToLower(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), tolower);
}

void stringToUpper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), toupper);
}

bool fileHasExtension(const char *fn, const char *end)
{
    const char *dot = strrchr(fn, '.');
    if(!dot)
        return false;

    return !strcmp(dot + 1, end);
}


std::string GetDateString(void)
{
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    char str[30];
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    sprintf(str,"%-4d-%02d-%02d %02d:%02d:%02d ",aTm->tm_year+1900,aTm->tm_mon+1,aTm->tm_mday,aTm->tm_hour,aTm->tm_min,aTm->tm_sec);
    return std::string(str);
}

std::string GetTimeString(void)
{
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    char str[15];
    sprintf(str,"%02d:%02d:%02d", aTm->tm_hour,aTm->tm_min,aTm->tm_sec);
    return std::string(str);
}


void HexStrToByteArray(char *dst, const char *str)
{
    int l = strlen(str);
    char a, b;
    int hi, lo;
    // uneven digit count? treat as if there was another '0' char in front
    if(l & 1)
    {
        a = '0';
        b = *str++;
    }
    l /= 2; // if uneven, this rounds down correctly

    for(int i=0; i < l; i++)
    {
        a = *str++;
        b = *str++;

        if(isdigit(a))            hi = a - '0';
        else if(a>='A' && a<='F') hi = a - 'A' + 10;
        else if(a>='a' && a<='f') hi = a - 'a' + 10;
        else                      hi = 0;

        if(isdigit(b))            lo = b - '0';
        else if(b>='A' && b<='F') lo = b - 'A' + 10;
        else if(b>='a' && b<='f') lo = b - 'a' + 10;
        else                      lo = 0;

        dst[i] = (hi << 4) + lo;
    }
}


// from http://board.byuu.org/viewtopic.php?f=10&t=1089&start=15
bool WildcardMatch(const char *str, const char *pattern)
{
    const char *cp = 0, *mp = 0;
    while(*str && *pattern != '*')
    {
        if(*pattern != *str && *pattern != '?')
            return false;
        pattern++, str++;
    }

    while(*str)
    {
        if(*pattern == '*')
        {
            if(!*++pattern)
                return 1;
            mp = pattern;
            cp = str + 1;
        }
        else if(*pattern == *str || *pattern == '?')
        {
            ++pattern;
            ++str;
        }
        else
        {
            pattern = mp;
            str = cp++;
        }
    }

    while(*pattern++ == '*');

    return !*pattern;
}
