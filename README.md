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
  * **MSVC**:  

    From the terminal run,
    ```sh
    winget install Microsoft.VisualStudio.2022.BuildTools --force
    --override "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools;includeRecommended"
    ```
--------------------------------------------------------------------------------
**NOTE**

* for all the subsequent commands use the **Developer Command Prompt for VS 2022**.
* `CMake` comes bundled with `MSVC` installation, so no need to install separately.

--------------------------------------------------------------------------------

* [NAppGui](https://nappgui.com/en/guide/build.html)

    GUI framework for the application.

    1. Download source
        ```sh
        git clone --depth 1 https://github.com/frang75/nappgui_src.git
        cd nappgui_src
        ```

    2. Build debug version
        ```sh
        cmake -S . -B build/static-debug -DNAPPGUI_DEMO=NO
        cmake --build build/static-debug --config Debug -j 4
        cmake --install build/static-debug --config Debug --prefix C:/nappgui/static-debug
        ```

    3. Build the release version
        ```sh
        cmake -S . -B build/static-release -DNAPPGUI_DEMO=NO
        cmake --build build/static-release --config Release -j 4
        cmake --install build/static-release --config Release --prefix C:/nappgui/static-release
        ```

    This configures, builds (statically linking versions of the framework) and
    installs them to the specified location (`c:/nappgui` in the above 
    instructions.) with both `Debug` and `Release` flavors.

* [`vcpkg`](https://vcpkg.io/)

    Install `vcpkg`, if not already installed,
    ```sh
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg && bootstrap-vcpkg.bat
    ```

* [Kaatib](https://github.com/roximn148/kaatib)
    
    Get the `kaatib` source code.
    ```sh
    git clone https://github.com/roximn148/kaatib.git
    cd kaatib
    ```

* Build

    1. Define presets,
   
        For building, create cmake presets file `CMakeUserPresets.json` in the 
        project root directory, for `debug` and `release` builds as well as 
        providing the paths to `nappgui` and `vcpkg` as environment variables.
        ```json
        {
            "version": 4,
            "configurePresets": [
                {
                    "name": "debug",
                    "inherits": "x64-debug",
                    "environment": {
                        "NAPPGUI_ROOT": "c:\\nappgui",
                        "VCPKG_ROOT": "<path/to/vcpkg>"
                    }
                },
                {
                    "name": "release",
                    "inherits": "x64-release",
                    "environment": {
                        "NAPPGUI_ROOT": "C:\\nappgui",
                        "VCPKG_ROOT": "<path/to/vcpkg>"
                    }
                }
            ],
            "buildPresets": [
                {
                    "name": "debug",
                    "displayName": "Windows Debug build",
                    "configurePreset": "debug",
                    "description": "Vanilla debug build for windows"
                },
                {
                    "name": "release",
                    "displayName": "Windows Release build",
                    "configurePreset": "release",
                    "description": "Vanilla release build for windows"
                }
            ]
        }
        ```

    2. select a configure preset, (`debug` or `release`),

        ```sh
        cmake --preset=debug
        ```
    
    3. build the project.

        ```sh
        cmake --build --preset=debug
        ```

### Linux (_incomplete & untested_)
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

    This configures and builds the `Release` version of the framework at the 
    specified location (`/usr/local/nappgui` in the above instructions.)

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


### Icons
The icons used are [Material Icons](https://fonts.google.com/icons) by Google.
The [Iconer](https://iconer.app/material/) website was used for rendering icons
to png. (Color `#0b6105`, Size `20`, Indent `0`).
