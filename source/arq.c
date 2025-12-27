#include "arq_options.h"
#include "arq_queue.h"
#include "arq_immediate.h"
#include "arq_log.h"
#include "arq_bool.h"
#include "arq_token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

static void error_msg_append_option(Arq_msg *error_msg, Arq_Option const *o) {
        assert(o != NULL);
        if (o->chr != 0) {
                arq_msg_append_cstr(error_msg, "-");
                arq_msg_append_chr(error_msg, o->chr);
                arq_msg_append_cstr(error_msg, " ");
        }
        if (strlen(o->name) != 0) {
                arq_msg_append_cstr(error_msg, "--");
                arq_msg_append_cstr(error_msg, o->name);
                arq_msg_append_cstr(error_msg, " ");
        }
        if (strlen(o->arguments ) != 0) {
                arq_msg_append_cstr(error_msg, o->arguments);
        }
        arq_msg_append_lf(error_msg);
}

static void error_msg_insert_cmd_line(Arq_msg *m, uint32_t line_nr, Arq_Vector *cmd) {
        uint32_t A_IDX, B_IDX, C_IDX, D_IDX, ARGV_LEN; 
        uint32_t ln_count = 0;
        uint32_t i;
        A_IDX = 0;
        for (i = 0; i < m->size; i++) {
                if (m->at[i] == '\n') {
                        ln_count++;
                        if (line_nr == ln_count) {
                                A_IDX = i + 1;
                        }
                }
        }

        B_IDX = m->size;
        for (i = 0; i <= cmd->idx; i++) {
                /* render argv to calculate argv_len */
                if (cmd->at[i].id == ARQ_CMD_SHORT_OPTION) {
                        uint32_t const x = cmd->at[i].at[0] == '-' ? 1 : 0; /* because of short option bundeling  */
                        arq_msg_append_chr(m, cmd->at[i].at[0]);
                        arq_msg_append_nchr(m, cmd->at[i].at[1], x);
                        arq_msg_append_chr(m, '_');
                } else if (cmd->at[i].id == ARQ_CMD_LONG_OPTION) {
                        /* arq_msg_append_nchr(m, '-', 2); */
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, '_');
                } else if (cmd->at[i].id == ARQ_CMD_END_OF_LINE) {
                } else {
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, '_');
                }
        }
        arq_msg_append_lf(m);
        C_IDX = m->size;
        ARGV_LEN = C_IDX - B_IDX;
        {
                uint32_t const shift_right = B_IDX - A_IDX;
                for (i = 0; i < shift_right; i++) {
                        uint32_t const idx = B_IDX - 1 - i;
                        m->at[idx + ARGV_LEN] = m->at[idx];
                }
        }

        D_IDX = m->size;
        for (i = 0; i <= cmd->idx; i++) {
                /* render argv once more for moving argv */
                if (cmd->at[i].id == ARQ_CMD_SHORT_OPTION) {
                        uint32_t const x = cmd->at[i].at[0] == '-' ? 1 : 0; /* because of short option bundeling  */
                        arq_msg_append_chr(m, cmd->at[i].at[0]);
                        arq_msg_append_nchr(m, cmd->at[i].at[1], x);
                        arq_msg_append_chr(m, ' ');
                } else if (cmd->at[i].id == ARQ_CMD_LONG_OPTION) {
                        /* arq_msg_append_nchr(m, '-', 2); */
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, ' ');
                } else if (cmd->at[i].id == ARQ_CMD_END_OF_LINE) {
                } else {
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, ' ');
                }
        }
        arq_msg_append_lf(m);

        {
                /* moving rendered argv */
                for (i = 0; i < ARGV_LEN; i++) {
                        m->at[i + A_IDX] =  m->at[i + D_IDX];
                }
                /* delete argv */
                m->size = D_IDX;
        }
}

static void output_option_verify_failure(Arq_msg *error_msg, char *arena_buffer) {
        uint32_t i;
        arq_msg_format(error_msg);
        for (i = 0; i < error_msg->size; i++) {
                arena_buffer[i] = error_msg->at[i];
        }
        arena_buffer[error_msg->size] = 0;
        assert(arena_buffer[error_msg->size] == 0);
}

