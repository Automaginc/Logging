project "Logging"
    kind "StaticLib"
    language "C++"
    cppdialect "C++23"
    architecture "x86_64"

    files { "src/Logging.cpp", "include/**.hpp" }

    includedirs { "include" }

    externalincludedirs { "rang/include", "StackWalker/Main/StackWalker" }
    filter { "system:linux", "action:gmake" }
        links { "stdc++exp" }
    filter { "system:macos", "action:gmake" }
        links { "stdc++exp" }

    filter { "system:windows" }
        defines { "WINDOWS" }
    filter { "system:linux" }
        defines { "UNIX", "LINUX" }
    filter { "system:solaris" }
        defines { "UNIX", "SOLARIS" }
    filter { "system:bsd" }
        defines { "UNIX", "BSD" }
    filter { "system:macos" }
        defines { "UNIX", "MACOS" }

    filter { "configurations:Debug" }
        defines { "DEBUG", "NRELEASE" }
        symbols "On"

    filter { "configurations:Release" }
        defines { "RELEASE", "NDEBUG" }
        optimize "On"
