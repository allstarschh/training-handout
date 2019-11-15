// tools that detect the issue(s) in this code:
// * Valgrind's memcheck with --leak-check=full
// * Address or Leak Sanitizer
// * heaptrack

//--> slide
int *foo()
{
    char *leaked = new char[100];
    leaked[0] = '\0';
    return new int(42);
}

int main()
{
    static int *bar = foo();

    return *bar;
}
//<-- slide
