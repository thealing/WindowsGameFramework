@echo off

gcc -std=c99 -w -o main.exe *.c -l gdi32 -l opengl32 -l glu32 -l ole32 -l windowscodecs -l vfw32
