#include "cgtest/cgtest.h"
#include "arq_main.h"
#include <stdio.h>
#include "arq_bool.h"
#include <stdarg.h>
char buffer[10000];
arq_uint32_t const b_size = sizeof(buffer);

void fn_failure(Arq_Queue *queue) {
        (void)queue;
}
void test_arq_verify(void) {
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, ")"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token ')' missing open parenthesis '('\n"
                                "    -v --version )\n"
                                "                 ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "("},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token '' is not a type\n"
                                "    -v --version (\n"
                                "                  ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(  ) sdf"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'sdf' after ')' no tokens allowed!\n"
                                "    -v --version (  ) sdf\n"
                                "                      ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(asdf"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'asdf' is not a type\n"
                                "    -v --version (asdf\n"
                                "                  ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure,  "()"},
                        {'u', "uint",    fn_failure,  "(uint32_t number = 324)"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'uint32_t' is not a type\n"
                                "    -u --uint (uint32_t number = 324)\n"
                                "               ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure,  "()"},
                        {'u', "uint",    fn_failure,  "(uint number = 324 asdf)"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'asdf' but expected ',' or ')'\n"
                                "    -u --uint (uint number = 324 asdf)\n"
                                "                                 ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure,  "()"},
                        {'u', "uint",    fn_failure,  "(uint number[] xxx"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'xxx' but expected ',' or ')'\n"
                                "    -u --uint (uint number[] xxx\n"
                                "                             ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint32_t number)"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'uint32_t' is not a type\n"
                                "    -v --version (uint32_t number)\n"
                                "                  ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token '' is not a parameter name\n"
                                "    -v --version (uint\n"
                                "                      ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number = xxx"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'xxx' is not a uint literal\n"
                                "    -v --version (uint number = xxx\n"
                                "                                ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(int number = xxx"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'xxx' is not a int literal\n"
                                "    -v --version (int number = xxx\n"
                                "                               ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(float number = xxx"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'xxx' is not a float literal\n"
                                "    -v --version (float number = xxx\n"
                                "                                 ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(cstr_t text = xxx"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'xxx' must be NULL\n"
                                "    -v --version (cstr_t text = xxx\n"
                                "                                ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint float"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'float' is not a parameter name\n"
                                "    -v --version (uint float\n"
                                "                       ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint, number)"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token ',' is not a parameter name\n"
                                "    -v --version (uint, number)\n"
                                "                      ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number,)"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token ')' is not a type\n"
                                "    -v --version (uint number,)\n"
                                "                              ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number = sdf "},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token 'sdf' is not a uint literal\n"
                                "    -v --version (uint number = sdf \n"
                                "                                ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number = 99999999999999999999"},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token '99999999999999999999' positive number > UINT32_MAX 4294967295\n"
                                "    -v --version (uint number = 99999999999999999999\n"
                                "                                ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number[ ] = sdf "},
                };
                arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "Option failure:\n"
                                "    Token '[' but expected '=' or '[]' or ',' or ')'\n"
                                "    -v --version (uint number[ ] = sdf \n"
                                "                             ^\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
typedef struct {
        char *argv[50];
        int argc;
} CommandLine;
CommandLine cmd = {0};

void set_argv_argc(CommandLine *cmd, char const *first, ...) {
        cmd->argc = 0;
        va_list args;
        va_start(args, first);
        char const *s = first;
        while (s != NULL) {
                cmd->argv[cmd->argc++] = (char *)s;
                s = va_arg(args, const char*);
        }
        va_end(args);
}
#define set(_cmd, ...) set_argv_argc((_cmd), __VA_ARGS__, NULL)

char result[10000] = {0};

void fn_no_parameter(Arq_Queue *queue) {
        (void) queue;
        sprintf(result, "fn_version");
}
void test_arq_no_parameter(void) {
        result[0] = 0;
        Arq_Option options[] = {
                {'v', "version", fn_no_parameter, "()"},
        };
        arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);

        {
                set(&cmd, "arq", "-v");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        /* arq_fn returns strlen of error msg */
                        /* printf("%s\n", buffer); */
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_version");
        }
        {
                set(&cmd, "arq", "--version");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_version");
        }
        {
                set(&cmd, "arq", "v");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    v \n"
                                "    Token 'v' is not an option\n"
                        );
                } else {
                        EXPECT_FALSE(ARQ_TRUE);
                }
        }
}
int pos;
void fn_bundeling_first(Arq_Queue *queue) {
        (void) queue;
        pos = 0;
        pos += sprintf(result + pos, "version ");
}
void fn_bundeling(Arq_Queue *queue) {
        (void) queue;
        pos += sprintf(result + pos, "recursion ");
}
void fn_bundeling_number(Arq_Queue *queue) {
        (void) queue;
        pos += sprintf(result + pos, "number %u\n", arq_uint(queue));
}
void fn_bundeling_string(Arq_Queue *queue) {
        (void) queue;
        pos += sprintf(result + pos, "string %s\n", arq_cstr_t(queue));
}
void test_arq_short_option_bundeling(void) {
        result[0] = 0;
        Arq_Option options[] = {
                {'v', "version",   fn_bundeling_first, "()"},
                {'r', "recursion", fn_bundeling, "()"},
                {'n', "number",    fn_bundeling_number, "(uint number)"},
                {'s', "string",    fn_bundeling_string, "(cstr_t string)"},
                {'o', "optional",  fn_bundeling_number, "(uint number = 42)"},
        };
        arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "-vovnr");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                               "CMD line failure:\n"
                               "    -v o v n r \n"
                               "    Token 'r' is not a positiv number\n"
                               "    -n --number (uint number)\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                set(&cmd, "arq", "-vorn", "69");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"version number 42\nrecursion number 69\n");
        }
        {
                set(&cmd, "arq", "-vrn", "69");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"version recursion number 69\n");
        }
        {
                set(&cmd, "arq", "-vrn8");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"version recursion number 8\n");
        }
        {
                set(&cmd, "arq", "-vrs", "hello!");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"version recursion string hello!\n");
        }
        {
                set(&cmd, "arq", "-vrshello!");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"version recursion string hello!\n");
        }
}

