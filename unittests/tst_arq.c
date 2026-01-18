#include "gtest/gtest.h"
#include "arq.h"
#include <stdio.h>
#include <stdarg.h>

int strcmp_verbose(const char* s1, const char* s2) {
        int i = 0;
        putchar('"');
        for (size_t idx = 0; idx < strlen(s1); idx++) {
                if (s1[idx] == '\n') {
                        putchar('\\');
                        putchar('n');
                        putchar('"');
                        putchar('\n');
                        if (idx + 1 < strlen(s1)) {
                                putchar('"');
                        }
                        continue;
                }
                putchar(s1[idx]);
        }
        // Compare character by character
        while (s1[i] != '\0' && s2[i] != '\0') {
                if (s1[i] != s2[i]) {
                        printf("--------------\n");
                        //printf("Strings differ at position %d: '%c' vs '%c'\n", i, s1[i], s2[i]);
                        printf("Strings differ at position %d: ", i);
                        if (s1[i] == '\n') {
                                putchar('\\');
                                putchar('n');
                        } else {
                                putchar(s1[i]);
                        }
                        printf(" vs ");
                        if (s2[i] == '\n') {
                                putchar('\\');
                                putchar('n');
                        } else {
                                putchar(s2[i]);
                        }
                        printf("\n-------------\n");
                        for (int idx = 0; idx <= i; idx++) {
                                if (s2[idx] == '\n') {
                                        putchar('\\');
                                        putchar('n');
                                }
                                putchar(s2[idx]);
                        }
                        printf("\n-------------\n");
                        return (unsigned char)s1[i] - (unsigned char)s2[i];
                }
                i++;
        }

        // If one string is longer
        if (s1[i] != s2[i]) {
                printf("--------------\n");
                printf("Strings differ at position %d: ", i);
                if (s1[i] == '\n') {
                        putchar('\\');
                        putchar('n');
                }
                printf(" vs ");
                if (s2[i] == '\n') {
                        putchar('\\');
                        putchar('n');
                }
                printf("\n--------------\n");
                for (int idx = 0; idx < i; idx++) {
                        if (s2[idx] == '\n') {
                                putchar('\\');
                                putchar('n');
                        }
                        putchar(s2[idx]);
                }
                printf("\n--------------\n");
                return (unsigned char)s1[i] - (unsigned char)s2[i];
        }

        // Strings are equal
        return 0;
}

char buffer[10000];
uint32_t const b_size = sizeof(buffer);

void fn_failure(Arq_Queue *queue) {
        (void)queue;
}
TEST(arq, verify) {
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, ")"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token ')' missing open parenthesis '('\n"
                                        "    -v --version )\n"
                                        "                 ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "("},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token '' is not a type\n"
                                        "    -v --version (\n"
                                        "                  ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(  ) sdf"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'sdf' after ')' no tokens allowed!\n"
                                        "    -v --version (  ) sdf\n"
                                        "                      ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(asdf"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'asdf' is not a type\n"
                                        "    -v --version (asdf\n"
                                        "                  ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure,  "()"},
                        {'u', "uint",    fn_failure,  "(uint32_t number = 324)"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'uint32_t' is not a type\n"
                                        "    -u --uint (uint32_t number = 324)\n"
                                        "               ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint32_t number)"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'uint32_t' is not a type\n"
                                        "    -v --version (uint32_t number)\n"
                                        "                  ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token '' is not a parameter name\n"
                                        "    -v --version (uint\n"
                                        "                      ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number = xxx"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'xxx' is not a uint literal\n"
                                        "    -v --version (uint number = xxx\n"
                                        "                                ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(int number = xxx"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'xxx' is not a int literal\n"
                                        "    -v --version (int number = xxx\n"
                                        "                               ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(float number = xxx"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'xxx' is not a float literal\n"
                                        "    -v --version (float number = xxx\n"
                                        "                                 ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(cstr_t text = xxx"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'xxx' must be NULL\n"
                                        "    -v --version (cstr_t text = xxx\n"
                                        "                                ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint float"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'float' is not a parameter name\n"
                                        "    -v --version (uint float\n"
                                        "                       ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint, number)"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token ',' is not a parameter name\n"
                                        "    -v --version (uint, number)\n"
                                        "                      ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number,)"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token ')' is not a type\n"
                                        "    -v --version (uint number,)\n"
                                        "                              ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number = sdf "},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token 'sdf' is not a uint literal\n"
                                        "    -v --version (uint number = sdf \n"
                                        "                                ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number = 99999999999999999999"},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token '99999999999999999999' positive number > UINT32_MAX 4294967295\n"
                                        "    -v --version (uint number = 99999999999999999999\n"
                                        "                                ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                Arq_Option options[] = {
                        {'v', "version", fn_failure, "(uint number[ ] = sdf "},
                };
                uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
                if (0 < arq_verify(buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                        "Option failure:\n"
                                        "    Token '[' but expected '=' or '[]' or ',' or ')'\n"
                                        "    -v --version (uint number[ ] = sdf \n"
                                        "                             ^\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
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
TEST(arq, no_parameter) {
        result[0] = 0;
        Arq_Option options[] = {
                {'v', "version", fn_no_parameter, "()"},
        };
        uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);

        {
                set(&cmd, "arq", "-v");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        /* arq_fn returns strlen of error msg */
                        /* printf("%s\n", buffer); */
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_version"));
        }
        {
                set(&cmd, "arq", "--version");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_version"));
        }
        {
                set(&cmd, "arq", "v");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                "CMD line failure:\n"
                                "    v \n"
                                "    Token 'v' is not an option\n"
                                     ), 0
                        );
                } else {
                        EXPECT_FALSE(true);
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
TEST(arq, short_option_bundeling) {
        result[0] = 0;
        Arq_Option options[] = {
                {'v', "version",   fn_bundeling_first, "()"},
                {'r', "recursion", fn_bundeling, "()"},
                {'n', "number",    fn_bundeling_number, "(uint number)"},
                {'s', "string",    fn_bundeling_string, "(cstr_t string)"},
                {'o', "optional",  fn_bundeling_number, "(uint number = 42)"},
        };
        uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "-vovnr");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(
                                        buffer,
                                       "CMD line failure:\n"
                                       "    -v o v n r \n"
                                       "    Token 'r' is not a positiv number\n"
                                       "    -n --number (uint number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "-vorn", "69");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"version number 42\nrecursion number 69\n"));
        }
        {
                set(&cmd, "arq", "-vrn", "69");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"version recursion number 69\n"));
        }
        {
                set(&cmd, "arq", "-vrn8");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"version recursion number 8\n"));
        }
        {
                set(&cmd, "arq", "-vrs", "hello!");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"version recursion string hello!\n"));
        }
        {
                set(&cmd, "arq", "-vrshello!");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"version recursion string hello!\n"));
        }
}

