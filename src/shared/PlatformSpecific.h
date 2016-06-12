#ifndef PLATFORM_SPECIFIC_H
#define PLATFORM_SPECIFIC_H

#include <string>

void TriggerBreakpoint();
bool SetWorkingDir(const char *d);
std::string GetWorkingDir(void);
std::string GetProgramDir(void);
unsigned GetConsoleWidth(void);
void MsgBox(const char *s);


#endif
