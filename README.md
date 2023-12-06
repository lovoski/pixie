Pixie
=====

Pixie is a minimal, cross-platform pixel framebuffer library for Windows and macOS.

![example.gif](/example.gif)

### Quick Start

To use the library, simply write:

```cmake
add_subdirectory(YOUR_PATH_TO_PIXIE_FOLDER)

# If there's a header file problem
# include_directories(YOUR_PATH_TO_PIXIE_FOLDER)

target_link_libraries(YOUR_PROJECT_NAME PRIVATE pixie)
```

To use Pixie:

```cpp
#include "pixie.h"

int main(int argc, char** argv)
{
    Pixie::Window window;
    if (!window.open("Hello, World!", 640, 480))
        return 0;

    while (!window.HasKeyGoneUp(Pixie::Key_Escape))
    {
        uint32_t* pixels = window.GetPixels();
        // ..draw pixels!

        if (!window.Update())
            break;
    }

    window.Close();
}
```

The pixel byte order is BGRx (from LSB to MSB). On macOS, the pixel buffer is transformed
from BGR to RGB.

### Examples

Pixie has an example program in `main.cpp`. It can be compiled with cmake.

The building process will generate an executable `pixie_demo`.

To disable the demo executable, set the variable `BUILD_PIXIE_DEMO` to OFF in cmake cache.

On macOS Pixie requires the `CoreGraphics` and `AppKit` frameworks.

### API

Pixie has some basic keyboard and mouse handling. You can check for:

* Mouse or key down in the current frame: `HasMouseGoneDown`, `HasKeyGoneDown`, `HasAnyKeyGoneDown`
* Mouse or key up in the current frame: `HasMouseGoneUp`, `HasKeyGoneUp`
* Mouse or key currently down: `IsMouseDown`, `IsKeyDown`, `IsAnyKeyDown`

The mouse position (in window coordinates) can be obtained with `GetMouseX` and `GetMouseY`.

Additionally the current time delta in seconds can be obtained with `GetDelta`.

### ImGui

Pixie has a basic ImGui with support for:

* Labels
* Buttons
* Input fields
* Check boxes
* Radio boxes
* Drawing rectangles and filled rectangles

The ImGui is integrated into cmake library `pixie` by default.

To load the font:

```cpp
Pixie::Font font;
const int FontWidth = 9;
const int FontHeight = 16;
if (!font.Load("font.bmp", FontWidth, FontHeight))
    return 0;
```
Another approach is to load the hard coded default font with:

```cpp
Pixie::Font font;
font.LoadDefaultFont();
```

The hrad coded default font file : `fontbmp.h`

In your main loop:

```cpp
while (!window.HasKeyGoneUp(Pixie::Key_Escape))
{
    Pixie::ImGui::Begin(&window, &font);

    if (Pixie::ImGui::Button("Hello", 100, 100, 100, 30))
        printf("Hello button was pressed\n");

    Pixie::ImGui::End();

    if (!window.Update())
        break;
}
```

### License

Pixie is licensed under the MIT License. See LICENSE for more information.