void fn_assign_version(Arq_Queue *queue) {
        (void) queue;
        pos += sprintf(result + pos, "version ");
}
void fn_assign_number(Arq_Queue *queue) {
        (void) queue;
        pos += sprintf(result + pos, "number %u\n", arq_uint(queue));
}
void fn_assign_3number(Arq_Queue *queue) {
        (void) queue;
        arq_uint32_t n1 = arq_uint(queue);
        arq_uint32_t n2 = arq_uint(queue);
        arq_uint32_t n3 = arq_uint(queue);
        pos += sprintf(result + pos, "number %u %u %u\n", n1, n2, n3);
}
void fn_assign_mixed(Arq_Queue *queue) {
        arq_uint32_t u_nr = arq_uint(queue);
        arq_int32_t i_nr = arq_int(queue);
        double f_nr = arq_float(queue);
        char const *comment = arq_cstr_t(queue);
        pos += sprintf(result + pos, "u_nr = %u\ni_nr = %d\nf_nr = %f\ncomment = %s\n", u_nr, i_nr, f_nr, comment);
}
void fn_assign_string(Arq_Queue *queue) {
        const char *s1 = arq_cstr_t(queue);
        const char *s2 = arq_cstr_t(queue); 
        pos += sprintf(result + pos, "s1 = %s\ns2 = %s\n", s1, s2);
}
void test_arq_assignment_operator(void) {
        result[0] = 0;
        Arq_Option options[] = {
                {'v', "version",   fn_assign_version, "()"},
                {'n', "number",    fn_assign_number, "(uint number)"},
                {'m', "numbers",   fn_assign_3number, "(uint n1, uint n2, uint n3)"},
                {'x', "mixed",     fn_assign_mixed, "(uint u_nr, int i_nr, float f_nr, cstr_t comment)"},
                {'s', "string",    fn_assign_string, "(cstr_t s1, cstr_t s2)"},
        };
        arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                pos = 0;
                set(&cmd, "arq", "--number=8");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"number 8\n");
        }
        {
                pos = 0;
                set(&cmd, "arq", "-n=8");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"number 8\n");
        }
        {
                pos = 0;
                set(&cmd, "arq", "-vn=8");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"version number 8\n");
        }
        {
                pos = 0;
                set(&cmd, "arq", "--number=",  "8");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    --number= \n"
                                "    Token '--number=' is not an option\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                pos = 0;
                set(&cmd, "arq", "-n=", "8");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    -n = \n"
                                "    Token '=' is not a positiv number\n"
                                "    -n --number (uint number)\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                pos = 0;
                set(&cmd, "arq", "-vn=", "8");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    -v n = \n"
                                "    Token '=' is not a positiv number\n"
                                "    -n --number (uint number)\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                pos = 0;
                set(&cmd, "arq", "--numbers=8=7=6=");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    --numbers 8 7 6 = \n"
                                "    Token '=' is not an option\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }

        }
        {
                pos = 0;
                set(&cmd, "arq", "-x=8=7=6.0=hello");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_TRUE(0 == strcmp(
                        result,
                        "u_nr = 8\n"
                        "i_nr = 7\n"
                        "f_nr = 6.000000\n"
                        "comment = hello\n"
                        )
                );
        }
        {
                pos = 0;
                set(&cmd, "arq", "--string", "hello", "world");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_TRUE(0 == strcmp(
                        result,
                        "s1 = hello\n"
                        "s2 = world\n"
                        )
                );
        }
        {
                pos = 0;
                set(&cmd, "arq", "--string=hello", "world");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_TRUE(0 == strcmp(
                        result,
                        "s1 = hello\n"
                        "s2 = world\n"
                        )
                );
        }
        {
                pos = 0;
                set(&cmd, "arq", "--string=hello=world");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    --string hello=world \n"
                                "    Token '' is not a c string => expected an argument\n"
                                "    -s --string (cstr_t s1, cstr_t s2)\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                pos = 0;
                set(&cmd, "arq", "--fleet=3");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    --fleet \n"
                                "    Token '--fleet' unknown long option \n"   
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
}

