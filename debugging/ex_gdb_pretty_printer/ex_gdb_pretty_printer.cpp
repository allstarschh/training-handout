struct Money
{
    enum Currency {
        Euro,
        Dollar
    };
    float value;
    Currency currency;
};

int main()
{
    Money money[2] = {
        {1.2f, Money::Euro},
        {3.4f, Money::Dollar},
    };

    (void)(money); // silence compiler warning
    return 0;
}
