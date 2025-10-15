# MLFBDPlugin

Плагин для [MyLibrary](https://github.com/ProfessorNavigator/mylibrary). Создаёт книги в формате fbd (файл книги упаковывается в архив вместе с fbd файлом, содержащем тэг `description` формата [fb2](https://github.com/gribuser/fb2)). Возможно добавление любых файлов, не только книг.

## Установка
MLFBDPlugin может быть установлен в любую директорию операционной системы.
Единственное обязательное требование - файл перевода (MLFBDPlugin.mo) должен находиться в директории <path_to_mylibrary_executable>/../share/locale/<language_code>/LC_MESSAGES. В противном случае в плагине будет использоваться английский язык.

### Linux
`git clone https://github.com/ProfessorNavigator/mlfbdplugin.git` \
`cd mlfbdplugin` \
`cmake -DCMAKE_BUILD_TYPE=Release -B _build`\
`cmake --build _build`\
`cmake --install _build`

Вам могут потребоваться привилегии суперпользователя для выполнения последней команды.

Также вам может потребоваться задать префикс опцией CMAKE_INSTALL_PREFIX (перфикс по умолчанию `/usr/local`).

### Windows
Для сборки и установки вам потребуется [MSYS2](https://www.msys2.org/). Кроме того вам нужно установить зависимости из секции `Зависимости`. После установки необходимых зависимостей откройте консоль MinGW и выполните следующие команды (в примере предполагается, что скачивание кода происходит в C:\MLFBDPlugin):

`mkdir -pv /c/MLFBDPlugin`\
`cd /c/MLFBDPlugin` \
`git clone https://altlinux.space/professornavigator/mlfbdplugin.git` \
`cd mlfbdplugin`\
`cmake -DCMAKE_BUILD_TYPE=Release -B ../_build`\
`cmake --build ../_build`\
`cmake --install ../_build`

Вам также обязательно необходимо указать префикс опцией CMAKE_INSTALL_PREFIX (префикс может быть например `/ucrt64` или `/mingw64`).

## Зависимости
Для сборки MLFBDPlugin нужна программа [MyLibrary](https://github.com/ProfessorNavigator/mylibrary) (версии >= 4.0), собранная с опцией USE_PLUGINS, установленной в `ON`. Также вам необходима библиотека [Magick++](https://imagemagick.org/Magick++/). Кроме того вам может потребоваться git (для клонирования репозитория).

### Windows
В Windows библиотеки [MyLibrary](https://github.com/ProfessorNavigator/mylibrary) обязательно должны находиться в одной из директорий, указанных в системной переменной Path. Или MyLibrary должна быть установлена с использованием MSYS2.

## Использование
После установки запустите [MyLibrary](https://github.com/ProfessorNavigator/mylibrary) и откройте окно со списком плагинов. Укажите путь до библиотеки libmlfbdplugin. Интерфейс MLFBDPlugin интуитивно понятен. Если у вас всё же возникли вопросы, то см. документацию по тэгу `description` формата [fb2](https://github.com/gribuser/fb2).

## Лицензия

GPLv3 (см. `COPYING`).

## Поддержка

Если есть желание поддержать проект, то можно пройти по следующей ссылке: [поддержка](https://yoomoney.ru/to/4100117795409573)

## Контакты автора

Вопросы, пожелания, предложения и отзывы можно направлять на следующий адрес: \
bobilev_yury@mail.ru 
