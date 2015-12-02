
#include <limits>

#include "io.hpp"

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>


parsedStrings getLines(string filename, int numlines, bool test) {

  ifstream file(filename, ios_base::binary);
  filtering_streambuf<input> inbuf;
  inbuf.push(gzip_decompressor());
  inbuf.push(file);

  istream stream(&inbuf);

  parsedStrings vec;
  string headers;
  getline(stream, headers);

  vector<string> line;
  string token;

  int ch, count = 0, stop = false, colNum = 0;
  while (EOF != (ch = stream.get()) && !stop) {
    switch(ch) {
      case '\n':
          line.push_back(token);
          vec.push_back(line);
          line.clear();
          token.clear();
          colNum = 0;
          if (++count > numlines) stop = true;
          break;
      case ',':
          // Se ignoran la columna id para el set de test y las columnas
          // Descript y Resolution para el set de train.
          if ((test && colNum != 0) ||
              (!test && colNum != 2 && colNum != 5)) {
            line.push_back(token);
          }
          token.clear();
          colNum++;
          break;
      case '\"':
          // Si el campo comienza con ", se ignora.
          while ((ch = stream.get()) != '\"');
          break;
      default:
          token.push_back(ch);
    }
  }

  return vec;
}


parsedStrings getLines(string filename, bool test) {
  int numlines = numeric_limits<int>::max();
  return getLines(filename, numlines, test);
}


void writeMatrix(fmat matrix, map<string, int> labelsMap, string filename) {
  stringstream sourcestream;

  // Agrego headers
  bool first = true;
  sourcestream << "Id";
  for(auto imap: labelsMap) {
    sourcestream << ',' << imap.first;
  }
  sourcestream << endl;

  int rows = matrix.n_rows;
  int cols = matrix.n_cols;

  for (int i = 0; i < rows; ++i) {
    // Agrego ids
    sourcestream << i;
    for (int j = 0; j < cols; ++j) {
      sourcestream << ',' << matrix(i, j);
    }
    sourcestream << endl;
  }

  ofstream f(filename, std::ios_base::out | std::ios_base::binary);
  filtering_streambuf<output> output;
  output.push(gzip_compressor());
  output.push(f);

  copy(sourcestream, output);

  f.close();
}
