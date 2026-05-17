 # ARQ

Arq is a getopt-like C library for parsing command-line options.
https://linux.die.net/man/3/getopt

## amalgamate/arq.h
* ```arq.h``` It's all you need.
* It's a single-header library.
* Written in c89.

## Compile example
[amalgamate/example.c](amalgamate/example.c)  
Take a look at the example to understand how ARQ works, then continue reading.
```
cd amalgamate 
gcc example.c -o example 
```

# Long options

Long options use two dashes followed by a descriptive name, for example --version or --help.
Long options cannot be bundled together and must be passed separately:

```./example --version --help```

# Short options
Short options use a single dash followed by one character, for example -v or -h.  
You can combine (bundle) short options into a single flag:  
```./example -vh```  
prints version and help.

```
example version 1

help
-v --version()
-h --help()
-c --cstring(cstr_t str)
-C --cstr2(cstr_t s_A, cstr_t s_B)
-o --optionalstr(cstr_t str = NULL)
-a --cstring_array(int number, cstr_t list[])
-u --uint(uint number)
-U --uint-default(uint number = 69)
-i --int(int number)
-I --int_default(int number = -69)
-j --int_array(int numbers[])
-f --float(float number)
-F --floatdefault(float number = 5.1e1)
-t --tuple(uint first_line = 0, uint last_line = +1200)
-x --mixed(uint u_nr, int i_nr, float f_nr, cstr_t comment)

https://github.com/bartgeier/arq
```
## Argument assignment
Direct assignment for arguments without ```=``` or a space is only supported for short options.
```
./example -f3.5
-f --float
fn_float number = 3.5000000000
```
```
./example -f 3.5
-f --float
fn_float number = 3.5000000000
```
```
./example -f=3.5
-f --float
fn_float number = 3.5000000000
```

# arq versus getopt

Just you know a dash '-' in the table means **no**.
<!-- https://www.tablesgenerator.com/markdown_tables -->
| Feature                                             | getopt | arq |
|-----------------------------------------------------|--------|-----|
| -- long options like --help                         | yes    | yes |
| - short options like -h                             | yes    | yes |
| option with multiple arguments                      | -      | yes |
| option without argument                             | yes    | yes |
| option with required argument                       | yes    | yes |
| option with optional argument                       | yes    | yes |
| argument with default values                        | yes    | yes |
| -- positional arguments end of command-line options | yes    | yes |
| -- switch from optional to required cstr argument   | -      | yes |
| = cmd-line assignment operator like --std=c99       | yes    | yes |
| direct assignment like -n42                         | yes    | yes |
| option with array of arguments                      | -      | yes |
| bundled short options                               | yes    | yes |
| types for argument                                  | -      | yes |
| cstr_t parameter const char* to argv argument       | yes    | yes |
| uint parameter                                      | -      | yes |
| int parameter                                       | -      | yes |
| float parameter                                     | -      | yes |

# cstr_t

cstr_t is a cstring ```const char*``` it points into the argv list.  
For cstr_t, the only supported default value is NULL.  
A string literal such as "hello world" cannot be used as the default value.  
Valid:
```
{'o', "optionalstr",  fn_optionalstr, "(cstr_t str = NULL)"},
```
Invalid:
```
{'o', "optionalstr",  fn_optionalstr, "(cstr_t str = \"hello world\")"},
```
### -- seperator
Use -- with a cstr_t argument whose default value is set to NULL.  
The -- separator tells the parser that everything after it should be treated as the argument’s value, even if it starts with -- or -.
```
{'o', "optionalstr",  fn_optionalstr, "(cstr_t str = NULL)"},
```
```
./example --optionalstr -- --hello
-o --optionalstr
str = --hello
```


## cstr_t[]

Put a cstr_t[] always at the end of a parameter list.  
Because a cstr_t[] consumes all remaining values,  
you cannot place another parameter after the cstr_t[] parameter.  
Any following values will be treated as part of the cstr_t array.

```
{'a', "cstring_array", fn_cstringarray, "(int number, cstr_t list[])"},
{'u', "uint",          fn_uint, "(uint number)"},
```
Example:

```
./example --cstring_array 69 hello world --uint 42

Output:

-a --cstring_array
number: 69
list array_size: 2
    list[0]: hello
    list[1]: world

-u --uint
number = 42
```
#### Invalid parameter order

The following does not work, because number is interpreted as part of the positional argument array:
```
{'a', "cstring_array", fn_cstringarray, "(cstr_t list[], int number)"},
```


### -- Positional argument
Use -- to turn a cstr_t[] array into a positional argument list.

```
./example --cstring_array 69 -- hello world --uint 42

Output:

-a --cstring_array
numbere: 69
list array_size: 4
    list[0]: hello
    list[1]: world
    list[2]: --uint
    list[3]: 42
```
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
## = Command-line Assignment operator

You can assign an argument using ```=```.  
However, when an cstr option has multiple arguments, this becomes ambiguous.  
For example, consider this option with two unsigned integers parameters:
```
{'t', "tuple", fn_tuple, "(uint first_line = 0, uint last_line = +1200)"},
```

You could write:
```
./example --tuple=42=69
```
Output:
```
-t --tuple
first_line = 42
last_line = 69
```
The parser correctly interprets this as 42 and 69. No problem.  
Now, consider an option with two cstr_t parameters:
```
{'C', "cstr2", fn_cstr2, "(cstr_t s_A, cstr_t s_B)"},
```
The following does **not** work:
You cannot use:
```
./example --cstr2=hello=world
```
Error:
```
./example --cstr2=hello=world
CMD line failure:
    --cstr2 hello=world 
    Token '' is not a c string => expected an argument
    -C --cstr2 (cstr_t s_A, cstr_t s_B)
```
This happens because the parser treats hello=world as the first argument  
and consumes the ```=``` character, leaving no second argument to parse.  
Which is not what you intended.

But this works:
```
./example --cstr2=hello world
-C --cstrs
str_A = hello
str_B = world

./example --cstr2 hello world
-C --cstrs
str_A = hello
str_B = world
```
# Conclusion 
## Callback dispatching
Dispatching via a callback function pointer has the drawback that only global variables are directly accessible.  
One possible solution is to add a ```void *ctx``` parameter to the function pointer type:
```
typedef void (*function_pointer_t)(void *context, Arq_Queue *queue);
```
```
Arq_Option options[] = {
    /* short, long,  context, callback, parameter */
    { 'i',    "int", &number, fn_int,   "(int number)" },
};
```

This allows access to stack-allocated variables through the context pointer. However, it comes at the cost of type safety.

An alternative approach would be to avoid callback functions entirely and instead return an identifier that is dispatched through a switch statement.  
In that design, rather than a single ```arq_fn``` function, we would need:

1. An initialization function that returns a pointer to the ```Arq_Queue```
2. A loop function that returns the next parsed option ID

This raises additional design questions,  
such as how to distinguish command-line parsing errors from normal termination,  
and how the caller determines when the parsing loop has completed.

## Parameter string

The parameter string allows configuration of the options[] array in a way that resembles type forwarding in C++, which is quite elegant:
```
Arq_Option options[] = {
    /* short, long,          callback,      parameter */
    { 'I',    "int_default", fn_intdefault, "(int number = -69)"}, 
};
```
However, this approach requires parsing and validating the parameter string, 
which is handled by ```arq_verify(...)```. 
In effect, the parameter string behaves like a small domain-specific language (a micro-program).

While a developer could implement such logic manually within their application,
in practice what is really needed is a clean set of functions that handle command-line parsing and provide consistent error management.

## Final thoughts
As always:  
We only discover the correct design at the end of a project.
