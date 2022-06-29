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
  w.name = "made with makeW_1";
  return w;
}

Wrapper makeWrapper_2()
{
  return Wrapper{std::string("made with makeC_2")};
}

int main()
{
  Wrapper c1;
  Wrapper c2{"abidin"};
  Wrapper c3{c1};
  Wrapper c4{Wrapper{"osman"}};
  Wrapper c5{std::move(Wrapper{"haydar"})};
  c1 = c5;
  Wrapper c6;
  c6 = Wrapper{"riza"};
}