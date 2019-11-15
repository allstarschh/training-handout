// tools that detect the issue(s) in this code:
// * Undefined Sanitizer

#include <limits>
#include <iostream>

template <typename T> void display(T a) { std::cout << a << std::endl; }

int main(int argc, char** /*argv*/)
{
//--> slide
    int foo = 23 << (argc * 32);

    int bar = std::numeric_limits<int>::min() * argc;
    unsigned int asdf = -bar;
//<-- slide

    display(foo);
    display(bar);
    display(asdf);
    return 0;
}
