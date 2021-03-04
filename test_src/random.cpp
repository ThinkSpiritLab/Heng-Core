#include<iostream>
#include<random>

int main(void)
{
    std::random_device r;
    std::default_random_engine eng(r());
    std::uniform_int_distribution<int> uniform_dist(1, 6);
    
    while(true)
    {
        std::cout << uniform_dist(eng) << std::endl;
    }
}