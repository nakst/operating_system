# Contributing Guidelines

## Map

- `api/` Contains files for the API library that provides a layer between programs and the kernel and the GUI.
  - `api.cpp` API initialisation and internal messaging.
  - `api.s` API functions that must be implemented in assembly.
  - `common.cpp` Standard functions, such as `OSCopyMemory` and `OSFormatString`. Also shared by the kernel.
  - `crti.s, ctrn.s` Global constructors and destructors setup.
  - `cstdlib.c` Provides a fake linux system call interface for musl (the C standard library we use).
  - `empty.manifest` An empty manifest file used by the glue library.
  - `font.cpp` Layer between the API and freetype for text rendering, including glyph caching and string measurements.
  - `glue.cpp` The source file that gets the standard definitions from `standard.manifest` that can be linked into programs that don't use the manifest system.
  - `gui.cpp` The GUI.
  - `heap.cpp` Heap functions. Also shared by the kernel.
  - `linked_list.cpp` Generic linked lists, for internal use. Also shared by the kernel.
  - `list_view.cpp` A list view control for the GUI.
  - `os.h` The header file containing all the definitions for the external facing portion of the API.
  - `standard.manifest` Contains the standard commands used by the GUI, such as `osCommandCopy`.
  - `stb_image.h` Loads images. From https://github.com/nothings/stb (public domain).
  - `stb_sprintf.h` Formatted output compatible with `printf`. From https://github.com/nothings/stb (public domain).
  - `syscall.cpp` System call stubs that call into `OSSyscall` from in `api.s`.
  - `test.cpp, test.manifest` A test program.
  - `textbox.cpp` A textbox control for the GUI.
  - `utf8.h` UTF-8 functions.
- `boot/` Contains files for booting the operating system.
  - `x86/` ...on x86.
    - `esfs-stage1.s` Loads `loader.s` from the start of a EsFS volume and passes control to it.
    - `esfs-stage2.s` Provides basic read-only EsFS functions for `loader.s`.
    - `loader.s` Loads the kernel and passes control to it.
    - `mbr.s` Finds and loads a bootable partition.
- `calculator/` A calculator program.
  - `calculator.manifest` The program's manifest.
  - `main.cpp` The program's implementation.
- `desktop/` The desktop environment. Acts as a package manager and fallback debugger.
  - `desktop.manifest` The program's manifest.
  - `main.cpp` The program's implementation.
- `file_manager/` A file manager program.
  - `file_manager.manifest` The program's manifest.
  - `main.cpp` The program's implementation.
- `image_viewer/` A image viewer program.
  - `image_viewer.manifest` The program's manifest.
  - `main.cpp` The program's implementation.
- `kernel/` The kernel and its drivers.
  - `acpi.cpp` A layer between the kernel and ACPICA. Also starts application processors on SMP systems.
  - `ahci.cpp` A (currently broken) AHCI/SATA driver.
  - `ata.cpp` A ATA/IDE driver.
  - `bitset.cpp` Functions for managing large bitsets.
  - `devices.cpp` The device and IO manager.
  - `elf.cpp` Parses ELF executables.
  - `esfs.cpp` The EssenceFS file system driver.
  - `graphics.cpp` The software renderer.
  - `kernel.h` Kernel definitions. Includes all other source files in the kernel.
  - `main.cpp` Kernel initilisation.
  - `memory.cpp` Physical, virtual and shared memory management.
  - `object_manager.cpp` Manages object and handles shared between the kernel and programs.
  - `pci.cpp` Finds devices on the PCI bus.
  - `ps2.cpp` A driver for PS/2 keyboard and mice.
  - `scheduler.cpp` A scheduler, and manager of threads and processes.
  - `syscall.cpp` Defers system calls to other parts of the kernel.
  - `terminal.cpp` Kernel debugging and serial output. 
  - `vfs.cpp` The virtual filesystem.
  - `window_manager.cpp` The window manager. Passes messages from the PS/2 driver to programs.
  - `x86_64.cpp` Code for the x64 architecture.
  - `x86_64.s` Assembly code for the x64 architecture.
- `ports/` A mess of ported programs. Enter with caution.
- `res/` Resources, such as fonts and visual styles.
  - `Fonts/` Fonts used by the GUI.
  - `Icons` Icon packs used by the GUI.
  - `Sample Images` Wallpapers loadable in `desktop`.
  - `Visual Styles` GUI visual styles.
- `system_monitor/` A system monitor program.
  - `system_monitor.manifest` The program's manifest.
  - `main.cpp` The program's implementation.
- `util/` Utilities for building the operating system.
  - `build.cpp` The build system.
  - `esfs.cpp` A version of EssenceFS for use on linux from the command line.
  - `linker.ld, linker64.ld, linker_userland.ld, linker_userland64.ld` Linker scripts.
  - `manifest_parser.cpp` A manifest parser. Used by the build system and `desktop`, and soon by the API.

## Code Style

Functions and structures names use `PascalCase`.
Variables use `camelCase`, while constants and macros use `SCREAMING_SNAKE_CASE`.

Tabs are `\t`, and are 8 characters in size.

Braces are placed at the end of the line: 

    if (a > b) {
        ...
    }
    
Blocks are always surrounded by newlines, and always have braces.

    int x = 5;
    
    if (x < 6) {
        x++; // Postfix operators are preferred.
    }
    
Exception: If there are lot of short, linked blocks, then they may be written like this-

    if (width == DIMENSION_PUSH) { bool a = grid->widths[i] == DIMENSION_PUSH; grid->widths[i] = DIMENSION_PUSH; if (!a) pushH++; }
    else if (grid->widths[i] < width && grid->widths[i] != DIMENSION_PUSH) grid->widths[i] = width;
    if (height == DIMENSION_PUSH) { bool a = grid->heights[j] == DIMENSION_PUSH; grid->heights[j] = DIMENSION_PUSH; if (!a) pushV++; }
    else if (grid->heights[j] < height && grid->heights[j] != DIMENSION_PUSH) grid->heights[j] = height;

Function names are always descriptive, and use prepositions and conjuctions if neccesary. 

    OSCopyToScreen // Symbols provided by the API are prefixed with OS-, or os-.
    OSDrawSurfaceClipped
    OSZeroMemory
    
Variable names are usually descriptive, but sometimes shortened names are used for short-lived variables.

    OSMessage m = {};
    m.type = OS_MESSAGE_MEASURE;
    OSCallbackResponse r = OSSendMessage(grid->objects[0], &m);
		if (r != OS_CALLBACK_HANDLED) OSCrashProcess(OS_FATAL_ERROR_MESSAGE_SHOULD_BE_HANDLED);

Operators are padded with spaces on either side.

    bounds.left = (grip->bounds.left + grip->bounds.right) / 2 - 4;
    
A space should be placed between a cast and its expression.

    int x = (float) y;

Although the operating system is written in C++, most C++ features are avoided.
However, the kernel uses a lot of member functions.

    struct Window {
        void Update(bool fromUser);
        void SetCursorStyle(OSCursorStyle style);
        void NeedWMTimer(int hz);
        void Destroy();
        bool Move(OSRectangle &newBounds);
        void ClearImage();

        Mutex mutex; // Mutex for drawing to the window. Also needed when moving the window.
        Surface *surface;
        OSPoint position;
        size_t width, height;
        ...
    }
    
Default arguments often provided as functions grow over time.

There is no limit on function size. However, you should avoid regularly exceeding 120 columns.

    static OSCallbackResponse ProcessControlMessage(OSObject _object, OSMessage *message) {
        // 300 lines later...
    }

Pointers are declared like this: `Type *name;`.
