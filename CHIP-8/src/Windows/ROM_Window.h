#include "../Graphics.h"
#include "../RAM.h"

#pragma once
namespace ROM_Window {
	float romH_factor = 0.2;
	float romW_factor = 0.25;

	void draw_rom() {
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * romW_factor, Graphics::height * romH_factor));
		//| ImGuiWindowFlags_HorizontalScrollbar
		ImGui::Begin("ROM", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::TextWrapped("ROM Title: %s", RAM::rom_title);
		ImGui::TextWrapped("ROM Path: %s", RAM::rom_path);
		ImGui::TextWrapped("Rom Size (Bytes): %d", RAM::rom_size);
		ImGui::End();
	}
};
