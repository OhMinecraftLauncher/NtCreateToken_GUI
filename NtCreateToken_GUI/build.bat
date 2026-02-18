@echo off
dotnet publish -r win-x86 --self-contained false -p:PublishSingleFile=true -c Release
echo.
pause