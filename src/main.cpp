#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>


int main(int argc, char const *argv[])
{
  using namespace std;
  using namespace boost::iostreams;

  ifstream file("../data/trainmin.csv.gz", ios_base::binary);
  if (file.good()) {
  filtering_streambuf<input> in;
  in.push(gzip_decompressor());
  in.push(file);
  boost::iostreams::copy(in, cout);
  return 0;
  }
}
