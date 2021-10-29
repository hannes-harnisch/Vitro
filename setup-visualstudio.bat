@echo off
call premake5 vs2022
if %ERRORLEVEL% neq 0 (pause)