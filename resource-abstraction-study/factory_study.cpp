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

void separator()
{
  std::cout << "***************************\n";
}

int main()
{
  std::set_terminate([]()
                     { std::cout << "Unhandled exception" << std::endl; std::abort(); });

  try
  {
    PRINT_EXPR(Wrapper w1);
    PRINT_EXPR(w1.use());
    separator();

    PRINT_EXPR(Wrapper w2{"abidin"});
    PRINT_EXPR(w2.use());
    separator();

    // PRINT_EXPR(Wrapper w3{w2}); // copy-construction not allowed

    PRINT_EXPR(Wrapper w3{std::move(w2)});
    EXCEPTION(w2.use());
    PRINT_EXPR(w3.use());
    separator();

    PRINT_EXPR(Wrapper w4{Wrapper{"osman"}});
    PRINT_EXPR(w4.use());
    separator();

    PRINT_EXPR(Wrapper w5{std::move(Wrapper{"haydar"})});
    PRINT_EXPR(w5.use());
    separator();

    // w1 = w2; // copy assignment not allowed

    PRINT_EXPR(Wrapper w6);
    PRINT_EXPR(w6.use());
    PRINT_EXPR(w6 = Wrapper{"riza"});
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
  catch (std::exception &e)
  {
    std::cout << "Hi!\n";
    std::cerr << e.what() << '\n';
  }
}