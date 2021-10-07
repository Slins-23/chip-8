#include "Windows/ROM_Window.h"
#include "Windows/CPU_Window.h"
#include "Windows/Stack_Window.h"
#include "Windows/Debug_Window.h"
#include "Windows/Keyboard_Window.h"
#include "Windows/Display_Window.h"
#include "Windows/Memory_Window.h"
#include "Windows/Disassembler_Window.h"

#pragma once
namespace Window_Manager {
	void draw_windows() {
		ROM_Window::draw_rom();
		CPU_Window::draw_cpu();
		Stack_Window::draw_stack();
		Debug_Window::draw_debug();
		Keyboard_Window::draw_keyboard();
		Display_Window::draw_display();
		Memory_Window::draw_memory();
		Disassembler_Window::draw_disassembler();
	}
};