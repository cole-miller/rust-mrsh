use std::path::PathBuf;

fn main() {
    println!(
        "cargo:include={}",
        dunce::canonicalize("./mrsh/include/mrsh")
            .unwrap()
            .display()
    );

    cc::Build::new()
        // mrsh's build allows these warnings so we do, too
        .flag_if_supported("-Wno-missing-braces")
        .flag_if_supported("-Wno-missing-field-initializers")
        .flag_if_supported("-Wno-unused-parameter")
        .flag_if_supported("-Wno-unused-result")
        .include("mrsh/include")
        .files([
            // libmrsh_objects
            "mrsh/arithm.c",
            "mrsh/array.c",
            "mrsh/ast_print.c",
            "mrsh/ast.c",
            "mrsh/buffer.c",
            "mrsh/builtin/alias.c",
            "mrsh/builtin/bg.c",
            "mrsh/builtin/break.c",
            "mrsh/builtin/builtin.c",
            "mrsh/builtin/cd.c",
            "mrsh/builtin/colon.c",
            "mrsh/builtin/command.c",
            "mrsh/builtin/dot.c",
            "mrsh/builtin/eval.c",
            "mrsh/builtin/exec.c",
            "mrsh/builtin/exit.c",
            "mrsh/builtin/export.c",
            "mrsh/builtin/false.c",
            "mrsh/builtin/fg.c",
            "mrsh/builtin/getopts.c",
            "mrsh/builtin/hash.c",
            "mrsh/builtin/jobs.c",
            "mrsh/builtin/pwd.c",
            "mrsh/builtin/read.c",
            "mrsh/builtin/return.c",
            "mrsh/builtin/set.c",
            "mrsh/builtin/shift.c",
            "mrsh/builtin/times.c",
            "mrsh/builtin/trap.c",
            "mrsh/builtin/true.c",
            "mrsh/builtin/type.c",
            "mrsh/builtin/ulimit.c",
            "mrsh/builtin/umask.c",
            "mrsh/builtin/unalias.c",
            "mrsh/builtin/unset.c",
            "mrsh/builtin/unspecified.c",
            "mrsh/builtin/wait.c",
            "mrsh/getopt.c",
            "mrsh/hashtable.c",
            "mrsh/parser/arithm.c",
            "mrsh/parser/parser.c",
            "mrsh/parser/program.c",
            "mrsh/parser/word.c",
            "mrsh/shell/arithm.c",
            "mrsh/shell/entry.c",
            "mrsh/shell/job.c",
            "mrsh/shell/path.c",
            "mrsh/shell/process.c",
            "mrsh/shell/redir.c",
            "mrsh/shell/shell.c",
            "mrsh/shell/task/pipeline.c",
            "mrsh/shell/task/simple_command.c",
            "mrsh/shell/task/task.c",
            "mrsh/shell/task/word.c",
            "mrsh/shell/trap.c",
            "mrsh/shell/word.c",
        ])
        .compile("mrsh");

    let bindings = bindgen::builder()
        .header("mrsh.h")
        .layout_tests(false)
        .prepend_enum_name(false)
        .generate_comments(false)
        .size_t_is_usize(true)
        .allowlist_type("mrsh_.+")
        .allowlist_function("mrsh_.+")
        .blocklist_item("__ssize_t")
        .blocklist_item("__uint32_t")
        .generate()
        .unwrap();
    let out_dir = PathBuf::from(std::env::var("OUT_DIR").unwrap());
    bindings.write_to_file(out_dir.join("bindings.rs")).unwrap();
}
