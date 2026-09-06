#define GEN_COMPILE_COMMANDS_IMPLEMENTATION
#include "gen_compile_commands.h"

#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_DIR "build"
#define TOOLS_DIR "tools"

bool mkdir_build_tools(bool const clean) {
        if (clean) {
                nob_remove(TOOLS_DIR);
                return nob_remove(BUILD_DIR);
        }
        bool ok = nob_mkdir_if_not_exists(TOOLS_DIR);
        ok &= nob_mkdir_if_not_exists(BUILD_DIR);
        printf("\n");
        return ok;
}

bool download_tools_cgtest(bool const clean) {
        if (clean) {
                return true;
        }
        bool ok = true;
        if (!nob_file_exists("tools/cgtest")) { 
                printf("-----------------------------------------------------------------\n");
                ok &= nob_mkdir_if_not_exists("tools/cgtest");
                printf("\n");
        }
        if (!nob_file_exists("tools/cgtest/cgtest.c")) { 
                Nob_Cmd cmd = {0};
                nob_cmd_append(&cmd, "curl");
                nob_cmd_append(&cmd, "-L");
                nob_cmd_append(&cmd, "https://github.com/bartgeier/cgtest/raw/refs/tags/v0.1.3/cgtest.c");
                nob_cmd_append(&cmd, "--output", "tools/cgtest/cgtest.c");
                ok &= nob_cmd_run_sync(cmd);
                cmd.count = 0;
                printf("\n");
        }
        if (!nob_file_exists("build/cgtest")) { 
                nob_log(NOB_INFO, "build_cgtest");
                Nob_Cmd cmd = {0};
                nob_cmd_append(&cmd, "gcc");
                nob_cmd_append(&cmd, "tools/cgtest/cgtest.c");
                nob_cmd_append(&cmd, "-o");
                nob_cmd_append(&cmd, "build/cgtest");
                ok &= nob_cmd_run_sync(cmd);
                cmd.count = 0;
                printf("-----------------------------------------------------------------\n");
                printf("\n");
        }
        return ok;
}


Nob_Cmd include_paths = {0};
void create_include_paths(void) {
        nob_cmd_append(&include_paths, "-I", "source");
}

Nob_Cmd source_paths = {0};
void create_source_paths(void) {
        nob_cmd_append(&source_paths, "./source/main.c");
        nob_cmd_append(&source_paths, "./source/arq_main.c");
        nob_cmd_append(&source_paths, "./source/arq_symbols.c");
        nob_cmd_append(&source_paths, "./source/arq_queue.c");
        nob_cmd_append(&source_paths, "./source/arq_lexer.c");
        nob_cmd_append(&source_paths, "./source/arq_log.c");
        nob_cmd_append(&source_paths, "./source/arq_conversion.c");
        nob_cmd_append(&source_paths, "./source/arq_msg.c");
        nob_cmd_append(&source_paths, "./source/arq_arena.c");
        nob_cmd_append(&source_paths, "./source/arq_immediate.c");
}

bool arq_build(bool const clean) {
        if (clean) {
                return true;
        }
        printf("-----------------------------------------------------------------\n");
        uint64_t t_start = nob_millis();
        nob_log(NOB_INFO, "BUILD: arq ----> cmd line argument library");
        Nob_Cmd c_compiler = {0};
        //nob_cmd_append(&cmd, "gcc", "-O0", "-ggdb", "-pedantic");
        //nob_cmd_append(&c_ompiler, "gcc", "-O3", "-Wall", "-Wextra", "-pedantic", "-Wno-parentheses"); 
#if 0
        //nob_cmd_append(&c_ompiler, "gcc", "-O3", "-Wall", "-Wextra", "-pedantic", "-Wparentheses"); 
        nob_cmd_append(&c_ompiler, "gcc", "-std=c99", "-O3", "-Wall", "-Wextra", "-pedantic", "-Wparentheses"); 
        nob_cmd_append(&c_ompiler, "-mshstk");
#endif
#if 0
        nob_cmd_append(&c_ompiler, "gcc", "-std=c89",
                // "-Os", "-s", 
                // "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
                // "-fomit-frame-pointer", "-fno-stack-protector", "-fno-asynchronous-unwind-tables",
                "-Wall", "-Wextra", "-pedantic", "-Wparentheses"
        ); 
#endif
#if 0
        nob_cmd_append(&c_ompiler, "gcc", "-std=c89", "-DARQ_LOG_MEMORY", 
                // "-Os", "-s", 
                // "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
                // "-fomit-frame-pointer", "-fno-stack-protector", "-fno-asynchronous-unwind-tables",
                "-Wall", "-Wextra", "-pedantic", "-Wparentheses"
        ); 
#endif
#if 1
        nob_cmd_append(&c_compiler, "gcc", "-std=c89", "-DARQ_LOG_TOKENIZER",
                // "-Os", "-s", 
                // "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
                // "-fomit-frame-pointer", "-fno-stack-protector", "-fno-asynchronous-unwind-tables",
                "-Wall", "-Wextra", "-pedantic", "-Wparentheses"
        ); 
#endif
#if 0
        nob_cmd_append(&c_compiler, "gcc", "-std=c89", "-DARQ_LOG_TOKENIZER", "-DARQ_LOG_TOKENS", 
                // "-Os", "-s", 
                // "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
                // "-fomit-frame-pointer", "-fno-stack-protector", "-fno-asynchronous-unwind-tables",
                "-Wall", "-Wextra", "-pedantic", "-Wparentheses"
        ); 
#endif
#if 0
        nob_cmd_append(&c_ompiler, "gcc", "-std=c89", "-DARQ_LOG_TOKENIZER", "-Os", "-Wall", "-Wextra", "-pedantic", "-fPIC",
                "-Os", "-s", "-Wl,--gc-sections",
                "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections", "-fomit-frame-pointer",
                "-shared", "-o", "build/libarq.so"
        );
#endif
        Nob_Cmd cmd = {0};
        nob_cmd_append_cmd(&cmd, &c_compiler);
        nob_cmd_append_cmd(&cmd, &include_paths);
        nob_cmd_append(&cmd, "-o", "build/arq");
        nob_cmd_append_cmd(&cmd, &source_paths);
        bool ok = nob_cmd_run_sync(cmd);

        cmd.count = 0;
        nob_cmd_append_cmd(&cmd, &c_compiler);
        nob_cmd_append(&cmd, "-o", "build/arq");
        nob_cmd_append_cmd(&cmd, &include_paths);
        // Language server
        generate_compile_commands("/home/berni/projects/arq", &cmd, &source_paths);

        nob_cmd_free(c_compiler);
        nob_cmd_free(cmd);
        nob_log(NOB_INFO ,"Successful done! %llu ms", nob_millis() - t_start);
        printf("-----------------------------------------------------------------\n");
        printf("\n");
        return ok;
}

