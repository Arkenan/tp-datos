
#ifndef HUEMUL_IO_H
#define HUEMUL_IO_H

#include <fstream>
#include <iostream>
#include <sstream>

#include <armadillo>

#include "core.hpp"

#include <boost/iostreams/filtering_streambuf.hpp>

typedef vector<vector<string>> parsedStrings;


using namespace arma;
using namespace boost::iostreams;

parsedStrings getLines(std::string filename);
parsedStrings getLines(std::string filename, int numlines);
void writeMatrix(mat matrix, string filename);

#endif
