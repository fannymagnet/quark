target("demo")
    set_kind("binary")
    add_files("main.cpp")
	--add_includedirs("$(projectdir)/deps/easyloggingpp")
    add_includedirs("$(projectdir)/src")
    add_deps("quark-base")


target("client")
    set_kind("binary")
    add_files("client.cpp")
    add_deps("quark-base")
    if is_plat("linux") then
        add_links("pthread")
    end
    add_includedirs("$(projectdir)/src")

target("server")
    set_symbols("debug")
    set_kind("binary")
    add_files("server.cpp")
    add_deps("quark-base")
    add_includedirs("$(projectdir)/src")

target("pingpong")
    set_kind("binary")
    add_files("pingpong.cpp")
    add_deps("quark-base")
    if is_plat("linux") then
        add_links("pthread")
    end
    add_includedirs("$(projectdir)/src")