static void output_cmd_line_option_failure(Arq_msg *error_msg, char *arena_buffer) {
        uint32_t i;
        arq_msg_format(error_msg);
        for (i = 0; i < error_msg->size; i++) {
                arena_buffer[i] = error_msg->at[i];
        }
        arena_buffer[error_msg->size] = 0;
        assert(arena_buffer[error_msg->size] == 0);
}

static void output_cmd_line_conversion_failure(Arq_msg *error_msg, Arq_Option const *o, char *arena_buffer) {
        uint32_t i;
        error_msg_append_option(error_msg, o);
        arq_msg_format(error_msg);
        for (i = 0; i < error_msg->size; i++) {
                arena_buffer[i] = error_msg->at[i];
        }
        arena_buffer[error_msg->size] = 0;
        assert(arena_buffer[error_msg->size] == 0);
}

static void call_back_function(Arq_Option const *options, Arq_List const *option_list, Arq_Queue *queue) {
        Arq_Option const *option = &options[option_list->row];
        option->fn(queue);
        assert(queue->read_idx == queue->write_idx && "Queue is not empty, there are still arguments in the queue!");
        arq_queue_clear(queue);
}

static uint32_t arq_option_parameter_idx(Arq_Option const *option) {
        uint32_t STRLEN;
        uint32_t result = 0;
        if (option->chr != 0) {
                result += 3;
        }
        STRLEN = strlen(option->name);
        if (STRLEN > 0) {
                result += STRLEN + 3;
        }
        return result;
}

