target("demo")
    set_kind("binary")
    add_files("main.cpp")
	add_includedirs("$(projectdir)/deps/easyloggingpp")
    add_includedirs("$(projectdir)/src")
    add_deps("quark-base")


target("client")
    set_kind("binary")
    add_files("client.cpp")

target("server")
    set_kind("binary")
    add_files("server.cpp")