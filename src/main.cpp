#include <fstream>
#include <iostream>
#include <iterator>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/tokenizer.hpp>

#include "vendor/csv.h"


int main(int argc, char const *argv[])
{
  using namespace std;
  using namespace boost;

  ifstream file("../data/trainmin.csv.gz", ios_base::binary);
  iostreams::filtering_streambuf<iostreams::input> inbuf;
  inbuf.push(iostreams::gzip_decompressor());
  inbuf.push(file);
  // iostreams::copy(inbuf, cout);

  std::istream instream(&inbuf);

  vector<vector<string>> vec;

  std::string headers;
  std::getline(instream, headers);

  cout << "Headers: " << headers << endl;

  std::string line;
  while(std::getline(instream, line)) {
    vector<string> lineVec;
    tokenizer<escaped_list_separator<char>> tok(line);
    lineVec.assign(tok.begin(),tok.end());
    vec.push_back(lineVec);
  }

  for(auto &item : vec) {
    copy(item.begin(), item.end(), ostream_iterator<string>(cout, "|"));
    cout << endl;
  }

  return 0;
}
