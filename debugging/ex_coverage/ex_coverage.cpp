#include <cstring>
#include <iostream>

int main(int argc, char **argv)
{
    if (argc == 2 || argc == 3) {
        std::cout << "there are two or three arguments available.\n";
    }

    if (argc == 2) {
        std::cout << "2nd arg is: " << argv[1] << '\n';
    }

    if (argc == 3) {
        std::cout << "3rd arg is: " << argv[2] << '\n';
    }

    return 0;
}
