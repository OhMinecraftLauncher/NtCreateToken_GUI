@echo off
g++ NtCreateToken.cpp -o NtCreateToken.exe -lSecur32 -lpsapi -static -municode
echo.
pause