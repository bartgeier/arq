#ifndef GEN_COMPILE_COMMANDS_H
#define GEN_COMPILE_COMMANDS_H

#include "nob.h"

void generate_compile_commands(
        char const *directory,
        Nob_Cmd const *command,
        Nob_Cmd const *source_files
);

#endif

#define GEN_COMPILE_COMMANDS_IMPLEMENTATION
#ifdef GEN_COMPILE_COMMANDS_IMPLEMENTATION

#include <string.h>
#include <stdio.h>
#include <assert.h>


#define JSON_SIZE (1024 * 1024)
char json_buffer[JSON_SIZE];
typedef struct json_tag {
        uint32_t const SIZE;
        uint32_t size;
        char *at;
}Json_t;

bool write_txt_file(Json_t const *source, char const *path) {
        if (strcmp(path,"-") == 0) { 
                printf("source_code:\n%s", source->at);
                return true;
        } else {
                FILE *file = fopen(path, "w");
                if (file == NULL) {
                        return false;
                }
                fprintf(file, "%s", source->at);
                fclose(file);
                return true;
        }
}

Json_t json = {.SIZE = JSON_SIZE, .size = 0, .at = json_buffer};


void Json_append_cstring(Json_t *self, char const *cstring) {
        uint32_t size = strlen(cstring);
        assert(self->size + size <= self->SIZE);
        for (uint32_t i = 0; i < size; i++) {
                self->at[self->size++] = cstring[i];
        }
}

void Json_append_char(Json_t *self, char chr) {
        assert(self->size + 1 <= self->SIZE);
        self->at[self->size++] = chr;
}

void generate_compile_commands(
        char const *directory,
        Nob_Cmd const *command,
        Nob_Cmd const *source_files
) {
        uint32_t idx = 0;
        Json_append_cstring(&json, "[\n");

        for (uint32_t i = 0; i < source_files->count; i++) {
                Json_append_cstring(&json, "  {\n");

                Json_append_cstring(&json, "    \"directory\": ");
                Json_append_char(&json, '"');
                Json_append_cstring(&json, directory);
                Json_append_char(&json, '"');
                Json_append_cstring(&json, ",\n");

                Json_append_cstring(&json, "    \"command\": ");
                Json_append_char(&json, '"');
                for (uint32_t i = 0; i < command->count; i++) {
                        Json_append_cstring(&json, command->items[i]);
                        if (i + 1 < command->count) {
                                Json_append_char(&json, ' ');
                        }
                }
                Json_append_cstring(&json, "\",\n");
                
                Json_append_cstring(&json, "    \"file\": ");
                Json_append_char(&json, '"');
                Json_append_cstring(&json, source_files->items[i]);
                Json_append_cstring(&json, "\"\n");


                if (i + 1 < source_files->count) {
                        Json_append_cstring(&json, "  },\n");
                } else {
                        Json_append_cstring(&json, "  }\n");
                }

        }
        Json_append_cstring(&json, "]\n");

        write_txt_file(&json, "build/compile_commands.json");
}


#endif