void fn_number32(Arq_Queue *queue) {
        arq_uint32_t x = arq_uint(queue);
        sprintf(result, "fn_number32 %u", x);
}
void fn_number32_array(Arq_Queue *queue) {
        arq_uint32_t const array_size = arq_array_size(queue);
        arq_uint32_t i;
        int pos = sprintf(result, "fn_number32_array %u ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%u ", arq_uint(queue));
        }

}
void test_arq_arq_uint32_t(void) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "numberA",  fn_number32,  "(uint number)"},
                {'b', "numberB",  fn_number32,  "(uint number = 324)"},
                {'c', "numberC",  fn_number32_array,  "(uint number[])"},
        };
        arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--numberA", "sdf");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer, 
                                "CMD line failure:\n"
                                "    --numberA sdf \n"
                                "    Token 'sdf' is not a positiv number\n"
                                "    -a --numberA (uint number)\n"
                        );
                } else {
                        EXPECT_FALSE(ARQ_TRUE);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "42949672950");
                /*set(&cmd, "arq", "--numberA", "429496");*/
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    --numberA 42949672950 \n"
                                "    Token '42949672950' positive number > UINT32_MAX 4294967295\n"
                                "    -a --numberA (uint number)\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "0xFFFFFFFF0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    --numberA 0xFFFFFFFF0 \n"
                                "    Token '0xFFFFFFFF0' more than 8 hex digits\n"
                                "    -a --numberA (uint number)\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "123");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_number32 123");
        }
        {
                set(&cmd, "arq", "--numberA", "0xAa");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_number32 170");
        }
        {
                set(&cmd, "arq", "--numberB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_number32 324");
        }
        {
                set(&cmd, "arq", "--numberB", "0xFFFFFFFF");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_number32 4294967295");
        }
        {
                set(&cmd, "arq", "--numberC", "0xF", "40004", "1", "42");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        printf("%s\n", buffer);
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_number32_array 4 15 40004 1 42 ");
        }
}

