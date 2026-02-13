project "Server"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

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
        "%{wks.location}/vendor/SDL3/include",
        "%{wks.location}/vendor/SDL3_net/include",
        "%{wks.location}/vendor/whereami/include"
    }

    libdirs {
        "%{wks.location}/vendor/SDL3/lib/x64",
        "%{wks.location}/vendor/SDL3_net/lib/x64",
        "%{wks.location}/vendor/whereami/lib/x64"
    }

    links {
        "libssl",
        "libcrypto",
        "SDL3",
        "SDL3_net",
        "whereami"
    }

    ApplyCommonConfigs()

    filter "configurations:Debug"
        includedirs {
            "%{wks.location}/vendor/mysql-debug/include"
        }
        libdirs {
            "%{wks.location}/vendor/mysql-debug/lib/x64/vs14"
        }
        links {
            "mysqlcppconn"
        }
        postbuildcommands {
            '{MKDIR} "%{cfg.targetdir}"',
            '{COPY} "config.otn" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/vendor/mysql-debug/lib/x64/*.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/vendor/SDL3/lib/x64/*.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/vendor/SDL3_net/lib/x64/*.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/vendor/whereami/lib/x64/*.dll" "%{cfg.targetdir}"'
        }

    filter "configurations:Release"
        includedirs {
            "%{wks.location}/vendor/mysql/include"
        }
        libdirs {
            "%{wks.location}/vendor/mysql/lib/x64/vs14"
        }
        links {
            "mysqlcppconn"
        }
        postbuildcommands {
            '{MKDIR} "%{cfg.targetdir}"',
            '{COPY} "config.otn" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/vendor/mysql/lib/x64/*.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/vendor/SDL3/lib/x64/*.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/vendor/SDL3_net/lib/x64/*.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/vendor/whereami/lib/x64/*.dll" "%{cfg.targetdir}"'
        }

    filter {}