#include "Wrapper01.h"

#include "macros.h"

#include <iostream>

int main()
{
  PRINT_EXPR(Wrapper w1 = Wrapper("MyMesh"));
  PRINT_EXPR(w1.use());
  {
    PRINT_EXPR(Wrapper w2 = w1);
    PRINT_EXPR(w2.use());
    std::cout << "--End of inner scope--\n";
  }
  EXCEPTION(w1.use());
  std::cout << "--End of main scope--\n";
}