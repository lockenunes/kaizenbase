#ifndef _ENTRY_H
#define _ENTRY_H

#if defined(WIN32) || defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#include <string>

extern "C" EXPORT const char* fillNPT(char* net);

#endif