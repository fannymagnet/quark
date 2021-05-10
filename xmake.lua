-- project
set_project("quark")

-- set xmake minimum version
set_xmakever("2.5.1")

-- set project version
set_version("1.6.6", {build = "%Y%m%d%H%M"})

-- set language
set_languages("c99", "cxx20")

-- include project sources
includes("src", "test", "demo")
