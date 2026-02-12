project "Server"
    kind "ConsoleApp"
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
        "%{wks.location}/vendor/mysql/include",
        "%{wks.location}/vendor/SDL3/include",
        "%{wks.location}/vendor/SDL3_net/include"
    }

    libdirs {
        "%{wks.location}/vendor/mysql/lib/x64/vs14",
        "%{wks.location}/vendor/SDL3/lib/x64",
        "%{wks.location}/vendor/SDL3_net/lib/x64"
    }

    links {
        "mysqlcppconn",
        "libssl",
        "libcrypto",
        "SDL3",
        "SDL3_net"
    }

    ApplyCommonConfigs()

    postbuildcommands {
        '{MKDIR} "%{cfg.targetdir}"',
        '{COPY} "config.otn" "%{cfg.targetdir}"',
        '{COPY} "%{wks.location}/vendor/mysql/lib/x64/*.dll" "%{cfg.targetdir}"',
        '{COPY} "%{wks.location}/vendor/SDL3/lib/x64/*.dll" "%{cfg.targetdir}"',
        '{COPY} "%{wks.location}/vendor/SDL3_net/lib/x64/*.dll" "%{cfg.targetdir}"'
    }