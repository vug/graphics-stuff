#include "Wrapper04.h"

#include "macros.h"

#include <iostream>
#include <memory>

int main()
{
  PRINT_EXPR(auto w1 = Wrapper::makeWrapper("MyMesh"));
  PRINT_EXPR(w1->use());
  std::cout << w1->state << " reference count: " << w1.use_count() << "\n";
  {
    std::cout << "--Begin inner scope--\n";
    PRINT_EXPR(std::shared_ptr<Wrapper> w2 = w1);
    PRINT_EXPR(w2->use());
    std::cout << w2->state << " reference count: " << w2.use_count() << "\n";
    std::cout << "--End inner scope--\n";
  }
  PRINT_EXPR(w1->use());
  std::cout << w1->state << " reference count: " << w1.use_count() << "\n";

  PRINT_EXPR(auto w3 = Wrapper::makeWrapper("made-via-factory-function"));
  PRINT_EXPR(w3->use());

  // Wrapper w4{"cant-create-this"}; // inaccessible function / constructor -> can't compile
  // PRINT_EXPR(Wrapper w4 = *w3); // deleted function / copy constructor -> can't compile
  // PRINT_EXPR(Wrapper w5 = std::move(*w3)); // deleted function / move constructor -> can't compile
  PRINT_EXPR(const Wrapper &w6 = *w3); // allowed but neither copy nor move, just reference

  std::cout << "--End main scope--\n";
}