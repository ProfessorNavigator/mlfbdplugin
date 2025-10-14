# MLFBDPlugin
This is plugin for [MyLibrary](https://github.com/ProfessorNavigator/mylibrary). It creates books in fbd format (book file packed into archive along with fbd file, containing `description` tag of [fb2](https://github.com/gribuser/fb2) format).

## Installation
### Linux
`git clone https://github.com/ProfessorNavigator/mlfbdplugin.git` \
`cd mlfbdplugin` \
`cmake -DCMAKE_BUILD_TYPE=Release -B _build`\
`cmake --build _build`\
`cmake --install _build`

You may need superuser privileges for last command execution.

Also you may need to set CMAKE_INSTALL_PREFIX variable (default prefix is `/usr/local`).

### Windows
You may need [MSYS2](https://www.msys2.org/) project for building and installation. Also you need to install dependencies from `Dependencies` section. After all dependencies have been installed, open MinGW terminal and execute following commands (code will be downloaded to C:\MLFBDPlugin in example):

`mkdir -pv /c/MLFBDPlugin`\
`cd /c/MLFBDPlugin` \
`git clone https://github.com/ProfessorNavigator/mlfbdplugin.git` \
`cd mlfbdplugin`\
`cmake -DCMAKE_BUILD_TYPE=Release -B ../_build`\
`cmake --build ../_build`\
`cmake --install ../_build`

Also you must set prefix by CMAKE_INSTALL_PREFIX option (it can be `/uctr64` or `/mingw64` for example).

## Dependencies
You need [MyLibrary](https://github.com/ProfessorNavigator/mylibrary) (version >= 4.0), built with option USE_PLUGINS set to `ON`. Also you may need [Magick++](https://imagemagick.org/Magick++/) and git (last one - only to clone repository).

### Windows
[MyLibrary](https://github.com/ProfessorNavigator/mylibrary) libraries must be in one of the system paths (indicated in `Path` system variable). Another option is to install MyLibrary by MSYS2.

## Usage
The MLFBDPlugin interface is intuitive. But if you have any questions, see [fb2](https://github.com/gribuser/fb2) `description` tag documentation.

## License

GPLv3 (see `COPYING`).

## Donation

If you want to help to develop this project, you can assist it by [donation](https://yoomoney.ru/to/4100117795409573)

## Contacts

You can contact author by email \
bobilev_yury@mail.ru