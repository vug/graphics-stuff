#include "Wrapper02.h"

#include "macros.h"

#include <iostream>

Wrapper makeWrapper()
{
  PRINT_EXPR(Wrapper w = {"custom-made"});
  PRINT_EXPR(w.state += " Wrapper");
  PRINT_EXPR(return w);
}

int main()
{
  PRINT_EXPR(Wrapper w1 = Wrapper("MyMesh"));
  PRINT_EXPR(w1.use());
  {
    std::cout << "--Begin inner scope--\n";
    PRINT_EXPR(Wrapper w2 = w1);
    PRINT_EXPR(w2.use());
    std::cout << "--End inner scope--\n";
  }
  EXCEPTION(w1.use());

  PRINT_EXPR(Wrapper w3 = makeWrapper());
  EXCEPTION(w3.use());

  std::cout << "--End main scope--\n";
}