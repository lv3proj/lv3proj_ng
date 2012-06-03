#ifndef TOOLS_H
#define TOOLS_H

#include "common.h"

void stringToLower(std::string& s);
void stringToUpper(std::string s);

void rand_seed(uint32);
bool chance(float p); // 0 - 100

void HexStrToByteArray(uint8 *dst, const char *str);
bool WildcardMatch(const char *str, const char *pattern);


std::string GetDateString(void);
std::string GetTimeString(void);

#endif
