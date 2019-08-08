@echo off

cmake -G "Visual Studio 15 2017 Win64" .. || exit /b 1

MSBuild violetminer.sln /p:Configuration=Release /m || exit /b 1 

cd Release

argon2-cpp-test.exe
