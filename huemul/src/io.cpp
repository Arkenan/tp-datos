
#include <limits>

#include "io.hpp"

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>


parsedStrings getLines(string filename, int numlines) {

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

  int ch, count = 0, stop = false;
  while (EOF != (ch = stream.get()) && !stop) {
    switch(ch) {
      case '\n':
          vec.push_back(line);
          line.clear();
          token.clear();
          if (++count > numlines) stop = true;
          break;
      case ',':
          line.push_back(token);
          token.clear();
          break;
      default:
          token.push_back(ch);
    }
  }

  return vec;
}


parsedStrings getLines(string filename) {
  int numlines = numeric_limits<int>::max();
  return getLines(filename, numlines);
}


void writeMatrix(mat matrix, string filename) {
  stringstream sourcestream;


  ofstream f(filename, std::ios_base::out | std::ios_base::binary);
  matrix.save(sourcestream, csv_ascii);
  filtering_streambuf<output> output;
  output.push(gzip_compressor());
  output.push(f);

  copy(sourcestream, output);

  f.close();
}
