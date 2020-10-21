#include <iostream>
#include <string>
#include <vector>
int main(int argc, char *argv[])
{
    int n = std::stoi(argv[argc - 1]);
    std::vector<std::vector<char> *> vecs;
    while(n--)
    {
        vecs.push_back(
          new std::vector<char>(1024 * 1024, 12));
        // std::cerr << n << std::endl;
    }
    return 0;
}