// Note: these should be set or unset by the build system, depending on
//       whether the build is a debug or release build.

//
// Enable assert()
//
#undef NDEBUG

//
// Enable assertions in the standard library
//

// GCC's libstdc++
#define _GLIBCXX_DEBUG 1

// clang's libc++
#define _LIBCPP_DEBUG 1

//
// Also useful to have: let the address sanitizer know when accessing a vector
// beyond its size, but still inside its capacity
//
#define _GLIBCXX_SANITIZE_VECTOR 1

// Include headers *after* setting the above defines
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <iostream>

//--> slide
void printPersonDetails(const std::vector<std::string> &names,
                        const std::vector<int> &ages)
{
    assert(names.size() == ages.size());
    for (size_t i = 0; i < names.size(); i++) {
        std::cout << "Person " << i << ": Name " << names[i]
                  << ", Age " << ages[i] << std::endl;
    }
}
//<-- slide

int main()
{
    const std::vector<std::string> names = { "Franz", "Hubert", "Josef", "Herbert" };
    const std::vector<int> ages = { 57, 43, 71, 88, 21 };

    // Will trigger our assert() for the precondition
    printPersonDetails(names, ages);

    // Will trigger assert in the STL
    std::cout << "Last Name: " << names[names.size()] << std::endl;

    // When the environment variable MALLOC_CHECK_ is set to 3 on Linux with glibc,
    // this memory corruption will be detected
    char * const data = static_cast<char*>(malloc(4));
    std::strcpy(data, "FAIL");
    free(data);
}
