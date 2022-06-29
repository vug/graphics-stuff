/**
 * A study of resource wrapping classes, and their creation and usage.
 */
#include "ResourceHandler.h"
#include "macros.h"
#include "Wrapper.h"

#include <iostream>
#include <string>

Wrapper makeWrapper_1()
{
  Wrapper w;
  w.handle = 1;
  w.name = "made with makeWrapper_1()";
  return w;
}

Wrapper makeWrapper_2()
{
  return Wrapper{std::string("made with makeWrapper_2()")};
}

int main()
{
  // Wrapper w1;
  // Wrapper w2{"abidin"};
  // Wrapper w3{c1};
  // Wrapper w4{Wrapper{"osman"}};
  // Wrapper w5{std::move(Wrapper{"haydar"})};
  // w1 = w5;
  // Wrapper w6;
  // w6 = Wrapper{"riza"};

  // Wrapper w7 = makeWrapper_1();
  // w7.use();

  Wrapper w8 = makeWrapper_2();
  w8.use();
}