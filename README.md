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
```
cd amalgamate 
gcc main2.c -o main2 
./main2 --help
```

# arq versus getopt

Just you know a dash '-' in the table means no.
<!-- ttps://www.tablesgenerator.com/markdown_tables -->
| Feature                                             | getopt | arq |
|-----------------------------------------------------|--------|-----|
| -- long options like --help                         | yes    | yes |
| - short options like -h                             | yes    | yes |
| option with multiple arguments                      | -      | yes |
| option without argument                             | yes    | yes |
| option required argument                            | yes    | yes |
| option with optional argument (ctsr)                | yes    | yes |
| option with optional argument with default value    | -      | yes |
| -- positional arguments end of command-line options | yes    | -   |
| -- switch from optional to required cstr argument   | -      | yes |
| = connects token --std=c99                          | yes    | -   |
| option with array of arguments                      | -      | yes |
| bundled short options                               | yes    | yes |
| types for argument                                  | -      | yes |
| cstr parameter pointer to argv argument             | yes    | yes |
| uint parameter                                      | -      | yes |
| int parameter                                       | -      | yes |
| float parameter                                     | -      | yes |

# Todos
* How to use -- 
* conclusion 
* Why not = 
