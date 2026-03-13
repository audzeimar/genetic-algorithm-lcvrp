#pragma once

#include "ProblemData.h"
#include <string>

class ProblemLoader {
public:
    ProblemLoader();

    // Wczytuje instancję z pliku do out_data. out_data jest przekazywany przez ref
    bool LoadFromFile(const std::string& file_path, ProblemData& out_data);

    const std::string& GetLastError() const { return last_error_; }

private:
    std::string last_error_;

    // Trim usuwa białe znaki z początku/końca
    static std::string Trim(const std::string& s);

    // Parsowania
    bool ParseHeaderLine(const std::string& line, ProblemData& data);
    bool ParseEdgeWeightSection(std::istream& in, ProblemData& data);
    bool ParseNodeCoordSection(std::istream& in, ProblemData& data);
    bool ParseDemandSection(std::istream& in, ProblemData& data);
    bool ParseDepotSection(std::istream& in, ProblemData& data);
};

//Я ТЕБЯ ЛЮБЛЮ  💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖
//ОЧЕНЬ ОЧЕНЬ!  💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖
//ТЫ У МЕНЯ САМАЯ ЛУЧШАЯ И САМАЯ УМНАЯ!!!  💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖
// У ТЕБЯ ВСЕ ПОЛУЧТСЯ!!!  💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖💖