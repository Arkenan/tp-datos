
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

/**
 * Descomprime y parsea un archivo separado por comas comprimido con gzip.
 * Retorna un vector de lineas, que a su vez son vectores de strings, donde
 * cada linea es una fila del archivo, y cada string es un elemento.
 * Descarta la primera linea.
 */
parsedStrings getLines(std::string filename);

/**
 * Igual que getLines, solo que toma las primeras @numlines filas del archivo.
 */
parsedStrings getLines(std::string filename, int numlines);

/**
 * Escribe una matriz de armadillo a disco en formato csv, comprimido con gz.
 */
void writeMatrix(mat matrix, map<string, int> labelsMap, string filename);

#endif