void fn_numberi32(Arq_Queue *queue) {
        arq_int32_t x = arq_int(queue);
        sprintf(result, "fn_numberi32 %d", x);
}
void fn_numberi32_array(Arq_Queue *queue) {
        arq_int32_t const array_size = arq_array_size(queue);
        arq_int32_t i;
        int pos = sprintf(result, "fn_numberi32_array %d ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%d ", arq_int(queue));
        }

}
void test_arq_int32_t(void) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "numberA",  fn_numberi32,  "(int number)"},
                {'b', "numberB",  fn_numberi32,  "(int number = -324)"},
                {'c', "numberC",  fn_numberi32_array,  "(int number[])"},
        };
        arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--numberA", "sdf");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer, 
                                "CMD line failure:\n"
                                "    --numberA sdf \n"
                                "    Token 'sdf' is not a signed number\n"
                                "    -a --numberA (int number)\n"
                        );
                } else {
                        EXPECT_FALSE(ARQ_TRUE);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "42949672950");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    --numberA 42949672950 \n"
                                "    Token '42949672950' positive number > INT32_MAX 2147483647\n"
                                "    -a --numberA (int number)\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "0xFFFFFFFF0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    --numberA 0xFFFFFFFF0 \n"
                                "    Token '0xFFFFFFFF0' more than 8 hex digits\n"
                                "    -a --numberA (int number)\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "-42949672950");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer,
                                "CMD line failure:\n"
                                "    --numberA -42949672950 \n"
                                "    Token '-42949672950' negative number < INT32_MIN -2147483648\n"
                                "    -a --numberA (int number)\n"
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "-123");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_numberi32 -123");
        }
        {
                set(&cmd, "arq", "--numberA", "0xAa");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_numberi32 170");
        }
        {
                set(&cmd, "arq", "--numberB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_numberi32 -324");
        }
        {
                set(&cmd, "arq", "--numberB", "0xFFFFFFFF");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_numberi32 -1");
        }
        {
                set(&cmd, "arq", "--numberC", "0xF", "-40004", "42", "0x80000000");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_numberi32_array 4 15 -40004 42 -2147483648 ");
        }
}

void fn_float(Arq_Queue *queue) {
        double x = arq_float(queue);
        sprintf(result, "fn_float = %.20f\n", x);
}
void fn_float_array(Arq_Queue *queue) {
        arq_int32_t const array_size = arq_array_size(queue);
        arq_int32_t i;
        int pos = sprintf(result, "fn_float_array %d\n", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%.20f\n", arq_float(queue));
        }

}

