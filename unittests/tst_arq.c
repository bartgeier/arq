#include "gtest/gtest.h"
#include "arq.h"
#include <stdio.h>
#include <stdarg.h>

int strcmp_verbose(const char* s1, const char* s2) {
        int i = 0;
        printf("%s", s1);
        // Compare character by character
        while (s1[i] != '\0' && s2[i] != '\0') {
                if (s1[i] != s2[i]) {
                        printf("--------------\n");
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
                printf("--------------\n");
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

void fn_number8(Arq_Queue *queue) {
        uint8_t x = arq_uint8_t(queue);
        sprintf(result, "fn_number8 %u", x);
}
void fn_number8_array(Arq_Queue *queue) {
        uint32_t const array_size = arq_array_size(queue);
        uint32_t i;
        int pos = sprintf(result, "fn_number8_array %u ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%u ", arq_uint8_t(queue));
        }

}
TEST(arq, uint8_t) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "numberA",  fn_number8,  "(uint8_t number)"},
                {'b', "numberB",  fn_number8,  "(uint8_t number = 124)"},
                {'c', "numberC",  fn_number8_array,  "(uint8_t number[])"},
        };
        uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--numberA", "sdf");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer, 
                                        "CMD line failure:\n"
                                        "    --numberA sdf \n"
                                        "    Token 'sdf' is not a positiv number\n"
                                        "    -a --numberA (uint8_t number)\n"
                                ), 0
                        );
                } else {
                        EXPECT_FALSE(true);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "256");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 256 \n"
                                        "    Token '256' positive number > UINT8_MAX 255\n"
                                        "    -a --numberA (uint8_t number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "0xFF0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 0xFF0 \n"
                                        "    Token '0xFF0' more than 2 hex digits\n"
                                        "    -a --numberA (uint8_t number)\n"
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
                EXPECT_TRUE(0 == strcmp(result,"fn_number8 123"));
        }
        {
                set(&cmd, "arq", "--numberA", "0xAa");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number8 170"));
        }
        {
                set(&cmd, "arq", "--numberB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number8 124"));
        }
        {
                set(&cmd, "arq", "--numberB", "0xFF");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number8 255"));
        }
        {
                set(&cmd, "arq", "--numberC", "0xF", "242", "1", "42");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number8_array 4 15 242 1 42 "));
        }
}

void fn_number16(Arq_Queue *queue) {
        uint16_t x = arq_uint16_t(queue);
        sprintf(result, "fn_number16 %u", x);
}
void fn_number16_array(Arq_Queue *queue) {
        uint32_t const array_size = arq_array_size(queue);
        uint32_t i;
        int pos = sprintf(result, "fn_number16_array %u ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%u ", arq_uint16_t(queue));
        }

}
TEST(arq, uint16_t) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "numberA",  fn_number16,  "(uint16_t number)"},
                {'b', "numberB",  fn_number16,  "(uint16_t number = 0xffff)"},
                {'c', "numberC",  fn_number16_array,  "(uint16_t number[])"},
        };
        uint32_t const o_size = sizeof(options)/sizeof(Arq_Option);
        {
                set(&cmd, "arq", "--numberA", "sdf");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer, 
                                        "CMD line failure:\n"
                                        "    --numberA sdf \n"
                                        "    Token 'sdf' is not a positiv number\n"
                                        "    -a --numberA (uint16_t number)\n"
                                ), 0
                        );
                } else {
                        EXPECT_FALSE(true);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "65536");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 65536 \n"
                                        "    Token '65536' positive number > UINT16_MAX 65535\n"
                                        "    -a --numberA (uint16_t number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "0xFFFFFFF0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 0xFFFFFFF0 \n"
                                        "    Token '0xFFFFFFF0' more than 4 hex digits\n"
                                        "    -a --numberA (uint16_t number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "42069");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number16 42069"));
        }
        {
                set(&cmd, "arq", "--numberA", "0xAaAa");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number16 43690"));
        }
        {
                set(&cmd, "arq", "--numberB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number16 65535"));
        }
        {
                set(&cmd, "arq", "--numberB", "0xFF00");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number16 65280"));
        }
        {
                set(&cmd, "arq", "--numberC", "0x8000", "1242", "69", "42");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_number16_array 4 32768 1242 69 42 "));
        }
}

