project "CoreChessLib"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    
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
        "../CoreLib/include"
    }

    links {
        "CoreLib"
    }

    ApplyCommonConfigs();