#include "net.h"
#include <iostream>
#include "smile.h"
#include "service.h"

extern "C" EXPORT const char* fillNPT(char* net)
{
	KaizenLibrary::Services service;
	std::string str = service.fillNPT(net);
	char * cstr = new char[str.length()+1];
	std::strcpy(cstr, str.c_str());
	
	return cstr;

}