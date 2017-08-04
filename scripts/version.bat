@echo off

for /f "tokens=*" %%m in ('git rev-list -1 HEAD -- .') do (
   set commit=%%m
)
for /f "tokens=*" %%i in ('git describe --tags --always %commit%') do (
   set ver=%%i
)
set test=-
if exist src\version.h (
  for /F "TOKENS=2,3*" %%j in (version.h) do ( 
    set test=%%j
  ) 
)
for /f "tokens=*" %%k in ('git show -s --format^=format:%%ci %ver%') do (
    set datever=%%k
)

if not %test%=="%ver%" (  
  echo #ifndef VERSION_H > src\version.h
  echo #define VERSION_H >> src\version.h
  echo #define VER_REV "%datever% (%ver%)" >> src\version.h
  echo #endif >> src\version.h
)