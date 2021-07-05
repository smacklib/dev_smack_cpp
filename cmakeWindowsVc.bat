set buildDir=.\build

rem Below the rm command is used, requires an installed cygwin.
rem Found no way to delete a dir tree on windows. rmdir rd del
rem are not cool

IF EXIST %buildDir% rm -rf %buildDir%
md %buildDir%
cd %buildDir%

rem Generation of tests and examples is enabled here.
"%programfiles%"\CMake\bin\cmake.exe -DENABLE_TESTS=ON -DENABLE_EXAMPLES=ON -G "Visual Studio 16 2019" ..

cd ..
