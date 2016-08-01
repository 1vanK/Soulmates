:: Указываем путь к cmake.exe.
set "PATH=c:\Programs\CMake\bin\"
:: Создаем проекты для Visual Studio 2015.
Urho3D/cmake_vs2015.bat Build -DURHO3D_OPENGL=ON -DURHO3D_STATIC_RUNTIME=ON -DURHO3D_SAMPLES=OFF -DURHO3D_LUA=OFF -DURHO3D_ANGELSCRIPT=OFF -DURHO3D_TOOLS=OFF
:: Ждём нажатия ENTER для закрытия консоли.
pause
