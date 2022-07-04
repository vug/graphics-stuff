#include "ResourceHandler.h"
#include "macros.h"
#include "Wrapper03.h"

#include <iostream>
#include <string>

Wrapper makeWrapper_1()
{
  Wrapper w;
  w.state = "made with makeWrapper_1()";
  return w;
}

Wrapper makeWrapper_2()
{
  return Wrapper{std::string("made with makeWrapper_2()")};
}

void separator()
{
  std::cout << "***************************\n";
}

int main()
{
  PRINT_EXPR(Wrapper w1);
  PRINT_EXPR(Wrapper w2{"MyMesh"});
  separator();

  // w1 = w2; // copy assignment not allowed

  // PRINT_EXPR(Wrapper w3{w2}); // copy-construction not allowed

  PRINT_EXPR(Wrapper w3{std::move(w2)});
  EXCEPTION(w2.use());
  PRINT_EXPR(w3.use());
  separator();

  PRINT_EXPR(Wrapper w4{Wrapper{"YourMesh"}});
  PRINT_EXPR(w4.use());
  separator();

  PRINT_EXPR(Wrapper w5{std::move(Wrapper{"HerMesh"})});
  PRINT_EXPR(w5.use());
  separator();

  PRINT_EXPR(Wrapper w6);
  PRINT_EXPR(w6.use());
  PRINT_EXPR(w6 = Wrapper{"HisMesh"});
  PRINT_EXPR(w6.use());
  separator();

  PRINT_EXPR(Wrapper w7 = makeWrapper_1());
  PRINT_EXPR(w7.use());
  separator();

  PRINT_EXPR(Wrapper w8 = makeWrapper_2());
  PRINT_EXPR(w8.use());
  separator();

  std::cout << "End of scope\n";
}