bool amalgamate_arq(const bool clean) {
        if (clean) {
                return true;
        }
        printf("-----------------------------------------------------------------\n");
#if 1
        {
                FILE *file_r = fopen("LICENSE", "r");
                if (!file_r) {
                        perror("Failed to open LICENSE");
                        return false;
                }
                FILE *file_w = fopen("build/license.h", "w");
                if (!file_w) {
                        perror("Failed to open file");
                        return false;
                }
                fprintf(file_w, "#if 1\n/*\n");
                char line[1024];
                while (fgets(line, sizeof(line), file_r)) {
                        fprintf(file_w, "%s", line);  // print each line
                } 
                fprintf(file_w, "*/\n#endif");
                fclose(file_r);
                fclose(file_w);
                fflush(stdout);
        }
#endif
        {
                // https://github.com/rindeal/Amalgamate/releases/tag/v0.99.0
                // whereis amalgamate /usr/local/bin
                Nob_Cmd cmd = {0};
                nob_cmd_append(&cmd, "amalgamate", "source/amalgamate.c", "amalgamate/arq.h", "-i", "source");
                bool ok = nob_cmd_run_sync(cmd);
        }
        printf("\n");
        {
                Nob_Cmd c_compiler = {0};
                nob_cmd_append(&c_compiler, "gcc", "-std=c89", // "-DARQ_LOG_TOKENIZER", 
                        // "-Os", "-s", 
                        // "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
                        // "-fomit-frame-pointer", "-fno-stack-protector", "-fno-asynchronous-unwind-tables",
                        "-Wall", "-Wextra", "-pedantic", "-Wparentheses"
                ); 
                Nob_Cmd cmd = {0};
                nob_cmd_append_cmd(&cmd, &c_compiler);
                nob_cmd_append(&cmd, "./amalgamate/example.c");
                nob_cmd_append(&cmd, "-o", "./amalgamate/example");
                nob_cmd_append(&cmd, "-I", "./amalgamate");
                nob_cmd_run_sync(cmd);
        }
        printf("-----------------------------------------------------------------\n");
        printf("\n");
        return true; // ok
}

bool cgtest_run(bool const clean) {
        if (clean) {
                return true;
        }
        bool ok = true;
        Nob_Cmd cmd = {0};
        nob_cmd_append( &cmd, "build/cgtest", "--run", "unittests/cgtest", "--time");
        ok &= nob_cmd_run_sync(cmd);
        cmd.count = 0;
        nob_cmd_free(cmd);
        return ok;
}

int main(int argc, char **argv) {
        bool ok = true;
        NOB_GO_REBUILD_URSELF(argc, argv);
        #ifdef _WIN32
                ok &= nob_remove("nob.exe.old");
                if (nob_file_exists("nob.exe.old")) {
                        nob_log(NOB_INFO, "RM: Don't worry `nob.exe.old` will be deleted next time!");
                }
        #else
                ok &= nob_remove("nob.old");
                printf("\n");
        #endif 

        struct {
                bool clean;
        } flag = { .clean = false };
        while (argc > 0) {
                char const *s = nob_shift_args(&argc, &argv);
                if (strcmp(s, "clean") == 0) flag.clean = true;
        }

        ok &= mkdir_build_tools(flag.clean);
        ok &= download_tools_cgtest(flag.clean);        

        create_source_paths();
        create_include_paths();
        ok &= arq_build(flag.clean);

        ok &= amalgamate_arq(flag.clean);

        ok &= cgtest_run(flag.clean); 

        if (!ok) {
                nob_log(NOB_ERROR, "Done  => One or more errors occurred! %llu ms\n");
                return false;
        }
        return  0;
}
