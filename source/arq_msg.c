#include "arq_msg.h"
#include <assert.h>
#include <string.h>

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

void arq_msg_append_str(Arq_msg *m, char const *str, uint32_t const size) {
        for (uint32_t i = 0; i < size; i++) {
                arq_msg_append_chr(m, str[i]);
        }
}