void fn_number32(Arq_Queue *queue) {
        uint32_t x = arq_uint32_t(queue);
        sprintf(result, "fn_number32 %u", x);
}
void fn_number32_array(Arq_Queue *queue) {
        uint32_t const array_size = arq_array_size(queue);
        uint32_t i;
        int pos = sprintf(result, "fn_number32_array %u ", array_size);
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
                                strcmp(buffer, 
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
                set(&cmd, "arq", "--numberA", "0xFFFFFFFF0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 0xFFFFFFFF0 \n"
                                        "    Token '0xFFFFFFFF0' more than 8 hex digits\n"
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
                EXPECT_TRUE(0 == strcmp(result,"fn_number32_array 4 15 40004 1 42 "));
        }
}

void fn_numberi8(Arq_Queue *queue) {
        int8_t x = arq_int8_t(queue);
        sprintf(result, "fn_numberi8 %d", x);
}
void fn_numberi8_array(Arq_Queue *queue) {
        int32_t const array_size = arq_array_size(queue);
        int32_t i;
        int pos = sprintf(result, "fn_numberi8_array %d ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%d ", arq_int8_t(queue));
        }
}
TEST(arq, int8_t) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "numberA",  fn_numberi8,  "(int8_t number)"},
                {'b', "numberB",  fn_numberi8,  "(int8_t number = -84)"},
                {'c', "numberC",  fn_numberi8_array,  "(int8_t number[])"},
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
                                        "    -a --numberA (int8_t number)\n"
                                ), 0
                        );
                } else {
                        EXPECT_FALSE(true);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "128");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 128 \n"
                                        "    Token '128' positive number > INT8_MAX 127\n"
                                        "    -a --numberA (int8_t number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "0xFF0");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA 0xFF0 \n"
                                        "    Token '0xFF0' more than 2 hex digits\n"
                                        "    -a --numberA (int8_t number)\n"
                                ), 0
                        );
                } else {
                        ASSERT_TRUE(false);
                }
        }
        {
                set(&cmd, "arq", "--numberA", "-129");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        EXPECT_EQ(
                                strcmp(buffer,
                                        "CMD line failure:\n"
                                        "    --numberA -129 \n"
                                        "    Token '-129' negative number < INT8_MIN -128\n"
                                        "    -a --numberA (int8_t number)\n"
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
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi8 -123"));
        }
        {
                set(&cmd, "arq", "--numberA", "0xAa");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi8 -86"));
        }
        {
                set(&cmd, "arq", "--numberB");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi8 -84"));
        }
        {
                set(&cmd, "arq", "--numberB", "0xFF");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi8 -1"));
        }
        {
                set(&cmd, "arq", "--numberC", "0xF", "-44", "42", "0x80");
                if (0 < arq_fn(cmd.argc, cmd.argv, buffer, b_size, options, o_size)) {
                        ASSERT_TRUE(false);
                }
                EXPECT_TRUE(0 == strcmp(result,"fn_numberi8_array 4 15 -44 42 -128 "));
        }
}

void fn_numberi32(Arq_Queue *queue) {
        int32_t x = arq_int32_t(queue);
        sprintf(result, "fn_numberi32 %d", x);
}
void fn_numberi32_array(Arq_Queue *queue) {
        int32_t const array_size = arq_array_size(queue);
        int32_t i;
        int pos = sprintf(result, "fn_numberi32_array %d ", array_size);
        for (i = 0; i < array_size; i++) {
                pos += sprintf(result + pos, "%d ", arq_int32_t(queue));
        }

}
TEST(arq, int32_t) {
        result[0] = 0;
        Arq_Option options[] = {
                {'a', "numberA",  fn_numberi32,  "(int32_t number)"},
                {'b', "numberB",  fn_numberi32,  "(int32_t number = -324)"},
                {'c', "numberC",  fn_numberi32_array,  "(int32_t number[])"},
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
                                        "    -a --numberA (int32_t number)\n"
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
                                        "    -a --numberA (int32_t number)\n"
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
                                        "    -a --numberA (int32_t number)\n"
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
                                        "    -a --numberA (int32_t number)\n"
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

#if 0

                {'s', "string",  NULL,  "(cstr_t str)"},
                {'n', "nstring", NULL, "(cstr_t str = NULL)"},
                {'u', "uint8",   NULL,   "(uint8_t number[])"},
                {'i', "int32",   NULL,   "(int32_t number)"}, 
                {'i', "int32",   NULL,   "(int32_t number = -56)"}, 

                {'p', "print",   NULL,   "(uint32_t first_line = 0, uint32_t last_line = +1200)"},
                {'a', "array",   NULL,   "(int32_t numbers[], cstr_t list[])"},
#endif
