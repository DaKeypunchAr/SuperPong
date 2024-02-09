--premake5.lua

workspace "SuperPong"
    filename "SuperPong"
    language "C++"
    location "SuperPong"
    configurations { "Debug", "Release" }
    platforms { "x86" }
    defines { "GLEW_STATIC" }
    links {
        "Dependencies/GLEW/glew32s.lib",
        "Dependencies/GLFW/GLFW_32/glfw3.lib",
        "Dependencies/FT/freetype.lib",
        "opengl32.lib"
    }

project "SuperPong"
    location "SuperPong/SuperPong"
    kind "ConsoleApp"
    files
    {
        "SuperPong/SuperPong/**.cpp",
        "SuperPong/SuperPong/**.h",
        "SuperPong/SuperPong/**.glsl",
        "SuperPong/SuperPong/**.vert",
        "SuperPong/SuperPong/**.frag",
    }
    targetdir "SuperPong/bin/SuperPong/%{cfg.buildcfg}"
    objdir "SuperPong/bin/intermediates/SuperPong/%{cfg.buildcfg}"

    includedirs { "Dependencies/include" }

    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines "RELEASE"
        symbols "Off"
        optimize "On"