#include "gtest/gtest.h"
#include "arq.h"
#include <stdio.h>
#include <stdarg.h>

int strcmp_verbose(const char* s1, const char* s2) {
        int i = 0;

        // Compare character by character
        while (s1[i] != '\0' && s2[i] != '\0') {
                if (s1[i] != s2[i]) {
                        printf("Strings differ at position %d: '%c' vs '%c'\n", i, s1[i], s2[i]);
                        printf("--------------\n");
                        for (int idx = 0; idx < i; idx++) {
                                putchar(s1[idx]);
                        }
                        printf("\n--------------\n");
                        return (unsigned char)s1[i] - (unsigned char)s2[i];
                }
                i++;
        }

        // If one string is longer
        if (s1[i] != s2[i]) {
                printf("Strings differ at position %d: '%c' vs '%c'\n", i, s1[i], s2[i]);
                printf("--------------\n");
                for (int idx = 0; idx < i; idx++) {
                        putchar(s1[idx]);
                }
                printf("\n--------------\n");
                return (unsigned char)s1[i] - (unsigned char)s2[i];
        }

        // Strings are equal
        return 0;
}

char buffer[10000];
uint32_t const b_size = sizeof(buffer);

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

char result[1000] = {0};

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
                                "    'v' is not an option\n"
                                     ), 0
                        );
                } else {
                        EXPECT_FALSE(true);
                }
        }
}

void fn_number32(Arq_Queue *queue) {
        uint32_t x = arq_uint32_t(queue);
        sprintf(result, "fn_number32 %u", x);
}

void fn_number32_array(Arq_Queue *queue) {
        uint32_t const array_size = arq_array_size(queue);
        uint32_t i;
        int pos = sprintf(result, "fn_number32 %u ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%u ", arq_uint32_t(queue));
        }

}
 
TEST(arq, uint32_t) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "numberA",  fn_number32,  "(uint32_t number)"},
                {'b', "numberB",  fn_number32,  "(uint32_t number = 324)"},
                {'c', "numberC",  fn_number32_array,  "(uint32_t number[])"},
        };
        uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--numberA", "sdf");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp_verbose(buffer, 
                                        "CMD line failure:\n"
                                        "    --numberA sdf \n"
                                        "    Token 'sdf' is not a positiv number\n"
                                        "    -a --numberA (uint32_t number)\n"
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
                                        "    Token '42949672950' positive number > UINT32_MAX 4294967295\n"
                                        "    -a --numberA (uint32_t number)\n"
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
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number32 4 15 40004 1 42 "));
        }
}

#if 0

                {'s', "string",  NULL,  "(cstr_t str)"},
                {'n', "nstring", NULL, "(cstr_t str = NULL)"},
                {'u', "uint8",   NULL,   "(uint8_t number[])"},
                {'i', "int32",   NULL,   "(int32_t number)"}, 
                {'i', "int32",   NULL,   "(int32_t number = -56)"}, 

                {'p', "print",   NULL,   "(uint32_t first_line = 0, uint32_t last_line = +1200)"},
                {'a', "array",   NULL,   "(int32_t numbers[], cstr_t list[])"},
#endif
