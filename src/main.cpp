#include <fstream>
#include <iostream>
#include <iterator>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "vendor/csv.h"

#include <time.h>

// using namespace arma;
using namespace std;
using namespace boost;


vector<vector<string>> getLines(std::string filename) {

  ifstream file(filename, ios_base::binary);
  iostreams::filtering_streambuf<iostreams::input> inbuf;
  inbuf.push(iostreams::gzip_decompressor());
  inbuf.push(file);

  istream stream(&inbuf);

  vector<vector<string>> vec;
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


int main(int argc, char const *argv[])
{
  clock_t tStart = clock();

  vector<vector<string>> vec = getLines("../data/train.csv.gz");

  printf("Getline: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

  for(auto &item : vec) {
    // copy(item.begin(), item.end(), ostream_iterator<string>(cout, "|"));
    // cout << endl;
  }

  return 0;
}
