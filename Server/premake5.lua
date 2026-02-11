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
        "%{wks.location}vendor/mysql/include"
    }

    libdirs {
        "%{wks.location}vendor/mysql/lib/x64"
    }

    links {
        "mysqlcppconn",
        "libssl",
        "libcrypto"
    }

    ApplyCommonConfigs()

    -- Copy mysql dlls
    postbuildcommands {
        '{MKDIR} "%{cfg.targetdir}"',
        '{COPY} "%{wks.location}/vendor/mysql/lib/x64/*.dll" "%{cfg.targetdir}"',
    }