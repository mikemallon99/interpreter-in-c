cmake_minimum_required(VERSION 3.10)
project(IntepreterInC)

set(This IntepreterInC)

set(CMAKE_C_STANDARD 99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
  src/tokens.h
  src/repl/repl.h
  src/parser/parser.h
  src/parser/statements.h
  src/lexer/lexer.h
  src/eval/eval.h
  src/eval/builtins.h
  src/ast/ast.h
  src/ast/expressions.h
)

set(Sources
  src/tokens.c
  src/repl/repl.c
  src/parser/parser.c
  src/parser/statements.c
  src/lexer/lexer.c
  src/eval/eval.c
  src/eval/builtins.c
  src/ast/ast.c
  src/ast/expressions.c
)

set(Tests
  tests/all_tests.c
)

add_library(${This}_lib STATIC ${Sources} ${Headers})
target_include_directories(${This}_lib PUBLIC src)

add_executable(${This} ${Sources} ${Headers} src/main.c)
add_executable(run_tests ${Tests} ${Sources} ${Headers})