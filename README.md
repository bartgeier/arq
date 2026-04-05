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

## Hexadecimal integer values

Both `int` and `uint` arguments can be specified using **hexadecimal notation**.

- Hexadecimal values must start with the `0x` prefix
- Negative hexadecimal values are **not allowed**
- For signed integers, the hexadecimal value is interpreted according to the integer type size

### Valid examples

```
./example --uint 0xFFFFFFFF
-u --uint
number = 4294967295
```

```
./example --int 0xFFFFFFFF
-i --int
number = -1
```
### Invalid examples
Don't sign the hexadecimal values.
```
./example --int -0x01
CMD line failure:
    --int -0x01 
    Token '-0x01' is not a signed number
    -i --int (int number)
```
```
./example --int +0x01
CMD line failure:
    --int +0x01 
    Token '+0x01' is not a signed number
    -i --int (int number)
```

## Hexadecimal floating-point values

The `float` argument can also be specified using **C99 hexadecimal floating-point notation**.

The format is:

```text
[+|-]0x<hexadecimal_fraction>p<exponent>
```

Where:
- `[+|-]` is an optional sign
- `0x` indicates hexadecimal notation
- integer and fractional parts are written in **base 16**  
Each digit after the decimal point represents a negative power of 16
- `p` introduces the exponent
- the exponent is a **power of 2**, not 10

### Example
```
./example --float 0xA.E1p1
-f --float
fn_float number = 21.7578125000
```
```
./example --float -0xA.E1p1
-f --float
fn_float number = -21.7578125000
```
### How this value is interpreted

```text
 0xA.E1p1= 0xA.E1 × 2¹
```

- `0xA` = 10  
- `.E1` = 14/16 + 1/256 = 0.87890625  
- 10 + 0.87890625 = 10.87890625  
- 10.87890625 × 2¹ = 21.7578125  

So the final result is:
```text
21.7578125000
```

# Todos
* terminate cstr[] ?
* conclusion 
