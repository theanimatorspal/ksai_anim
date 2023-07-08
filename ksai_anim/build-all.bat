REM build script for KSAI Anim
@ECHO OFF
SetLocal EnableDelayedExpansion

REM get a list of all the .c files
SET cFileNames=
FOR /R %%f in (*.c) do (
    SET cFileNames=!cFileNames! %%f
)

REM echo "Files:" %cFileNames%

SET assembly=ksai_anim
SET compilerFlags=-g
REM -Wall -Werror
SET includeFlags=-Isrc -I%VULKAN_SDK%/Include
SET linkerFlags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib -lSDL2 -lassimp -lCommondlg32
SET defines=-D_DEBUG
echo "Building %assembly%"
clang %cFileNames% %compilerFlags% -o bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%
