:: Указываем путь к git.exe.
set "PATH=c:\Program Files (x86)\Git\bin\"
:: Скачиваем репозиторий.
git clone https://github.com/Urho3D/Urho3D.git
:: Переходим в папку со скачанными исходниками.
cd Urho3D
:: Возвращаем состояние репозитория к определённой версии (25 июля 2016).
git reset --hard 56ba0def78fa31e9420b53ffef910bad8a4cea34
:: Ждём нажатия ENTER для закрытия консоли.
pause
