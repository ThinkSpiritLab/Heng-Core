#include <iostream>
#include <string>

int main(void)
{
    std::string s;
    while(std::getline(std::cin, s))
    {
        std::cout << s << std::endl;
    }
}