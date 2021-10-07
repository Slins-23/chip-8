#include "../CPU.h" // Also includes Graphics and RAM, which are dependencies
#include "CPU_Window.h"

#pragma once
namespace Stack_Window {
	float stackH_factor = 0.4;
	float stackW_factor = 0.2;

	void draw_stack() {
		ImGui::SetNextWindowPos(ImVec2(0, Graphics::height * (ROM_Window::romH_factor + CPU_Window::cpuH_factor)), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * stackW_factor, Graphics::height * stackH_factor));
		ImGui::Begin("Stack", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		char sp[5];
		CPU::decToHex(sp, RAM::sp - RAM::sp_addr);

		ImGui::Text("SP: %d (0x%s)", RAM::sp - RAM::sp_addr, sp);
		ImGui::Columns(3);

		for (int i = 0; i < RAM::stack_size; i++) {
			ImGui::Text("Depth: %d", i);
		}

		ImGui::NextColumn();

		for (int i = 0; i < RAM::stack_size; i++) {
			ImGui::Text("%d", *(RAM::sp_addr + i));
		}


		ImGui::NextColumn();

		for (int i = 0; i < RAM::stack_size; i++) {
			char val[5];
			CPU::decToHex(val, *(RAM::sp_addr + i));

			ImGui::Text("0x%s", val);
		}

		ImGui::End();
	}
};