@echo off
call premake5 vs2019
if %ERRORLEVEL% neq 0 (pause)