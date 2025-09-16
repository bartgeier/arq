#include "arq_msg.h"
#include "arq_symbols.h"
#include <assert.h>
#include <string.h>

void arq_msg_clear(Arq_msg *m) {
        m->size = 0;
}

void arq_msg_format(Arq_msg *m) {
        uint32_t number_of_lf = 0;
        for (uint32_t i = 0; i < m->size; i++) {
                uint32_t const last = m->size - 1;
                if (m->at[i] == '\n' && i < last) {
                        number_of_lf++;
                }
        }
        uint32_t const indent_size = 4;
        uint32_t const shift_right = number_of_lf * indent_size;
        assert(m->size + shift_right < m->SIZE);
        for (uint32_t i = 0; i < m->size; i++) {
                m->at[m->size - 1 - i + shift_right] = m->at[m->size - 1 - i];
        }
        m->size += shift_right;
        m->at[m->size] = '\0';
        uint32_t j = 0;
        for (uint32_t i = shift_right; i < m->size; i++) {
                m->at[j++] = m->at[i];
                if (m->at[i] == '\n' && i + indent_size < m->size) {
                        memset(&m->at[j], ' ', indent_size);
                        j += indent_size;
                }
        }
}

void arq_msg_append_chr(Arq_msg *m, char const chr) {
        assert(m->size + 1 < m->SIZE);
        m->at[m->size++] = chr;
        m->at[m->size] = 0; // thats wy m->size has to be smaller than m->SIZE
}

void arq_msg_append_nchr(Arq_msg *m, char const chr, uint32_t const num_of_chr) {
        for (uint32_t i = 0; i < num_of_chr; i++) {
                arq_msg_append_chr(m, chr);
        }
}

void arq_msg_append_lf(Arq_msg *m) {
        arq_msg_append_chr(m, '\n');
}

void arq_msg_append_cstr(Arq_msg *m, char const *cstr) {
        uint32_t len = strlen(cstr);
        for (uint32_t i = 0; i < len; i++) {
                arq_msg_append_chr(m, cstr[i]);
        }
}

void arq_msg_insert_cmd_ln(Arq_msg *m, uint32_t line_nr, Arq_Vector *cmd) {
        uint32_t ln_count = 0;
        uint32_t a_idx = 0; 
        for (uint32_t i = 0; i < m->size; i++) {
                if (m->at[i] == '\n') {
                        ln_count++;
                        if (line_nr == ln_count) {
                                a_idx = i + 1;
                        }
                }
        }

        uint32_t const b_idx = m->size;
        for (uint32_t i = 0; i <= cmd->idx; i++) {
                // render argv to calculate argv_len 
                if (cmd->at[i].id == ARQ_CMD_SHORT_OPTION) {
                        arq_msg_append_chr(m, '-');
                        arq_msg_append_chr(m, cmd->at[i].at[0]);
                        arq_msg_append_chr(m, '_');
                } else if (cmd->at[i].id == ARQ_CMD_LONG_OPTION) {
                        arq_msg_append_nchr(m, '-', 2);
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, '_');
                } else {
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, '_');
                }
        }
        arq_msg_append_lf(m);
        uint32_t const c_idx = m->size;
        uint32_t const argv_len = c_idx - b_idx;

        uint32_t const shift_right = b_idx - a_idx;
        for (uint32_t i = 0; i < shift_right; i++) {
                uint32_t const idx = b_idx - 1 - i;
                m->at[idx + argv_len] = m->at[idx];
        }

        uint32_t const d_idx = m->size;
        for (uint32_t i = 0; i <= cmd->idx; i++) {
                // render argv once more for moving argv
                if (cmd->at[i].id == ARQ_CMD_SHORT_OPTION) {
                        arq_msg_append_chr(m, '-');
                        arq_msg_append_chr(m, cmd->at[i].at[0]);
                        arq_msg_append_chr(m, ' ');
                } else if (cmd->at[i].id == ARQ_CMD_LONG_OPTION) {
                        arq_msg_append_nchr(m, '-', 2);
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, ' ');
                } else {
                        arq_msg_append_cstr(m, cmd->at[i].at);
                        arq_msg_append_chr(m, ' ');
                }
        }
        arq_msg_append_lf(m);

        {
                // moving rendered argv 
                for (uint32_t i = 0; i < argv_len; i++) {
                        m->at[i + a_idx] =  m->at[i + d_idx];
                }
                //delete argv 
                m->size = d_idx;
        }
}

void arq_msg_append_str(Arq_msg *m, char const *str, uint32_t const size) {
        for (uint32_t i = 0; i < size; i++) {
                arq_msg_append_chr(m, str[i]);
        }
}
