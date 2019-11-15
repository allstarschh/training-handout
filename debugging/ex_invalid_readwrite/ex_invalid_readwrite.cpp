// tools that detect the issue(s) in this code:
// * Address Sanitizer
// * Valgrind's memcheck

int main()
{
//--> slide
    int *i = new int[2];
    int read = i[2];
    i[3] = 1; // and write
//<-- slide

    (void) read; // silence compiler warning
    return 0;
}
