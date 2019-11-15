//--> slide
enum E { A, B, C, NumElements };
static const char * strings[] = { "A", "B", "C" };

static_assert( sizeof strings / sizeof *strings == NumElements,
               "Oops, 'strings' and 'E' don't have the same number of elements" );
//<-- slide

int main()
{
    (void)strings;

    return 0;
}