void fn_number32(Arq_Queue *queue) {
        uint32_t x = arq_uint(queue);
        sprintf(result, "fn_number32 %u", x);
}
void fn_number32_array(Arq_Queue *queue) {
        uint32_t const array_size = arq_array_size(queue);
        uint32_t i;
        int pos = sprintf(result, "fn_number32_array %u ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%u ", arq_uint(queue));
        }

}
TEST(arq, uint32_t) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "numberA",  fn_number32,  "(uint number)"},
                {'b', "numberB",  fn_number32,  "(uint number = 324)"},
                {'c', "numberC",  fn_number32_array,  "(uint number[])"},
        };
        uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
#if 1
        {
                set(&cmd, "arq", "--numberA", "sdf");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer, 
                                        "CMD line failure:\n"
                                        "    --numberA sdf \n"
                                        "    Token 'sdf' is not a positiv number\n"
                                        "    -a --numberA (uint number)\n"
                                ), 0
                        );
                } else {
                        EXPECT_FALSE(true);
                }
        }
#endif
        {
                set(&cmd, "arq", "--numberA", "42949672950");
                /*set(&cmd, "arq", "--numberA", "429496");*/
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 42949672950 \n"
                                        "    Token '42949672950' positive number > UINT32_MAX 4294967295\n"
                                        "    -a --numberA (uint number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "0xFFFFFFFF0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 0xFFFFFFFF0 \n"
                                        "    Token '0xFFFFFFFF0' more than 8 hex digits\n"
                                        "    -a --numberA (uint number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "123");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number32 123"));
        }
        {
                set(&cmd, "arq", "--numberA", "0xAa");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number32 170"));
        }
        {
                set(&cmd, "arq", "--numberB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number32 324"));
        }
        {
                set(&cmd, "arq", "--numberB", "0xFFFFFFFF");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number32 4294967295"));
        }
        {
                set(&cmd, "arq", "--numberC", "0xF", "40004", "1", "42");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        printf("%s\n", buffer);
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number32_array 4 15 40004 1 42 "));
        }
}

void fn_numberi32(Arq_Queue *queue) {
        int32_t x = arq_int(queue);
        sprintf(result, "fn_numberi32 %d", x);
}
void fn_numberi32_array(Arq_Queue *queue) {
        int32_t const array_size = arq_array_size(queue);
        int32_t i;
        int pos = sprintf(result, "fn_numberi32_array %d ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%d ", arq_int(queue));
        }

}
TEST(arq, int32_t) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "numberA",  fn_numberi32,  "(int number)"},
                {'b', "numberB",  fn_numberi32,  "(int number = -324)"},
                {'c', "numberC",  fn_numberi32_array,  "(int number[])"},
        };
        uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--numberA", "sdf");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer, 
                                        "CMD line failure:\n"
                                        "    --numberA sdf \n"
                                        "    Token 'sdf' is not a signed number\n"
                                        "    -a --numberA (int number)\n"
                                ), 0
                        );
                } else {
                        EXPECT_FALSE(true);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "42949672950");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 42949672950 \n"
                                        "    Token '42949672950' positive number > INT32_MAX 2147483647\n"
                                        "    -a --numberA (int number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "0xFFFFFFFF0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 0xFFFFFFFF0 \n"
                                        "    Token '0xFFFFFFFF0' more than 8 hex digits\n"
                                        "    -a --numberA (int number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "-42949672950");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA -42949672950 \n"
                                        "    Token '-42949672950' negative number < INT32_MIN -2147483648\n"
                                        "    -a --numberA (int number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "-123");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi32 -123"));
        }
        {
                set(&cmd, "arq", "--numberA", "0xAa");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi32 170"));
        }
        {
                set(&cmd, "arq", "--numberB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi32 -324"));
        }
        {
                set(&cmd, "arq", "--numberB", "0xFFFFFFFF");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi32 -1"));
        }
        {
                set(&cmd, "arq", "--numberC", "0xF", "-40004", "42", "0x80000000");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi32_array 4 15 -40004 42 -2147483648 "));
        }
}