void test_arq_hex_float(void) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "floatA",  fn_float,       "(float number)"},
                {'b', "floatB",  fn_float,       "(float number = 0x23.23p1)"},
                {'c', "floatC",  fn_float_array, "(float number[])"},
                {'d', "floatD",  fn_float,       "(float number = -0x23.23p1)"},
                {'e', "floatE",  fn_float,       "(float number = +0x23.23p1)"},
        };
        arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--floatA", "0xFF");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                buffer, 
                                "CMD line failure:\n"
                                "    --floatA 0xFF \n"
                                "    Token '0xFF' is not a float number\n"
                                "    -a --floatA (float number)\n"
                        );
                } else {
                        EXPECT_FALSE(ARQ_TRUE);
                }
        }
        {
                set(&cmd, "arq", "--floatA", "0xFFFFFFFFFF.AAp0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                } else {
                        EXPECT_EQ_STR(result, "fn_float = 1099511627775.66406250000000000000\n");
                }
        }
        {
                set(&cmd, "arq", "--floatA", "-0x23.23p1");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                } else {
                        EXPECT_EQ_STR(result,"fn_float = -70.27343750000000000000\n");
                }
        }
        {
                set(&cmd, "arq", "--floatB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_float = 70.27343750000000000000\n");
        }
        {
                set(&cmd, "arq", "--floatB", "0x24.23p1");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_float = 72.27343750000000000000\n");
        }
        {
                set(&cmd, "arq", "--floatC", "0x1.1p0",  "-0x2.1p0",  "+0xFF.1p5", "0x1.0p0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(
                        "fn_float_array 4\n"
                        "1.06250000000000000000\n"
                        "-2.06250000000000000000\n"
                        "8162.00000000000000000000\n"
                        "1.00000000000000000000\n",
                        result
               );
        }
        {
                set(&cmd, "arq", "--floatD");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_float = -70.27343750000000000000\n");
        }
        {
                set(&cmd, "arq", "--floatE");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_float = 70.27343750000000000000\n");
        }
}
void test_arq_dec_float(void) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "floatA",  fn_float,       "(float number)"},
                {'b', "floatB",  fn_float,       "(float number = 2.0e2)"},
                {'c', "floatC",  fn_float_array, "(float number[])"},
        };
        arq_uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--floatA", ".ge0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                "CMD line failure:\n"
                                "    --floatA .ge0 \n"
                                "    Token '.ge0' is not a float number\n"
                                "    -a --floatA (float number)\n",
                                buffer
                        );
                } else {
                        EXPECT_FALSE(ARQ_TRUE);
                }
        }
        {
                set(&cmd, "arq", "--floatB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                } else {
                        EXPECT_EQ_STR(result, "fn_float = 200.00000000000000000000\n");
                }
        }
        {
                set(&cmd, "arq", "--floatA", ".");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                                "CMD line failure:\n"
                                "    --floatA . \n"
                                "    Token '.' is not a float number\n"
                                "    -a --floatA (float number)\n",
                                buffer
                        );
                } else {
                        EXPECT_TRUE(ARQ_FALSE);
                }
        }
        {
                set(&cmd, "arq", "--floatA", "-1.0e0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                } else {
                        EXPECT_EQ_STR(
                                result,
                                "fn_float = -1.00000000000000000000\n"
                        );
                }
        }
        {
                set(&cmd, "arq", "--floatA", "23.2re1");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ_STR(
                               "CMD line failure:\n"
                               "    --floatA 23.2 re1 \n"
                               "    Token 're1' is not an option\n",
                                buffer
                        );
                } else {
                        ASSERT_TRUE(ARQ_FALSE);
                }
        }
        {
                set(&cmd, "arq", "--floatB", "-2.0e-2");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result, "fn_float = -0.02000000000000000042\n");
        }
        {
                set(&cmd, "arq", "--floatB", "1.0e-3");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_float = 0.00100000000000000002\n");
        }
        {
                set(&cmd, "arq", "--floatB", "1e-3");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(result,"fn_float = 0.00100000000000000002\n");
        }
        {
                set(&cmd, "arq", 
                        "--floatC", "3.14", "0.5", "10.0", ".25", "25.",
                         "1e3", "1E3",  "3.14e-2", "0.5e+1", "25.0e0", "1e2", "1.e2");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        printf("%s\n", buffer);
                        ASSERT_TRUE(ARQ_FALSE);
                }
                EXPECT_EQ_STR(
                        "fn_float_array 12\n"
                           "3.14000000000000012434\n"
                           "0.50000000000000000000\n"
                          "10.00000000000000000000\n"
                           "0.25000000000000000000\n"
                          "25.00000000000000000000\n"
                        "1000.00000000000000000000\n"
                        "1000.00000000000000000000\n"
                           "0.03140000000000000430\n"
                           "5.00000000000000000000\n"
                          "25.00000000000000000000\n"
                         "100.00000000000000000000\n"
                         "100.00000000000000000000\n",
                          result
               );
        }
}
