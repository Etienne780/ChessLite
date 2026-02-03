project "CoreChessLib"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    
    SetTargetAndObjDirs("%{prj.name}")

    files {
        "src/**.cpp",
        "src/**.c",
        "include/**.h",
        "include/**.hpp"
    }

    includedirs { 
        "include", 
        "include/%{prj.name}",
        "%{wks.location}/CoreLib/include"
    }

    links {
        "CoreLib"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
        buildoptions { "/MTd" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "Full"
        buildoptions { "/MT" }

    filter "configurations:Distribution"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "Full"
        buildoptions { "/MT" }