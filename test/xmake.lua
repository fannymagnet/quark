add_requires("catch2")

target("test")
    set_kind("binary")
    add_deps("quark-base")
    add_includedirs("$(projectdir)/src")
    add_files("test.cpp")
    add_packages("catch2")