void fn_float(Arq_Queue *queue) {
        double x = arq_float(queue);
        sprintf(result, "fn_float = %.20f\n", x);
}
void fn_float_array(Arq_Queue *queue) {
        int32_t const array_size = arq_array_size(queue);
        int32_t i;
        int pos = sprintf(result, "fn_float_array %d ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%.20f ", arq_float(queue));
        }

}
TEST(arq, hex_float) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "floatA",  fn_float,       "(float number)"},
                {'b', "floatB",  fn_float,       "(float number = 0x23.23p1)"},
                {'c', "floatC",  fn_float_array, "(float number[])"},
        };
        uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--floatA", "0xFF");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer, 
                                        "CMD line failure:\n"
                                        "    --floatA 0xFF \n"
                                        "    Token '0xFF' is not a float number\n"
                                        "    -a --floatA (float number)\n"
                                ), 0
                        );
                } else {
                        EXPECT_FALSE(true);
                }
        }
        {
                set(&cmd, "arq", "--floatA", "0xFFFFFFFFFF.AAp0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                } else {
                        EXPECT_EQ(
                                strcmp(result,
                                        "fn_float = 1099511627775.66406250000000000000\n"
                                ), 0
                        );
                }
        }
        {
                set(&cmd, "arq", "--floatA", "-0x23.23p1");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --floatA -0x23.23p1 \n"
                                        "    Token '-0x23.23p1' is not a float number\n"
                                        "    -a --floatA (float number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--floatB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_float = 70.27343750000000000000\n"));
        }
        {
                set(&cmd, "arq", "--floatB", "0x24.23p1");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_float = 72.27343750000000000000\n"));
        }
        {
                set(&cmd, "arq", "--floatC", "0x1.1p0",  "0x2.1p0",  "0xFF.1p5", "0x1.0p0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(
                        result,
                        "fn_float_array 4 "
                        "1.06250000000000000000 "
                        "2.06250000000000000000 "
                        "8162.00000000000000000000 "
                        "1.00000000000000000000 "
               ));
        }
}
TEST(arq, dec_float) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "floatA",  fn_float,       "(float number)"},
                {'b', "floatB",  fn_float,       "(float number = 2.0e2)"},
                {'c', "floatC",  fn_float_array, "(float number[])"},
        };
        uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--floatA", ".ge0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer, 
                                        "CMD line failure:\n"
                                        "    --floatA .ge0 \n"
                                        "    Token '.ge0' is not a float number\n"
                                        "    -a --floatA (float number)\n"
                                ), 0
                        );
                } else {
                        EXPECT_FALSE(true);
                }
        }
        {
                set(&cmd, "arq", "--floatB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                } else {
                        EXPECT_EQ(
                                strcmp(result, "fn_float = 200.00000000000000000000\n"), 0
                        );
                }
        }
        {
                set(&cmd, "arq", "--floatA", "-1.0e0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                } else {
                        EXPECT_EQ(
                                strcmp(result,
                                        "fn_float = -1.00000000000000000000\n"
                                ), 0
                        );
                }
        }
        {
                set(&cmd, "arq", "--floatA", "23.2re1");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --floatA 23.2re1 \n"
                                        "    Token '23.2re1' is not a float number\n"
                                        "    -a --floatA (float number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--floatB", "-2.0e-2");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_float = -0.02000000000000000042\n"));
        }
        {
                set(&cmd, "arq", "--floatB", "1.0e-3");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_float = 0.00100000000000000002\n"));
        }
        {
                set(&cmd, "arq", "--floatB", "1e-3");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_float = 0.00100000000000000002\n"));
        }
        {
                set(&cmd, "arq", 
                        "--floatC", "3.14", "0.5", "10.0", ".25", "25.",
                         "1e3", "1E3",  "3.14e-2", "0.5e+1", "25.0e0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(
                        result,
                        "fn_float_array 10 "
                           "3.14000000000000012434 "
                           "0.50000000000000000000 "
                          "10.00000000000000000000 "
                           "0.25000000000000000000 "
                          "25.00000000000000000000 "
                        "1000.00000000000000000000 "
                        "1000.00000000000000000000 "
                           "0.03140000000000000430 "
                           "5.00000000000000000000 "
                          "25.00000000000000000000 "
               ));
        }
}
