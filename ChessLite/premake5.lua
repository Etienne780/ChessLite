project "ChessLite"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    SetTargetAndObjDirs("%{prj.name}")

    files {
        "src/**.cpp",
        "src/**.c",
        "include/**.h",
        "include/**.hpp",
        "main.cpp"
    }

    includedirs {
        "include",
        "include/%{prj.name}",
        "%{wks.location}/SDLCoreLib/include",
        "%{wks.location}/CoreLib/include",
        "%{wks.location}/CoreChessLib/include"
    }

    links {
        "CoreLib",
        "CoreChessLib",
        "SDLCoreLib"
    }
    
    IncludeSDLCoreLib()
    -- copys the SDL DLLs in to the build path of this project
    CopySDLDLLs()

    ApplyCommonConfigs()