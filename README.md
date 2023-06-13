# Monkey Interpreter implemented in C

This is a C implementation of the Monkey programming language interpreter from the book Writing An Interpreter In Go by Thorsten Ball. I made this project in order to learn more about how to write good C code, and also to learn more about how programming languages work. I encourage others who are trying to write their own language interpreter in C to give this a look!

Book can be purchased here: https://interpreterbook.com/

I may expand on this implementation in the future, but for now I'm happy with what I've done and have learned a ton making this. The next thing I'd like to polish is the garbage collector, since right now theres no garbage collection happening (I tried making one and it was broken :|). 

# How to Use

The CMake file will create two executables: monkey.exe and run_tests.exe  
monkey.exe will start a REPL if there are no arguments, and will run a program file if a path is provided (monkey.exe [filepath]).  

# Language

The language supports the following datatypes: int, bool, string, array, map, function.  
It supports if/else statements, function calls, array and map indexing, builtin functions, let statements, and return statements.  
Also has builtin functions: len, first, last, rest, push, print.  
  
Here's an example program that does a good job of showing off all the features of the language. It contains a monkey implementation of map/reduce and uses these functions to convert strings to ints, multiply an array by 1000, calculate the sum of the array, and then print a string based on the result.

```
let map = fn(arr, f) {
  let iter = fn(arr, accumulated) {
    if (len(arr) == 0) {
      accumulated
    } else {
      iter(rest(arr), push(accumulated, f(first(arr))));
    }
  };

  iter(arr, []);
};

let reduce = fn(arr, initial, f) {
  let iter = fn(arr, result) {
    if (len(arr) == 0) {
      result
    } else {
      iter(rest(arr), f(result, first(arr)));
    }
  };

  iter(arr, initial);
};

let str_list = ["one", "two", "three", "four"];
let str_to_int_map = {"one": 1, "two": 2, "three": 3, "four": 4};

let get_int = fn(in_str) {
    return str_to_int_map[in_str];
};

let int_list = map(str_list, fn(x) {get_int(x)});

let list_times_thousand = map(int_list, fn(x) {x * 1000});
print(list_times_thousand);

let sum_list = reduce(list_times_thousand, 0, fn(r, x) {r + x});

if (sum_list > 9999) {
    print("This number is massive");
}
else {
    if (sum_list > 1000) {
        print("This number is decent.");
    }
    else {
        print("This number is small.");
    }
}
```
Output:
```
[1000, 2000, 3000, 4000]
"This number is massive"
```
