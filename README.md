# Description
<p align="center"><img src="https://i.imgur.com/NOgPd32.gif"/></p>
<p align="center">(Some frames were deleted to make the GIF smaller)</p>
<br>
<br>
CHIP-8 emulator/debugger made with C++ 17.
<br>
SDL 2.0.16 for window creation, event handling and audio.
<br>
ImGui & OpenGL 3 for user interface and rendering.
<br>
nativefiledialog for opening file dialogs.
<br>

Configuration files generated using Premake 5 Alpha 16.

Default settings:
<br>
Instructions executed per second: 700
<br>
Delay timer decrements per second: 60
<br>
Sound timer decrements per second: 60
<br>
Beep frequency: 440 Hz
<br>
FPS Limit: 60
<br>
VSync: On
<br>
BG Color: Black
<br>
FG Color: White

You can find pre-built binaries for some operational systems in the releases section of this repository.

So far tested and functional on Windows 10 host, Ubuntu 20.04 64-bit & Ubuntu 16.04 32-bit virtual machines and MacOS Big Sur 64-bit virtual machine.

This is my first emulator, as well as my first time using OpenGL, ImGui, premake, and attempting cross-platform. Please excuse possible flaws and/or poor performance...
<br>
<br>
Also note that I didn't see any CHIP-8 source code when making this - I only read guides about how the CHIP-8 worked, without actual implementation.

