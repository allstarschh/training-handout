/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <thread>
#include <chrono>

int main()
{
//--> slide
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
//<-- slide

    return 0;
}
