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
        nob_cmd_append(&source_paths, "./source/arq_queue.c");
        nob_cmd_append(&source_paths, "./source/arq_options.c");
        nob_cmd_append(&source_paths, "./source/arq_cmd.c");
        nob_cmd_append(&source_paths, "./source/arq_conversion.c");
        nob_cmd_append(&source_paths, "./source/arq_msg.c");
        nob_cmd_append(&source_paths, "./source/arq_arena.c");
        nob_cmd_append(&source_paths, "./source/arq_immediate.c");
}

bool arq_build(bool const clean) {
        if (clean) {
                return true;
        }
        nob_log(NOB_INFO, "BUILD: arq ----> cmd line argument library");
        Nob_Cmd c_ompiler = {0};
        //nob_cmd_append(&cmd, "gcc", "-O0", "-ggdb", "-pedantic");
        //nob_cmd_append(&c_ompiler, "gcc", "-O3", "-Wall", "-Wextra", "-pedantic", "-Wno-parentheses"); 
#if 0
        //nob_cmd_append(&c_ompiler, "gcc", "-O3", "-Wall", "-Wextra", "-pedantic", "-Wparentheses"); 
        nob_cmd_append(&c_ompiler, "gcc", "-std=c99", "-O3", "-Wall", "-Wextra", "-pedantic", "-Wparentheses"); 
        nob_cmd_append(&c_ompiler, "-mshstk");
#endif
#if 1
        nob_cmd_append(&c_ompiler, "gcc", "-std=c89", "-DARQ_LOG_TOKENIZER", 
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

bool download_build_googleTest(bool const clean) {
        #if 1
                #define GTEST_URL "https://github.com/google/googletest/archive/refs/heads/"
                #define GTEST_COMMIT "main"
        #else
                #define GTEST_URL "https://github.com/google/googletest/archive/"
                #define GTEST_COMMIT "76bb2afb8b522d24496ad1c757a49784fbfa2e42"
        #endif
        bool ok = true;
        if (clean) {
                ok &= nob_remove("googletest.zip");
                ok &= nob_remove("googletest-"GTEST_COMMIT);
                ok &= nob_remove("googletest");
                return ok;
        }
        if (nob_file_exists("googletest")) return true;
        nob_log(NOB_INFO, "DOWNLOAD BUILD: googletest");
        Nob_Cmd cmd = {0};
        nob_cmd_append(&cmd, "curl");
        nob_cmd_append(&cmd, "-L");
        nob_cmd_append(&cmd,GTEST_URL GTEST_COMMIT".zip");
        nob_cmd_append(&cmd, "--output", "googletest.zip");
        ok &= nob_cmd_run_sync(cmd);
        cmd.count = 0;
        #ifdef _WIN32
                nob_log(NOB_INFO,"xxxxx unzip with tar xxxxx");
                nob_cmd_append(&cmd, "tar", "-xvzf", "googletest.zip");
        #else
                nob_cmd_append(&cmd, "unzip", "googletest.zip");                
        #endif
        ok &= nob_cmd_run_sync(cmd);
        cmd.count = 0;
        ok &= nob_remove("googletest.zip");
        ok &= nob_mkdir_if_not_exists("googletest-"GTEST_COMMIT"/build");
        #ifdef _WIN32
                nob_cmd_append(&cmd, "cmake");
                nob_cmd_append(&cmd, "-DCMAKE_CXX_COMPILER=g++");
                nob_cmd_append(&cmd, "-DCMAKE_CC_COMPILER=gcc");
                nob_cmd_append(&cmd, "-DCMAKE_MAKE_PROGRAM=mingw32-make");
                nob_cmd_append(&cmd, "-G", "MinGW Makefiles");
                nob_cmd_append(&cmd, "-Hgoogletest-"GTEST_COMMIT);
                nob_cmd_append(&cmd, "-DBUILD_SHARED_LIBS=OFF", "-DBUILD_GMOCK=OFF");
                nob_cmd_append(&cmd, "-Bgoogletest-"GTEST_COMMIT"/build");
                ok &= nob_cmd_run_sync(cmd);
                cmd.count = 0;

                nob_cmd_append(&cmd, "mingw32-make", "-C", "googletest-"GTEST_COMMIT"/build");
                ok &= nob_cmd_run_sync(cmd);
                cmd.count = 0;
        #else       
                nob_cmd_append(&cmd, "cmake");
                nob_cmd_append(&cmd, "-Hgoogletest-"GTEST_COMMIT);
                nob_cmd_append(&cmd, "-DBUILD_SHARED_LIBS=OFF", "-DBUILD_GMOCK=OFF", "-Dgtest_disable_pthreads=ON");
                nob_cmd_append(&cmd, "-Bgoogletest-"GTEST_COMMIT"/build");
                ok &= nob_cmd_run_sync(cmd);
                cmd.count = 0;

                nob_cmd_append(&cmd, "make", "-C", "googletest-"GTEST_COMMIT"/build");                
                ok &= nob_cmd_run_sync(cmd);
                cmd.count = 0;
        #endif  
        nob_cmd_free(cmd);
        ok &= nob_mkdir_if_not_exists("googletest");
        ok &= nob_mkdir_if_not_exists("googletest/build");
        ok &= nob_mkdir_if_not_exists("googletest/include");
        ok &= nob_rename("googletest-"GTEST_COMMIT"/LICENSE", "googletest/LICENSE");
        ok &= nob_rename("googletest-"GTEST_COMMIT"/build/lib", "googletest/build/lib/");
        ok &= nob_rename("googletest-"GTEST_COMMIT"/googletest/include/gtest", "googletest/include/gtest/");
        ok &= nob_remove("googletest-"GTEST_COMMIT);
        return ok;
}

#if 1
bool unittests_build(bool const clean) {
        if (clean) {
                return true;
        }
        bool ok = true;
        nob_log(NOB_INFO, "BUILD: otest ----> unit tests");
        Nob_Cmd cmd = {0};
        nob_cmd_append(
                &cmd, 
                "g++", "-ggdb", "-O0", "-std=c++20",
                "-Wall", "-Wextra", "-pedantic",
                "-Wno-parentheses", "-Wno-missing-field-initializers"
        );
        nob_cmd_append(&cmd, "-I", "googletest/include/"); 
        nob_cmd_append(&cmd, "-I", "source/"); 
        nob_cmd_append(&cmd, "-L", "googletest/build/lib/");
        nob_cmd_append(&cmd, "-o", "build/test");
        nob_cmd_append(&cmd, "./source/arq.c");
        nob_cmd_append(&cmd, "./source/arq_symbols.c");
        nob_cmd_append(&cmd, "./source/arq_queue.c");
        nob_cmd_append(&cmd, "./source/arq_options.c");
        nob_cmd_append(&cmd, "./source/arq_cmd.c");
        nob_cmd_append(&cmd, "./source/arq_conversion.c");
        nob_cmd_append(&cmd, "./source/arq_msg.c");
        nob_cmd_append(&cmd, "./source/arq_arena.c");
        nob_cmd_append(&cmd, "./source/arq_immediate.c");
        nob_cmd_append(&cmd, "unittests/tst_arq_arena.c");
        nob_cmd_append(&cmd, "unittests/tst_arq_queue.c");
        nob_cmd_append(&cmd, "unittests/tst_arq.c");
        nob_cmd_append(&cmd, "-lgtest", "-lgtest_main");
        ok &= nob_cmd_run_sync(cmd);
        cmd.count = 0;
        nob_cmd_free(cmd);
        return ok;
}
#else
bool unittests_build(bool const clean) {
        if (clean) {
                return true;
        }
        bool ok = true;
        nob_log(NOB_INFO, "BUILD: otest ----> unit tests");
        Nob_Cmd cmd = {0};
        nob_cmd_append(
                &cmd, 
                "g++", "-ggdb", "-O0", "-std=c++20",
                "-Wall", "-Wextra", "-pedantic",
                "-Wno-parentheses", "-Wno-missing-field-initializers"
        );
        nob_cmd_append(&cmd, "-I", "googletest/include/"); 
        nob_cmd_append(&cmd, "-I", "source/"); 
        nob_cmd_append(&cmd, "-L", "googletest/build/lib/");
        nob_cmd_append(&cmd, "-o", "build/test");
        nob_cmd_append(&cmd, "source/arq_arena.c");
        nob_cmd_append(&cmd, "source/arq_queue.c");
        nob_cmd_append(&cmd, "unittests/tst_arq_arena.c");
        nob_cmd_append(&cmd, "unittests/tst_arq_queue.c");
        nob_cmd_append(&cmd, "unittests/tst_arq.c");
        nob_cmd_append(&cmd, "-lgtest", "-lgtest_main");
        ok &= nob_cmd_run_sync(cmd);
        cmd.count = 0;
        nob_cmd_free(cmd);
        return ok;
}

#endif

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
        ok &= download_build_googleTest(flag.clean);        

        create_source_paths();
        create_include_paths();
        // ok &= arq_build(flag.clean);
        ok &= unittests_build(flag.clean);

        if (!ok) {
                nob_log(NOB_ERROR, "Done  => One or more errors occurred! %llu ms\n", nob_millis() - t_start);
                return false;
        }
        nob_log(NOB_INFO ,"Successful done! %llu ms\n", nob_millis() - t_start);
        return  0;
}
