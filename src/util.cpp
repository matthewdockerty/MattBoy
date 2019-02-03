#include "util.hpp"

#include <fstream>
#include <istream>
#include <iterator>
namespace mattboy {

  bool ReadFile(const std::string& filename, std::vector<char>& result)
  {
    std::ifstream file(filename, std::ios::binary);
    if (!file)
      return false;   
    
    file.unsetf(std::ios::skipws); // don't consume newlines

    std::streampos filesize;

    file.seekg(0, std::ios::end);
    filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    result.reserve(filesize);
    result.insert(result.begin(), std::istream_iterator<char>(file),std::istream_iterator<char>());

    return true;
  }

}