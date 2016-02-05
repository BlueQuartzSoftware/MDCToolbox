cd %1
cmake.exe ../
msbuild /p:Configuration=Release MDCToolProj.sln
msbuild /p:Configuration=Release PACKAGE.vcxproj
cd ..\
