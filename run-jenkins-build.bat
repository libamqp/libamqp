
set MSBUILD_EXE=C:\Windows\Microsoft.NET\Framework\v3.5\MSBuild


cd 3rd-party\UnitTest++


%MSBUILD_EXE% UnitTest++.vsnet2008.sln  /p:Configuration=Debug /target:Clean
%MSBUILD_EXE% UnitTest++.vsnet2008.sln  /p:Configuration=Release /target:Clean

%MSBUILD_EXE% UnitTest++.vsnet2008.sln  /p:Configuration=Debug 
%MSBUILD_EXE% UnitTest++.vsnet2008.sln  /p:Configuration=Release

cd ..\..

cmake CMakeLists.txt

%MSBUILD_EXE% libamqp.sln  /p:Configuration=Debug /target:Clean
%MSBUILD_EXE% libamqp.sln  /p:Configuration=Release /target:Clean

%MSBUILD_EXE% libamqp.sln  /p:Configuration=Debug
