cmake_minimum_required(VERSION 3.10)
project(IntepreterInC)

set(This IntepreterInC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
  tokens.h
  repl/repl.h
  parser/parser.h
  parser/statements.h
  lexer/lexer.h
  eval/eval.h
  ast/ast.h
  ast/expressions.h
)

set(Sources
  tokens.c
  repl/repl.c
  parser/parser.c
  parser/statements.c
  lexer/lexer.c
  eval/eval.c
  ast/ast.c
  ast/expressions.c
)

add_library(${This}_lib STATIC ${Sources} ${Headers})
target_include_directories(${This}_lib PUBLIC src)

add_executable(${This} ${Sources} ${Headers} main.c)