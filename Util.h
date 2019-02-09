#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <vector>

namespace mattboy {

	bool ReadFile(const std::wstring& filename, std::vector<char>& result);

}

#endif

