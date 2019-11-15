// tools that detect the issue(s) in this code:
// * Valgrind's memcheck

#include <cstdio>

int main()
{
//--> slide
    struct data {
        const char* string;
        bool flag;
    };

    data* d = new data;

    if (d->flag)
        d->string = "Hello, World!\n";

    puts(d->string);
//<-- slide
}