uint32_t arq_verify(
        char *arena_buffer, uint32_t const buffer_size,
        Arq_Option const *options, uint32_t const num_of_options
) {
#if 1
        Arq_msg error_msg;
        char const *error_str;
        uint32_t i;
        Arq_Arena *arena;
        (void) buffer_size;

        log_memory(("Buffer capacity %d byte", buffer_size));
        arena = arq_arena_init(arena_buffer, buffer_size);
        log_memory(("Arena head %d byte, capacity %d byte,                arena.size = %4d byte", 
                (int)offsetof(Arq_Arena,at),
                arena->SIZE, 
                arena->size
        ));

        {
                uint32_t SIZE_OF_ERROR_MSG;
                uint32_t const shrink = arena->size;
                char *chr = (char *)arq_arena_malloc_rest(arena, 0, sizeof(char), &SIZE_OF_ERROR_MSG);
                arena->size = shrink;
                error_msg.SIZE = SIZE_OF_ERROR_MSG;
                error_msg.size = 0;
                error_msg.at = chr;
        }

        for (i = 0; i < num_of_options; i++) {
                Lexer l = arq_lexer_create();
                l.at = options[i].arguments;
                l.SIZE = strlen(options[i].arguments);
                l.cursor_idx = 0;
                arq_next_opt_token(&l);
                error_str = "' missing open parenthesis '('\n";
                if (arq_imm_L_parenthesis_t(&l)) {
                        if (arq_imm_R_parenthesis_t(&l)) {
                                error_str = "' after ')' no tokens allowed!\n";
                                if (arq_imm_noToken(&l.token)) {
                                        return 0;
                                } 
                                goto error;
                        }
                        do {
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                                if (arq_imm_type_t(&l, ARQ_OPT_UINT32_T)) {
                                        error_str = "' but expected '=' or '[]' or ',' or ')'\n";
                                        if (arq_imm_not_identifier_t(&l)) {
                                                error_str = "' is not a parameter name\n";
                                                goto error;

                                        }
                                        if (arq_imm_equal_t(&l)) {
                                                if (false == arq_imm_is_a_uint32_t_t(&l)) {
                                                        error_str = "' is not a uint32_t literal\n";
                                                        goto error;
                                                }
                                                error_str = "' but expected ',' or ')'\n";
                                        } else if (arq_imm_array_t(&l)) {
                                                error_str = "' but expected ',' or ')'\n";
                                        }
                                        if (arq_imm_comma_t(&l)) {
                                                continue;
                                        }
                                        if (arq_imm_R_parenthesis_t(&l)) {
                                                error_str = "' after ')' no tokens allowed!\n";
                                                if (arq_imm_noToken(&l.token)) {
                                                        return 0;
                                                } 
                                        }
                                        goto error;
                                }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                                if (arq_imm_type_t(&l, ARQ_OPT_INT32_T)) {
                                        error_str = "' but expected '=' or '[]' or ',' or ')'\n";
                                        if (arq_imm_not_identifier_t(&l)) {
                                                error_str = "' is not a parameter name\n";
                                                goto error;

                                        }
                                        if (arq_imm_equal_t(&l)) {
                                                if (false == arq_imm_is_a_int32_t_t(&l)) {
                                                        error_str = "' is not a int32_t literal\n";
                                                        goto error;
                                                }
                                                error_str = "' but expected ',' or ')'\n";
                                        } else if (arq_imm_array_t(&l)) {
                                                error_str = "' but expected ',' or ')'\n";
                                        }
                                        if (arq_imm_comma_t(&l)) {
                                                continue;
                                        }
                                        if (arq_imm_R_parenthesis_t(&l)) {
                                                error_str = "' after ')' no tokens allowed!\n";
                                                if (arq_imm_noToken(&l.token)) {
                                                        return 0;
                                                } 
                                        }
                                        goto error;
                                }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                                if (arq_imm_type_t(&l, ARQ_OPT_FLOAT)) {
                                        error_str = "' but expected '=' or '[]' or ',' or ')'\n";
                                        if (arq_imm_not_identifier_t(&l)) {
                                                error_str = "' is not a parameter name\n";
                                                goto error;

                                        }
                                        if (arq_imm_equal_t(&l)) {
                                                if (false == arq_imm_is_a_float_t(&l)) {
                                                        error_str = "' is not a float literal\n";
                                                        goto error;
                                                }
                                                error_str = "' but expected ',' or ')'\n";
                                        } else if (arq_imm_array_t(&l)) {
                                                error_str = "' but expected ',' or ')'\n";
                                        }
                                        if (arq_imm_comma_t(&l)) {
                                                continue;
                                        }
                                        if (arq_imm_R_parenthesis_t(&l)) {
                                                error_str = "' after ')' no tokens allowed!\n";
                                                if (arq_imm_noToken(&l.token)) {
                                                        return 0;
                                                } 
                                        }
                                        goto error;
                                }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                                if (arq_imm_type_t(&l, ARQ_OPT_CSTR_T)) {
                                        error_str = "' but expected '=' or '[]' or ',' or ')'\n";
                                        if (arq_imm_not_identifier_t(&l)) {
                                                error_str = "' is not a parameter name\n";
                                                goto error;

                                        }
                                        if (arq_imm_equal_t(&l)) {
                                                if (false == arq_imm_is_a_NULL_t(&l)) {
                                                        error_str =  "' must be NULL\n";
                                                        goto error;
                                                }
                                                error_str = "' but expected ',' or ')'\n";
                                        } else if (arq_imm_array_t(&l)) {
                                                error_str = "' but expected ',' or ')'\n";
                                        }
                                        if (arq_imm_comma_t(&l)) {
                                                continue;
                                        }
                                        if (arq_imm_R_parenthesis_t(&l)) {
                                                error_str = "' after ')' no tokens allowed!\n";
                                                if (arq_imm_noToken(&l.token)) {
                                                        return 0;
                                                } 
                                        }
                                        goto error;
                                } 
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                                error_str = "' is not a type\n";
                                goto error;
                        } while (l.token.id != ARQ_OPT_NO_TOKEN);
                }
error:
                {
                        /* error */
                        uint32_t n;
                        uint32_to ups; 
                        ups.error = true; 
                        ups.u32 = l.cursor_idx - l.token.size;
                        arq_msg_clear(&error_msg);
                        arq_msg_append_cstr(&error_msg, "Option failure:\n");
                        arq_msg_append_cstr(&error_msg, "token '");
                        arq_msg_append_str(&error_msg, l.token.at, l.token.size);
                        arq_msg_append_cstr(&error_msg, error_str);

                        n = arq_option_parameter_idx(&options[i]) + ups.u32;
                        error_msg_append_option(&error_msg, &options[i]);
                        arq_msg_append_nchr(&error_msg, ' ', n);
                        arq_msg_append_cstr(&error_msg, "^\n");
                        output_option_verify_failure(&error_msg, arena_buffer);
                        return error_msg.size;
                }

        } /* for loop */
        assert(false);
        return 0;
#endif
}

