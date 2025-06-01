#define GEN_COMPILE_COMMANDS_IMPLEMENTATION
#include "gen_compile_commands.h"

#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_DIR "build"

bool create_build_dir(bool const clean) {
        if (clean) {
                return nob_remove(BUILD_DIR);
        }
        return nob_mkdir_if_not_exists(BUILD_DIR);
}

Nob_Cmd include_paths = {0};
void create_include_paths(void) {
        nob_cmd_append(&include_paths, "-I", "source");
}

Nob_Cmd source_paths = {0};
void create_source_paths(void) {
        nob_cmd_append(&source_paths, "./source/main.c");
        nob_cmd_append(&source_paths, "./source/arq.c");
        nob_cmd_append(&source_paths, "./source/arq_symbols.c");
        nob_cmd_append(&source_paths, "./source/arq_stack.c");
        nob_cmd_append(&source_paths, "./source/arq_options.c");
        nob_cmd_append(&source_paths, "./source/arq_cmd.c");
        nob_cmd_append(&source_paths, "./source/arq_tok.c");
        nob_cmd_append(&source_paths, "./source/arq_msg.c");
}

bool arq_build(bool const clean) {
        if (clean) {
                return true;
        }
        nob_log(NOB_INFO, "BUILD: arq ----> cmd line argument library");
        Nob_Cmd c_ompiler = {0};
        //nob_cmd_append(&cmd, "gcc", "-O0", "-ggdb", "-pedantic");
        //nob_cmd_append(&c_ompiler, "gcc", "-O3", "-Wall", "-Wextra", "-pedantic", "-Wno-parentheses"); 
        nob_cmd_append(&c_ompiler, "gcc", "-O3", "-Wall", "-Wextra", "-pedantic", "-Wparentheses"); 

        Nob_Cmd cmd = {0};
        nob_cmd_append_cmd(&cmd, &c_ompiler);
        nob_cmd_append_cmd(&cmd, &include_paths);
        nob_cmd_append(&cmd, "-o", "build/arq");
        nob_cmd_append_cmd(&cmd, &source_paths);
        bool ok = nob_cmd_run_sync(cmd);

        cmd.count = 0;
        nob_cmd_append_cmd(&cmd, &c_ompiler);
        nob_cmd_append(&cmd, "-o", "build/arq");
        nob_cmd_append_cmd(&cmd, &include_paths);
        // Language server
        generate_compile_commands("/home/berni/projects/arq", &cmd, &source_paths);

        nob_cmd_free(c_ompiler);
        nob_cmd_free(cmd);
        return ok;
}

int main(int argc, char **argv) {
        uint64_t t_start = nob_millis();
        bool ok = true;
        NOB_GO_REBUILD_URSELF(argc, argv);
        #ifdef _WIN32
                ok &= nob_remove("nob.exe.old");
                if (nob_file_exists("nob.exe.old")) {
                        nob_log(NOB_INFO, "RM: Don't worry `nob.exe.old` will be deleted next time!");
                }
        #else
                ok &= nob_remove("nob.old");
        #endif 

        struct {
                bool clean;
        } flag = { .clean = false };
        while (argc > 0) {
                char const *s = nob_shift_args(&argc, &argv);
                if (strcmp(s, "clean") == 0) flag.clean = true;
        }
        ok &= create_build_dir(flag.clean);

        create_source_paths();
        create_include_paths();
        ok &= arq_build(flag.clean);

        if (!ok) {
                nob_log(NOB_ERROR, "Done  => One or more errors occurred! %llu ms", nob_millis() - t_start);
                return false;
        }
        nob_log(NOB_INFO ,"Successful done! %llu ms", nob_millis() - t_start);
        return  0;
}
