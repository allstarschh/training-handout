/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <memory>
#include <vector>

struct Car;
struct Person
{
    std::vector<std::shared_ptr<Car>> cars;
};

struct Car
{
    Car(std::shared_ptr<Person> owner)
        : owner(std::move(owner))
    {}

    std::shared_ptr<Person> owner;
};

int main()
{
    auto alice = std::make_shared<Person>();

    auto convertible = std::make_shared<Car>(alice);
    auto suv = std::make_shared<Car>(alice);

    alice->cars = {std::move(convertible), std::move(suv)};

    return 0;
}
