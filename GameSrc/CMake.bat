:: Указываем путь к cmake.exe.
set "PATH=c:\Programs\CMake\bin\"
:: Создаем папку для проекта.
mkdir Build
:: Переходим в эту папку.
cd Build
:: Создаем проект для Visual Studio 2015. Используем те же параметры, что и для движка.
cmake.exe .. -DURHO3D_OPENGL=ON -DURHO3D_STATIC_RUNTIME=ON -DURHO3D_SAMPLES=OFF -DURHO3D_LUA=OFF -DURHO3D_ANGELSCRIPT=OFF -DURHO3D_TOOLS=OFF
:: Ждём нажатия ENTER для закрытия консоли.
pause
