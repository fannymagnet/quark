-- project
set_project("quark")

-- set xmake minimum version
set_xmakever("2.5.1")

-- set project version
set_version("1.0.0", {build = "%Y%m%d%H%M"})

-- set language
set_languages("c99", "cxx20")

-- add_cxxflags("-fcoroutines")

-- include project sources
includes("src", "test", "demo")

task("ie")
    on_run("scripts/tasks/init_env")
    set_menu {
        usage = "xmake ie",
        description = "初始化开发环境!",
        options = {}
    } 

task("dc")
    on_run("scripts/tasks/dist_clean")
    set_menu {
        usage = "xmake dc",
        description = "清理中间文件!",
        options = {}
    }

task("vsc")
    on_run("scripts/tasks/vsc_cc")
    set_menu {
        usage = "xmake vsc",
        description = "generate compile_commands.json for vscode",
        options = {}
    }