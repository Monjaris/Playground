{
    depfiles_format = "gcc",
    files = {
        "src/main.cpp"
    },
    depfiles = "main.o: src/main.cpp src/game.hpp src/player.hpp src/joystick.hpp  src/core.h src/common.h src/parser.hpp src/bullet.hpp\
",
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++23",
            "-DNDEBUG"
        }
    }
}