uint32_t arq_fn(
        int argc, char **argv, 
        char *arena_buffer, uint32_t const buffer_size,
        Arq_Option const *options, uint32_t const num_of_options
) {
        Arq_Arena *arena;
        Arq_List *option_list;
        Arq_msg error_msg;
        Arq_Vector *cmd;
        Arq_Queue *queue;
        uint32_t i;

        log_memory(("Buffer capacity %d byte", buffer_size));
        arena = arq_arena_init(arena_buffer, buffer_size);
        log_memory(("Arena head %d byte, capacity %d byte,                arena.size = %4d byte", 
                (int)offsetof(Arq_Arena,at),
                arena->SIZE, 
                arena->size
        ));

#if 0
        {
                uint32_t  const error_msg_size = arq_verify(arena_buffer, buffer_size, options, num_of_options);
                if (error_msg_size > 0) {
                        return error_msg_size;
                }
                arena->size = 0;
        }
#endif

        option_list = (Arq_List *)arq_arena_malloc(arena, offsetof(Arq_List, at) + num_of_options * sizeof(Arq_OptVector *));
        option_list->num_of_Vec = 0;
        option_list->row = 0;

        log_memory(("Options list        %2d + %2d pVect * %2d = %3d byte => arena.size = %4d byte",
                (int)offsetof(Arq_List,at),
                num_of_options,
                (int)sizeof(option_list->at),
                (int)offsetof(Arq_List,at) + (int)(num_of_options * sizeof(option_list->at)),
                arena->size
        ));

        {
                uint32_t SIZE_OF_ERROR_MSG;
                uint32_t const shrink = arena->size;
                error_msg.at = (char *)arq_arena_malloc_rest(arena, 0, sizeof(char), &SIZE_OF_ERROR_MSG);
                error_msg.SIZE = SIZE_OF_ERROR_MSG;
                error_msg.size = 0;
                arena->size = shrink;
        }

        /* Arq_OptVector */
        for (i = 0; i < num_of_options; i++) {
                uint32_t NUM_OF_TOKEN;
                uint32_t const shrink = arena->size;
                Arq_OptVector *v = (Arq_OptVector *)arq_arena_malloc_rest(arena, offsetof(Arq_OptVector, at), sizeof(Arq_Token), &NUM_OF_TOKEN);
                arena->size = shrink;

                arq_option_tokenize(&options[i], v, NUM_OF_TOKEN);
                {
                        Arq_OptVector *vb = (Arq_OptVector *)arq_arena_malloc(arena,  offsetof(Arq_OptVector, at) + v->num_of_token * sizeof(Arq_Token));
                        assert(v == vb);
                        log_memory(("      Option vector %2d + %2d token * %2d = %3d byte => arena.size = %4d byte", 
                                (int)offsetof(Arq_Token, at),
                                vb->num_of_token,
                                (int)sizeof(vb->at[0]),
                                (int)offsetof(Arq_Token, at) + (int)(vb->num_of_token * sizeof(vb->at[0])),
                                arena->size
                        ));
                }
                assert(option_list->num_of_Vec < num_of_options);
                option_list->at[option_list->num_of_Vec++] = v;
        }
        log_tokenizer_option_t(options, num_of_options);

/*///////////////////////////////////////////////////////////////////////////*/

        {
                uint32_t NUM_OF_TOKEN;
                uint32_t const shrink = arena->size;
                cmd = (Arq_Vector *)arq_arena_malloc_rest(arena, offsetof(Arq_Vector, at), sizeof(Arq_Token), &NUM_OF_TOKEN);
                arena->size = shrink;
                arq_cmd_tokenize(argc, argv, cmd, NUM_OF_TOKEN);
                assert(cmd == arq_arena_malloc(arena,  offsetof(Arq_Vector, at) + cmd->num_of_token * sizeof(Arq_Token)));
                log_memory(("Command line vector %2d + %2d token * %2d = %3d byte => arena.size = %4d byte", 
                        (int)offsetof(Arq_Vector, at),
                        cmd->num_of_token, 
                        (int)sizeof(cmd->at[0]), 
                        (int)offsetof(Arq_Vector, at) + (int)(cmd->num_of_token * sizeof(cmd->at[0])), 
                        arena->size
                ));
        }
        log_tokenizer_cmd_line(cmd);

/*///////////////////////////////////////////////////////////////////////////*/
        log_int_banner("interpreter");
        queue = arq_queue_malloc(arena);
        log_memory(("Argument queue      %2d + %2d argum * %2d = %3d byte => arena.size = %4d byte", 
                (int)offsetof(Arq_Queue, at),
                queue->NUM_OF_ARGUMENTS,
                (int)sizeof(queue->at[0]),
                (int)offsetof(Arq_Queue, at) + (int)(queue->NUM_OF_ARGUMENTS * sizeof(queue->at[0])),
                arena->size
        ));
        log_memory(("%d arguments fit in the queue.\n", queue->NUM_OF_ARGUMENTS));

        while(arq_imm_cmd_has_token_left(cmd)) {
                /* Arq_OptVector *opt = NULL; */
                Lexer opt = arq_lexer_create();
                log_int_ln();
                if (arq_imm_cmd_is_long_option(cmd)) {
                        log_int_token(ARQ_CMD_LONG_OPTION);
                        opt = arq_imm_get_long_t(option_list, options, cmd, &error_msg);
                        if (opt.at == NULL) {
                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                output_cmd_line_option_failure(&error_msg, arena_buffer);
                                return error_msg.size;
                        }
                } else if (arq_imm_cmd_is_short_option(cmd)) {
                        log_int_token(ARQ_CMD_SHORT_OPTION);
                        opt = arq_imm_get_short_t(option_list, options, cmd, &error_msg);
                        if (opt.at == NULL) {
                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                output_cmd_line_option_failure(&error_msg, arena_buffer);
                                return error_msg.size;
                        }
                } else if (arq_imm_end_of_line(cmd)) {
                        log_int_token(ARQ_CMD_END_OF_LINE);
                        arena_buffer[0] = 0;
                        return 0;
                } else {
                        arq_imm_cmd_not_a_option(cmd, &error_msg);
                        error_msg_insert_cmd_line(&error_msg, 1, cmd);
                        output_cmd_line_option_failure(&error_msg, arena_buffer);
                        return error_msg.size;
                }
                arq_next_opt_token(&opt);
                (void)arq_imm_L_parenthesis_t(&opt);
                while (true) {
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm_type_t(&opt, ARQ_OPT_UINT32_T)) {
                                log_int_token_indent(ARQ_OPT_UINT32_T);
                                (void)arq_imm_not_identifier_t(&opt);
                                if (arq_imm_equal_t(&opt)) {
                                        uint32_to num = arq_imm_default_uint32_t_t(&opt);
                                        if (arq_imm_optional_argument_uint32_t(cmd, &num, &error_msg)) {
                                                /* overflow */
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                        arq_push_uint32_t(queue, num.u32);
                                        log_inta(("u32 %u", num.u32));
                                } else if (arq_imm_array_t(&opt)) {
                                        uint32_t *array_size = arq_push_array_size(queue, 0);
                                        log_inta(("u32 %u // init array_size", *array_size));
                                        while (arq_imm_is_p_dec(cmd) || arq_imm_is_hex(cmd)) {
                                                uint32_to num = {0};
                                                if (arq_imm_optional_argument_uint32_t(cmd, &num, &error_msg)) {
                                                        /* overflow */
                                                        error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                        output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                        return error_msg.size;
                                                }
                                                *array_size += 1;
                                                arq_push_uint32_t(queue, num.u32);
                                                log_inta(("u32 %u", num.u32));
                                        }
                                } else {
                                        uint32_to const num = arq_imm_argument_uint32_t(cmd, &error_msg);
                                        if (num.error) { 
                                                /* wasn't an uint32_t number or overflow */
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                        arq_push_uint32_t(queue, num.u32);
                                        log_inta(("u32 %u", num.u32));
                                }
                                if (arq_imm_comma_t(&opt)) continue;
                                goto terminator;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm_type_t(&opt, ARQ_OPT_INT32_T)) {
                                log_int_token_indent(ARQ_OPT_INT32_T);
                                (void)arq_imm_not_identifier_t(&opt);
                                if (arq_imm_equal_t(&opt)) {
                                        int32_to num = arq_imm_default_int32_t_t(&opt);
                                        if (arq_imm_optional_argument_int32_t(cmd, &num, &error_msg)) {
                                                /* overflow */
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                        arq_push_int32_t(queue, num.i32);
                                        log_inta(("i32 %d", num.i32));
                                } else if (arq_imm_array_t(&opt)) {
                                        uint32_t *array_size = arq_push_array_size(queue, 0);
                                        log_inta(("u32 %u // init array_size", *array_size));
                                        while (arq_imm_is_p_dec(cmd) || arq_imm_is_n_dec(cmd) || arq_imm_is_hex(cmd)) {
                                                int32_to num = {0};
                                                if (arq_imm_optional_argument_int32_t(cmd, &num, &error_msg)) {
                                                        /* overflow */
                                                        error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                        output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                        return error_msg.size;
                                                }
                                                *array_size += 1;
                                                arq_push_int32_t(queue, num.i32);
                                                log_inta(("i32 %d", num.i32));
                                        }
                                } else {
                                        int32_to const num = arq_imm_argument_int32_t(cmd, &error_msg);
                                        if (num.error) { 
                                                /* wasn't an int32_t number or overflow */
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                        arq_push_int32_t(queue, num.i32);
                                        log_inta(("i32 %d", num.i32));
                                }
                                if (arq_imm_comma_t(&opt)) continue;
                                goto terminator;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm_type_t(&opt, ARQ_OPT_FLOAT)) {
                                log_int_token_indent(ARQ_OPT_FLOAT);
                                (void)arq_imm_not_identifier_t(&opt);
                                if (arq_imm_equal_t(&opt)) {
                                        float_to num = arq_imm_default_float_t(&opt);
                                        if (arq_imm_optional_argument_float(cmd, &num, &error_msg)) {
                                                /* in the moment there is no error */
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                        arq_push_float(queue, num.f);
                                        log_inta(("float %f", num.f));
                                } else if (arq_imm_array_t(&opt)) {
                                        uint32_t *array_size = arq_push_array_size(queue, 0);
                                        log_inta(("u32 %u // init array_size", *array_size));
                                        while (arq_imm_is_hexFloat(cmd)) {
                                                float_to num = {0};
                                                if (arq_imm_optional_argument_float(cmd, &num, &error_msg)) {
                                                        /* in the moment there is no error */
                                                        error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                        output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                        return error_msg.size;
                                                }
                                                *array_size += 1;
                                                arq_push_float(queue, num.f);
                                                log_inta(("float %f", num.f));
                                        }
                                } else {
                                        float_to const num = arq_imm_argument_float(cmd, &error_msg);
                                        if (num.error) { 
                                                /* wasn't an float number */
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer); 
                                                return error_msg.size;
                                        }
                                        arq_push_float(queue, num.f);
                                        log_inta(("float %f", num.f));
                                }
                                if (arq_imm_comma_t(&opt)) continue;
                                goto terminator;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
                        if (arq_imm_type_t(&opt, ARQ_OPT_CSTR_T)) {
                                char const *cstr; 
                                log_int_token_indent(ARQ_OPT_CSTR_T);
                                (void)arq_imm_not_identifier_t(&opt);
                                if (arq_imm_equal_t(&opt)) {
                                        cstr = arq_imm_default_cstr_t_t(&opt);
                                        if (arq_imm_cmd_is_dashdash(cmd)) {
                                                cstr = arq_imm_argument_csrt_t(cmd, &error_msg);
                                                if (cstr == NULL) {
                                                        error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                        arq_msg_append_cstr(&error_msg, "'--' alone isn't enough if you want '--' as an argument then do -- --\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' allows you to set an argument that looks like an option -- --hello\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' undoes optional behavior in case of an cstr_t = NULL\n");
                                                        output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer);
                                                        return error_msg.size;
                                                }
                                        } else {
                                                /* For a short option with an optional cstr_t as an argument. */
                                                /* It is not always possible to include the argument immediately after the option. */
                                                /* This is the case whether the next character is a bundled option or a character from a cstr_t. */
                                                /* If the next character is a number, then it is an argument => here is it possible. */
                                                /* {'S', "cstring", fn_cstring, &ctx, "cstr_t = NULL"}, */
                                                /* failure: -abcShello    => the 'h' is interpreted as short option part of the bundle (no space) thats why failure */
                                                /* ok:      -abcS hello   => is string token fine */
                                                /* ok:      -abcS69       => 69 is a number fine can't be a short option */
                                                (void)arq_imm_optional_argument_cstr_t(cmd, &cstr);
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                } else if (arq_imm_array_t(&opt)) {
                                        struct {
                                                bool on;
                                                bool edge;
                                        } dashdash = {0};
                                        uint32_t *array_size = arq_push_array_size(queue, 0);
                                        log_inta(("u32 %u // init array_size", *array_size));
                                        while (1) {
                                                dashdash.on |= arq_imm_cmd_is_dashdash(cmd);
                                                if (dashdash.on && !arq_imm_pick_cstr_t(cmd, &cstr)) { 
                                                        if (dashdash.edge) { 
                                                                break; 
                                                        }
                                                        arq_msg_append_cstr(&error_msg, CMD_LINE_FAILURE);
                                                        error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                        arq_msg_append_cstr(&error_msg, "'--' alone isn't enough if you want '--' as an argument then do -- --\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' allows to set an argument that looks like an option -- --hello\n");
                                                        arq_msg_append_cstr(&error_msg, "'--' switch to positional arguments in case of an cstr_t array\n");
                                                        output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer);
                                                        return error_msg.size;
                                                } 
                                                if (!dashdash.on && !arq_imm_optional_argument_cstr_t(cmd, &cstr)) { 
                                                        break;
                                                }
                                                dashdash.edge = dashdash.on;
                                                *array_size += 1;
                                                arq_push_cstr_t(queue, cstr);
                                                log_inta(("cstr %s", cstr));
                                        }
                                } else {
                                        /* A short option with a mandatory argument allows the argument to be included immediately after the option. */
                                        /* However, this short option must be the last option in a bundle of options. */
                                        /* {'S', "cstring", fn_cstring, &ctx, "cstr_t"}, */
                                        /* 'hello' is the argument */
                                        /* ok: -abcShello    => 'hello' is the argument */
                                        /* ok: -abcS hello   => 'hello' is the argument */
                                        /* ok: -abcS--hello  => '--hello' is the argument */
                                        /* ok: -abcS-hello   => '-hello' is the argument */
                                        cstr = arq_imm_argument_csrt_t(cmd, &error_msg);
                                        if (cstr == NULL) {
                                                error_msg_insert_cmd_line(&error_msg, 1, cmd);
                                                output_cmd_line_conversion_failure(&error_msg, &options[option_list->row], arena_buffer);
                                                return error_msg.size;
                                        }
                                        arq_push_cstr_t(queue, cstr);
                                }
                                if (arq_imm_comma_t(&opt)) continue;
                                goto terminator;
                        }
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
terminator:
                        if (arq_imm_R_parenthesis_t(&opt)) {
                                log_int_comment("call_back_function");
                                call_back_function(options, option_list, queue);
                                break;
                        }
                        assert(false);
                } /* while */
        } /* while */
        arena_buffer[0] = 0;
        return 0;
}
