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

| Feature                                             | getopt | arq |
|-----------------------------------------------------|--------|-----|
| long options like --help                            | yes    | yes |
| short options like -h                               | yes    | yes |
| option with multiple arguments                      | no     | yes |
| option without argument                             | yes    | yes |
| option required argument                            | yes    | yes |
| option with optional argument (ctsr)                | yes    | yes |
| option with optional argument with default value    | no     | yes |
| -- positional arguments end of command-line options | yes    | no  |
| -- switch from optional to required cstr argument   | no     | yes |
| option with array of arguments                      | no     | yes |
| bundled short options                               | yes    | yes |
| cstr parameter pointer to argv argument             | yes    | yes |
| uint parameter                                      | no     | yes |
| int parameter                                       | no     | yes |
| float parameter                                     | no     | yes |

