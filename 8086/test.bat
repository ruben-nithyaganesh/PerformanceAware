@echo off
del %1
nasm %1.asm
sim8086 %1
