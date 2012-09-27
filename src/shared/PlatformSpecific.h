#ifndef PLATFORM_SPECIFIC_H
#define PLATFORM_SPECIFIC_H

void TriggerBreakpoint();

#include "common.h"

std::string GetProgramDir(void);
bool SetWorkingDir(std::string d);
std::string GetWorkingDir(void);
uint32 GetConsoleWidth(void);
void MsgBox(const char *s);


#endif
