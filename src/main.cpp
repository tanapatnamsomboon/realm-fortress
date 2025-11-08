/**
 * @file main.cpp
 * @brief
 * @date 11/8/2025
 */

#include "core/Application.h"
#include <iostream>

int main()
{
    try
    {
        RealmFortress::Application app;
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Fatal] " << e.what() << std::endl;
    }
    return 0;
}
