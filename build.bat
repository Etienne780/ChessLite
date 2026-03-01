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
IF "%~1" == "db-stop-containers" GOTO DbStopContainers
IF "%~1" == "db-status" GOTO DbStatus
IF "%~1" == "db-tables" GOTO DbTables

vendor\premake5\premake5.exe %1
GOTO Done

:PrintHelp

echo.
echo Enter 'build.bat [action]' where action is one of the following:
echo.
echo   clean               Remove all binaries and intermediate binaries and project files.
echo   compile             Will generate make file then compile using the make file.
echo   ...
echo   DB:
echo   db-start	           Starts the MySQL DB
echo   db-stop             Stops the MySQL DB and DELETES its volume
echo   db-stop-containers  Stops the MySQL DB and KEEPS the volume
echo   db-status           Returns the status of the DB
echo   db-tables           Lists all tables in the game database
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

set MAX_WAIT=120
set /a WAITED=0

:WaitLoop
docker exec game-db mysqladmin ping -u %DB_USER% -p%DB_PASS% --silent > nul 2>&1
REM docker logs -f game-db

IF ERRORLEVEL 1 (
    if %WAITED% GEQ %MAX_WAIT% (
        echo ERROR: MySQL did not become ready in %MAX_WAIT% seconds
        GOTO Done
    )
    timeout /t 2 > nul
    set /a WAITED+=2
    GOTO WaitLoop
)

docker exec game-db mysql -u %DB_USER% -p%DB_PASS% -e "USE %DB_NAME%;" > nul 2>&1
IF ERRORLEVEL 1 (
    timeout /t 2 > nul
    set /a WAITED+=2
    if %WAITED% GEQ %MAX_WAIT% (
        echo ERROR: Database %DB_NAME% not ready
        GOTO Done
    )
    GOTO WaitLoop
)

echo MySQL and database '%DB_NAME%' are ready.
GOTO Done

:DbStop
echo Stopping Docker database...
docker compose down -v
GOTO Done

:DbStopContainers
echo Stopping Docker containers (keep volumes)...
docker compose stop
GOTO Done

:DbStatus
echo Docker container status:
docker ps
GOTO Done

:DbTables
echo Showing tables in database %DB_NAME%...
docker exec -it game-db mysql -u %DB_USER% -p%DB_PASS% -e "USE %DB_NAME%; SHOW TABLES;"
GOTO Done

:Done