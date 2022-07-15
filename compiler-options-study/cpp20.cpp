#include <iostream>
#include <map>

int main()
{
  std::map<int, char> example = {{1, 'a'}, {2, 'b'}};
  std::cout << example.contains(1) << " " << example.contains(3) << std::endl;
}