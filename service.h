#pragma once
#include <string>

namespace KaizenLibrary
{
    // This class is exported from the KaizenLibrary.dll
    class Services
    {
    public:
        // Returns net with NPT filled
        std::string fillNPT(char* net);
    };
}