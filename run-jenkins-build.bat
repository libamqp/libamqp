@echo off
set DEVENV=C:\Windows\Microsoft.NET\Framework\v3.5\MSBuild

set DEVENV=C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe
if exist "%DEVENV%" goto devenv_found

set DEVENV=C:\Program Files(x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe
if exist "%DEVENV%" goto devenv_found

echo "Please set DEVENV to the location of the devenv.exe program.
got end

:devenv_found

echo "Building UnitTest++"
cd 3rd-party\UnitTest++


"%DEVENV%" UnitTest++.vsnet2008.sln /Rebuild Debug
"%DEVENV%" UnitTest++.vsnet2008.sln /Rebuild Release


cd ..\..
echo "Running CMake"
del CMakeCache.txt /q
del CMakeFiles /s /q
cmake CMakeLists.txt

echo "Building Libamqp"
"%DEVENV%" libamqp.sln /Rebuild Debug
"%DEVENV%" libamqp.sln /Build Debug /project RUN_TESTS


:end

