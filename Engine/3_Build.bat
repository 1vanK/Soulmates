:: Указываем путь к vsvars32.bat. Также необходим путь к System32.
set "PATH=c:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\;c:\Windows\System32"
:: Устанавливаем переменные среды.
call vsvars32.bat
:: Компилируем релизную версию движка.
devenv Build/INSTALL.vcxproj /build Release
:: Ждём нажатия ENTER для закрытия консоли.
pause
