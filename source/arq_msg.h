#ifndef ARQ_MSG_H
#define ARQ_MSG_H

#include <stdint.h>

typedef struct {
        uint32_t SIZE; /* sizeof(error_buffer), */
        uint32_t size;
        char *at;
} Arq_msg;

#ifdef __cplusplus
exter "C" {
#endif

void arq_msg_clear(Arq_msg *m);
void arq_msg_format(Arq_msg *m);
void arq_msg_append_lf(Arq_msg *m);
void arq_msg_append_chr(Arq_msg *m, char const chr);
void arq_msg_append_nchr(Arq_msg *m, char const chr, uint32_t const num_of_chr);
void arq_msg_append_cstr(Arq_msg *m, char const *cstr);
void arq_msg_append_str(Arq_msg *m, char const *str, uint32_t const size);

#ifdef __cplusplus
}
#endif
#endif

