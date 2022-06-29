#pragma once

#include <iostream>

#define PRINT_EXPR(expression)        \
  printf("[expr] %s\n", #expression); \
  expression;

#define EXCEPTION(expression)           \
  try                                   \
  {                                     \
    printf("[expr] %s\n", #expression); \
    expression;                         \
  }                                     \
  catch (std::exception & e)            \
  {                                     \
    std::cerr << e.what() << "\n";      \
  }