# Features
* Functional CHIP-8 emulation (As far as I'm concerned);
* Tracking and decimal & hexadecimal representation of all the registers, the program counter, sound & delay timers and stack;

* Pausing, reloading and resetting;

* Ability to modify on-the-fly:
    * Instructions/s - How many instructions are executed every second; 
    * Delay dec/s - How much does the delay timer gets decremented by every second;
    * Sound dec/s - How much does the delay timer gets decremented by every second;
    * Beep Freq - The frequency at which the beep sound gets played;
    * FPS Limit - What is the maximum amount of frames executed every second through the whole program; 
    * VSync - When activated caps your FPS at your display's refresh rate (independent of FPS limit);
    * BG Color - Background color;
    * FG Color - Foreground color;
    * Reset Settings - Resets all above settings to default;
* Disassembler - Follow PC toggles whether or not to stick to the current instruction.
  <br>
  * All addresses loaded by the ROM (which starts at 0x200) each represent the instruction stored at that address.
  * Note that each instruction allocates 2 bytes (16 bits, or 2 memory addresses).
  * The reason why I didn't make it so that only every 2 addresses (i.e. 0x200, 0x202, 0x204...) were shown is that some ROMs access odd addresses (i.e. Space Invaders) and there would be no proper way to organize the disassembler otherwise.
  * An alternative is to display only every 2 even addresses by default (given the overwhelming majority of ROMs access even addresses exclusively) along with the current instruction, regardless of whether it's odd or not. Then figure out whether this instruction accesses odd addresses, and if it does, display also the address accessed by this current instruction - This way you could always follow the current address and see which address is being accessed.
  The problem is that if the ROM accesses only odd addresses, you won't be able to inspect each of these addresses without getting to the instruction that accesses each of them first. Not to mention that all of the addresses displayed are pretty much useless, given they're almost all even. This would be too much of a hassle, time consuming, and inefficient for the user.
  * The downsides to my approach are that since all addresses are displayed, the disassembler has much more content and the addresses are possibly harder to follow visually, if the ROM has either even or odd only addresses.

* Memory reader;

* Stepping and skipping through instructions;

* Tracking of which keys are being pressed/held, and ability to trigger them by clicking on their respective buttons.

# Building
<b>Warning: Tested only with vs2019 build of Premake on Windows host and gmake2 for both Linux & MacOS Virtual Machines.
<br>
When compiling for GNU Make, GCC version must be >= 8.1, as the "charconv" header is not available in older versions.
</b>

This project uses C++ 17 and Premake 5 Alpha 16.

Run the premake executable (inside the "bin" folder) compatible with your operational system and pass in which type of project to generate build files for as an argument.

premake5.exe for Windows, premake5-64bit for 64-bit Linux, premake5-32bit for 32-bit Linux, premake5-macos for MacOS.

| Option | Description |
| ------ | ----------- |
| vs2019 | Generate Visual Studio 2019 project files |
| vs2017 | Generate Visual Studio 2017 project files |
| vs2015 | Generate Visual Studio 2015 project files |
| vs2013 | Generate Visual Studio 2013 project files |
| vs2012 | Generate Visual Studio 2012 project files |
| vs2010 | Generate Visual Studio 2010 project files |
| vs2008 | Generate Visual Studio 2008 project files |
| vs2005 | Generate Visual Studio 2005 project files |
| gmake | Generate GNU Makefiles (deprecated in favor of gmake2) |
| gmake2 | Generate GNU Makefiles (including Cygwin and MinGW) |
| xcode4 | Generate XCode project files |
| codelite | Generate CodeLite project files |

## Windows
<b>Warning: Tested on Windows 10 64-bit host (BUILD 19043.1237) with Visual Studio 2019. </b>

Just running premake.exe and passing in your preferred configuration as an argument should suffice.

## Linux
<b>Warning: Tested on Ubuntu 20.04 (64-bit) & Ubuntu 16.04 (32-bit) Virtual Machines, with GNU Make. </b>

You will need to have installed 2 libraries prior to building: libgtk-3-dev and libsdl2-dev.
<br>
Then, if you're on a 64-bit operating system, you have to build using "premake5-64bit", which can be found inside the "bin" folder. Likewise, if you're on a 32-bit operating system, build using "premake5-32bit".

Note: The "premake5-32bit" executable was built from source on a Ubuntu 16.04 32-bit Virtual Machine. The reason for this is that the executable in the official premake website did not run in my 32-bit Virtual Machine, only in the 64-bit one.

## MacOS
<b>Warning: Tested on MacOS Big Sur (11.1) 64-bit Virtual Machine.</b>

You will need to have installed the SDL2 library in order to build. You can do so through the following command if you have brew installed: "brew install sdl2".

Afterwards, just run the "premake5-macos" executable in the "bin" folder, passing in your preferred configuration as an argument.

# Notes
* You can hide/show the debug information by pressing 'H' or clicking the "Toggle Debug (H)" button in the "Debug Info" window;

* Timers do not change when skipping instructions, only when stepping through them or playing/resuming the emulation. This is on purpose;

* Sprintf seemed to be ~4 and std::string ~8 times slower than manually formatting instructions as character pointers during my tests, which is why I chose this route, even though it's pretty ugly;

* GCC version needs to be 8.1 or greater for compilation with GNU Make, as older versions don't support the header "charconv" which is used here for converting other types (such as uint8_t) into characters;

# Known Issues
* Smaller window dimensions clip certain elements of the interface;

* Mouse position not captured properly when I tested on my Ubuntu 16.04 32-bit Virtual Machine;

* Sometimes, the ROM automatically starts executing after loading instead of waiting for the "Start ROM" button to be clicked;

* SDL_GetError() returns "wglMakeCurrent(): The pixel format is invalid.". Need to find out the cause of this issue.

# To be done
* Add implementation notes to README;

* Properly de-initialize variables when closing;
  
* Refactor, get rid of compiler warnings, unitialized variables, etc...;
  
* Separate each debug component into its own container;
  
* Make headers self-sufficient;
  
* Standardize variable and function names;
  
* Document code;

* Improve the messaging, logging and error system;

* Fix known issues;

* Introduce keyboard shortcuts for stepping and skipping through instructions;

* Implement setting that handles emulation speed (presets slow, original, smooth, fast) along with a slider independent of framerate representing a speed multiplier that affects instructions per second, delay decrements per second, sound decrements per second, etc...;

* Implement an option to toggle tiles which draws a non-filled square around every tile in the emulator, then when some tile is hovered pop-up a tooltip displaying its corresponding X and Y coordinates;

* Update emulation screen (pixels/texture) only when drawing instructions are triggered;

* Make a more general function for writing and formatting instructions' assembly descriptions;

* Implement more complete descriptions to each instruction, which will pop-up when they are hovered over;

* Introduce tooltips which display extra information about some item when it is hovered;

* Implement dialog which contains all instruction patterns, their complete descriptions and assembly description;

* Let instructions that vary if implementation-dependant (i.e. COSMAC VIP, ETI 660, etc...) be configurable in some dialog. Along with a preset system dropdown in the debug menu;

* Ability to modify keybindings;

* Add options to change the background color of window tiles (perhaps individually?), buttons, and primary text color;

* Adjust font size accordingly when window resizes;

* Allow resizing of ImGUI tiles;

* Find, if possible and beneficial, a more concise way of represeting/decoding the instructions in the code other than a long switch statement with nested switch statements;

* Decrease memory usage if possible. After introducing OpenGL and ImGUI (from just using SDL rendering) memory usage went from ~5MB to ~50MB on Windows;

* Profile sections of the code and optimize the worst ones;

* Implement breakpoints;

* Allow users to change the text font;

* Add option for saving/loading settings, this should include everything from emulator-specific variables such as instructions per second and including user interface ones such as colors;

* Add clean action to premake;

* Improve tracking of FPS in order to optimize distinctively for display and use in the main loop in order to update emulation variables?

* Possibly use threads to separate the emulation from the rest of the program and thus isolate their respective framerates and other variables;

* Add option to step/skip back instead of forward only;

* Introduce tests;
  
* Add slider for different options of bytes per row in the memory view (default is 16);
  
* Option to search in memory view and disassembler for specific instructions and addresses;

* Switch to Vulkan? Definitely not a necessity performance wise, but would be an interesting learning process...
