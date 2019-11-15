/*************************************************************************
 *
 * Copyright (c) 2018, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <string>
#include <iostream>

class MyClass
{
public:
    bool isValid() const
    {
        return m_member; // here try `watch m_member` and what happens when exiting the method. Solution: `watch -l m_member`.
    }

    void doSomething()
    {
        m_member = false;
    }

    bool m_member = true;
};

int main()
{
    MyClass c;
    std::cerr << c.isValid() << '\n'; // step into isValid
    c.doSomething();
    std::cerr << c.isValid() << '\n';
    return 0;
}
