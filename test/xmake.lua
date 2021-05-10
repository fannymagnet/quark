add_requires("catch2")

target("test")
    set_kind("binary")
    add_files("test.cpp")
    add_packages("catch2")
