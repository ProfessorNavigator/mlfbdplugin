# MLFBDPlugin

Plugin for [MyLibrary](https://github.com/ProfessorNavigator/mylibrary). Creates fbd books (book file will be packed into archive alongside fbd file containing [fb2](https://github.com/gribuser/fb2) `description` tag). It is possible to add files of any types, not just books.

## Installation
`cmake -DCMAKE_BUILD_TYPE=release -S <path_to_sources> -B <path_to_build_dir>` \
`cmake --build <path_to_build_dir>` \
`cmake --install <path_to_build_dir>`

Plugin will be installed to `/usr/local/<libdir>/MLFBDPlugin` by default.

## Dependencies
You need [MyLibrary](https://github.com/ProfessorNavigator/mylibrary) (version >= 5.0).

## Usage
After installation open [MyLibrary](https://github.com/ProfessorNavigator/mylibrary), proceed to plugins window and set the path to plugin file. The MLFBDPlugin interface is intuitive. But if you have any questions, see [fb2](https://github.com/gribuser/fb2) `description` tag documentation. 

## License

GPLv3 (see `COPYING`).

## Donation

If you want to help to develop this project, you can assist it by [donation](https://yoomoney.ru/to/4100117795409573)

## Contacts

You can contact author by email \
bobilev_yury@mail.ru
