add_rules("mode.debug", "mode.release")
set_defaultmode("mode.debug")

add_requires("raylib")

target("playground")
    set_languages("c++23")
    add_files("src/*.cpp")
    add_packages("raylib")
