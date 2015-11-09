
#ifndef HUEMUL_IO_H
#define HUEMUL_IO_H

#include <fstream>
#include <iostream>
#include <sstream>

#include "core.hpp"

#include <boost/iostreams/filtering_streambuf.hpp>
typedef vector<vector<string>> parsedStrings;


using namespace boost::iostreams;

parsedStrings getLines(std::string filename);
void writeLines(parsedStrings lines);

#endif
