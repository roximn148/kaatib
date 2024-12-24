# Kaatib - Urdu Unicode Text Editor

Kaatib(کاتب) is a unicode text editor specifically designed for use with
right to left cursive language text editing, particularly for Urdu.

## Design
Kaatib is a C language application using
[NAppGui Framework](https://nappgui.com/)
for cross-platform gui application. It uses
[FreeType](https://freetype.org/),
[HarfBuzz](https://github.com/harfbuzz/harfbuzz) and
[libraqm](https://github.com/HOST-Oman/libraqm)
for the unicode script rendering.

## Features
* Fast, cross-platform, light weight
* Dedicated RTL text editor
* Roman transliteration
* Built-in Onscreen keyboard, supporting mutiple languages
* NLP
* Spell checker

## Setup
### Windows
* Build Tools
  * MSVC
    
    From the terinal run,
    ```sh
    winget install Microsoft.VisualStudio.2022.BuildTools --force --override "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools;includeRecommended"
    ```

    and for all the subsequent commands use the **Developer Comaand Prompt for VS 2022**.

    `CMake` comes bundled with `MSVC` installation, so no need to install separately.

* [NAppGui](https://nappgui.com/en/guide/build.html)

    GUI framework for the application.

    ```sh
    git clone --depth 1 https://github.com/frang75/nappgui_src.git
    cd nappgui_src

    cmake -S . -B build -DNAPPGUI_DEMO=NO
    cmake --build build --config Release -j 4
    cmake --install build --config Release --prefix C:/nappgui
    ```

    This configures, builds the `Release` version of the framework and installs it to the specified location (`c:/nappgui` in the above instructions.)

* [Kaatib](https://github.com/roximn148/kaatib)
    
    Get the `Kaatib` source code.
    ```sh
    git clone https://github.com/roximn148/kaatib.git
    cd kaatib
    ```

* [Unity](https://github.com/ThrowTheSwitch/Unity)

    For unit testing.
    
    In the `Kaatib` source directory download the `Unity` source code.

    ```sh
    git clone https://github.com/ThrowTheSwitch/Unity.git
    ```

* Build

  For building, provide the installation location of the `NAppGui`.
  ```sh
  cmake -S . -B build -DCMAKE_INSTALL_PREFIX=c:/nappgui
  cmake --build build
  ```

  For Visual Studio Code, add following to the `settings.json`.
  ```json
  {
    "cmake.configureArgs": [
        "-DCMAKE_INSTALL_PREFIX=c:/nappgui"
    ]
  }
  ```
  
### Linux
* Build Tools

* [NAppGui](https://nappgui.com/en/guide/build.html)

    GUI framework for the application.

    ```sh
    git clone --depth 1 https://github.com/frang75/nappgui_src.git
    cd nappgui_src

    cmake -S . -B build -DNAPPGUI_DEMO=NO -DCMAKE_BUILD_TYPE=Release
    cmake --build build -j 4
    cmake --install build --config Release --prefix /usr/local/nappgui
    ```

    This configures and builds the `Release` version of the framework at the specified location (`/usr/local/nappgui` in the above instructions.)

* [Kaatib](https://github.com/roximn148/kaatib)
    
    Get the `Kaatib` source code.
    ```sh
    git clone https://github.com/roximn148/kaatib.git
    cd kaatib
    ```

* [Unity](https://github.com/ThrowTheSwitch/Unity)

    For unit testing.
    
    In the `Kaatib` source directory download the `Unity` source code.

    ```sh
    git clone https://github.com/ThrowTheSwitch/Unity.git
    ```
