@echo off

set DB_USER=root
set DB_PASS=root
set DB_HOST=127.0.0.1
set DB_NAME=game

IF "%~1" == "" GOTO PrintHelp
IF "%~1" == "help" GOTO PrintHelp
IF "%~1" == "compile" GOTO Compile
IF "%~1" == "db-start" GOTO DbStart
IF "%~1" == "db-stop" GOTO DbStop
IF "%~1" == "db-status" GOTO DbStatus
IF "%~1" == "db-restart" GOTO DbRestart
IF "%~1" == "db-tables" GOTO DbTables

vendor\premake5\premake5.exe %1
GOTO Done

:PrintHelp

echo.
echo Enter 'build.bat [action]' where action is one of the following:
echo.
echo   clean             Remove all binaries and intermediate binaries and project files.
echo   compile           Will generate make file then compile using the make file.
echo   ...
echo   DB:
echo   db-start	         Starts the MySQL DB
echo   db-stop           Stops the MySQL DB
echo   db-status         Returns the status of the DB
echo   db-reset          Stops, removes volumes, and starts the DB fresh
echo   db-tables         Lists all tables in the game database
GOTO Done

:Compile
vendor\premake5\premake5.exe vs2022

if not defined DevEnvDir (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
)

set solutionFile=Game.sln
msbuild /t:Build /p:Configuration=Debug /p:Platform=x64 %solutionFile%
GOTO Done

:DbStart
echo Starting Docker database...
docker compose up -d

REM Check if docker compose failed (e.g. port already in use)
IF ERRORLEVEL 1 (
    echo.
    echo ERROR: Docker could not start the database container.
    echo.
    echo Most common reason:
    echo   Port 3306 is already in use.
    echo.
    echo To find the process using port 3306 run:
    echo   netstat -ano ^| findstr :3306
    echo.
    echo Then identify the process with:
    echo   tasklist ^| findstr 'PID'
    echo.
    echo Replace 'PID' with the number shown in netstat.
    echo.
    echo If it is MySQL, you can stop it with:
    echo   net stop MySQL80
    echo.
    echo Make sure to run the command in an Administrator console 'CMD or PowerShell'.
    echo.
    GOTO Done
)

echo Waiting for MySQL to be ready...

:WaitLoop
docker exec game-db mysqladmin ping -u %DB_USER% -p%DB_PASS% --silent > nul 2>&1

REM If container is not reachable yet, wait
IF ERRORLEVEL 1 (
    timeout /t 2 > nul
    GOTO WaitLoop
)

echo MySQL is ready.
GOTO Done

:DbStop
echo Stopping Docker database...
docker compose down -v
GOTO Done

:DbStatus
echo Docker container status:
docker ps
GOTO Done

:DbRestart
echo Restarting Docker database...
docker compose down
docker compose up -d
GOTO Done

:DbTables
echo Showing tables in database %DB_NAME%...
docker exec -it game-db mysql -u %DB_USER% -p%DB_PASS% -e "USE %DB_NAME%; SHOW TABLES;"
GOTO Done

:Done