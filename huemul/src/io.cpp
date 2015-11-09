
#include "io.hpp"

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>


parsedStrings getLines(std::string filename) {

  ifstream file(filename, ios_base::binary);
  filtering_streambuf<input> inbuf;
  inbuf.push(gzip_decompressor());
  inbuf.push(file);

  istream stream(&inbuf);

  parsedStrings vec;
  string headers;
  getline(stream, headers);

  cout << "Headers: " << headers << endl;

  vector<string> line;
  string token;

  int ch;
  while (EOF != (ch = stream.get())) {
    switch(ch) {
      case '\n':
          vec.push_back(line);
          line.clear();
          token.clear();
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


void writeLines(parsedStrings lines) {
  string result_ = "";

  for (int i = 0; i < lines.size(); ++i) {
    for (int j = 0; j < lines[i].size(); ++j)
    {
      result_ += lines[i][j] + ',';
    }
    result_ += "\n";
  }
}
