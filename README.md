 # ARQ

> [!WARNING]
> This software is unfinished. Keep your expectations low.

Arq is a getopt-like C library for parsing command-line options.
https://linux.die.net/man/3/getopt

## amalgamate/arq.h
* It's all you need.
* It's a single-header library.
* Written in c89.

## Example
[amalgamate/example.c](amalgamate/example.c)
```
cd amalgamate 
gcc example.c -o example 
./example --help
```

## arq versus getopt

Just you know a dash '-' in the table means no.
<!-- ttps://www.tablesgenerator.com/markdown_tables -->
| Feature                                             | getopt | arq |
|-----------------------------------------------------|--------|-----|
| -- long options like --help                         | yes    | yes |
| - short options like -h                             | yes    | yes |
| option with multiple arguments                      | -      | yes |
| option without argument                             | yes    | yes |
| option required argument                            | yes    | yes |
| option with optional argument with default value    | -      | yes |
| -- positional arguments end of command-line options | yes    | yes |
| -- switch from optional to required cstr argument   | -      | yes |
| = connects token --std=c99                          | yes    | -   |
| option with array of arguments                      | -      | yes |
| bundled short options                               | yes    | yes |
| types for argument                                  | -      | yes |
| cstr_t parameter const char* to argv argument       | yes    | yes |
| uint parameter                                      | -      | yes |
| int parameter                                       | -      | yes |
| float parameter                                     | -      | yes |

## Default values for cstr_t

For cstr_t, the only supported default value is NULL.  
A string literal such as "hello world" cannot be used as the default value.  
Valid:
```
{'o', "optionalstr",  fn_optionalstr, "(cstr_t str = NULL)"},
```
Infalid:
```
{'o', "optionalstr",  fn_optionalstr, "(cstr_t str = \"hello world\")"},
```


## Positional argument

An array of cstr_t behaves like a positional argument list.  
Because it consumes all remaining values, you cannot place another argument after the cstr_t[] parameter. Any following values will be treated as part of the cstr_t array.

```
{'p', "positional_argument", fn_positionlargument, "(int number, cstr_t list[])"},

```
Example:

```
./example --positional_argument 69 hello world ok abc

Output:

-p --positional_argument
number: 69

list array_size: 4
    list[0]: hello
    list[1]: world
    list[2]: ok
    list[3]: abc
```
#### Invalid parameter order

The following does not work, because number is interpreted as part of the positional argument array:
```
{'p', "positional_argument", fn_positionlargument, "(cstr_t list[], int number)"},
```

## When to use --
Use -- only with a cstr_t argument whose default value is set to NULL.  
The -- separator tells the parser that everything after it should be treated as the argument’s value, even if it starts with --.
```
{'o', "optionalstr",  fn_optionalstr, "(cstr_t str = NULL)"},
```
```
./example --optionalstr -- --hello
-o --optionalstr
str = --hello
```

## = Is not implemented

I might implement support for this later.

In many command-line tools, you can assign an argument using =.  
However, when an cstr argument has multiple values, this becomes ambiguous.  
For example, consider this option with two integers:
```
{'i', "ints", fn_int, "(int number_0, int number_1)"},
```

You could write:
```
./example --ints=42=69
```

The parser correctly interprets this as 42 and 69. No problem.
Now, consider an option with two cstr_t values:
```
{'s', "cstrs", fn_cstrs, "(cstr_t s_0, cstr_t s_1)"},
```

You cannot use:
```
./example --cstrs=hello=world
```

because the parser interprets the first argument as ```"hello=world"``` and the second as ```"world"```, which is not what you intended.

# Todos
* conclusion 
* hex values
* comment n example positional arguments
