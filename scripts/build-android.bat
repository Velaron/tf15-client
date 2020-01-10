@echo off
setlocal

if not defined ANDROID_NDK (
echo ANDROID_NDK variable is not defined
exit /B
)

if not defined TOOLCHAIN (
set TOOLCHAIN=4.9
)

if not defined ARCHS (
set ARCHS=armeabi-v7a armeabi x86 aarch64
)

if not defined API (
set API=21
)

for %%i in (%ARCHS%) do (
call ./waf configure -o "build/%%i" -T debug --android=%%i,%TOOLCHAIN%,%API% --check-c-compiler=clang --check-cxx-compiler=clang++ build -v
call ./waf install --destdir="build/android" -v
)

call ./waf configure -T debug --build-apk build -v