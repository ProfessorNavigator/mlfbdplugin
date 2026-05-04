# MLFBDPlugin

Плагин для [MyLibrary](https://github.com/ProfessorNavigator/mylibrary). Создаёт книги в формате fbd (файл книги упаковывается в архив вместе с fbd файлом, содержащем тег `description` формата [fb2](https://github.com/gribuser/fb2)). Возможно добавление любых файлов, не только книг.

## Установка
`cmake -DCMAKE_BUILD_TYPE=release -S <path_to_sources> -B <path_to_build_dir>` \
`cmake --build <path_to_build_dir>` \
`cmake --install <path_to_build_dir>`

По умолчанию плагин будет установлен в `/usr/local/<libdir>/MLFBDPlugin`

## Зависимости
Для сборки MLFBDPlugin нужна программа [MyLibrary](https://github.com/ProfessorNavigator/mylibrary) (версии >= 5.0).

## Использование
После установки запустите [MyLibrary](https://github.com/ProfessorNavigator/mylibrary) и откройте окно со списком плагинов. Укажите путь до библиотеки libMLFBDPlugin. Интерфейс MLFBDPlugin интуитивно понятен. Если у вас всё же возникли вопросы, то см. документацию по тегу `description` формата [fb2](https://github.com/gribuser/fb2).

## Лицензия

GPLv3 (см. `COPYING`).

## Поддержка

Если есть желание поддержать проект, то можно пройти по следующей ссылке: [поддержка](https://yoomoney.ru/to/4100117795409573)

## Контакты автора

Вопросы, пожелания, предложения и отзывы можно направлять на следующий адрес: \
bobilev_yury@mail.ru 
