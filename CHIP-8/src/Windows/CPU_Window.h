#include "../CPU.h"
#include "ROM_Window.h"

#pragma once
namespace CPU_Window {
	float cpuH_factor = 0.4;
	float cpuW_factor = 0.25;

	void new_register(uint8_t reg) {
		char arr[5];
		char hexReg[5];
		uint8_t decValue = *(CPU::register_pointer + reg);
		CPU::decToHex(arr, decValue);
		CPU::decToHex(hexReg, reg);

		ImGui::Text("V%s", hexReg);
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Text("%d", decValue);
		ImGui::Text("0x%s", arr);
	}

	void draw_cpu() {
		char pc[5], I[5], DT[5], ST[5];
		char V0[5], V1[5], V2[5], V3[5], V4[5], V5[5], V6[5], V7[5];
		char V8[5], V9[5], VA[5], VB[5], VC[5], VD[5], VE[5], VF[5];

		CPU::decToHex(pc, CPU::pc - RAM::buffer);
		CPU::decToHex(I, *((uint16_t*)(CPU::register_pointer + 16)));
		CPU::decToHex(DT, CPU::delay);
		CPU::decToHex(ST, CPU::sound);

		ImGui::SetNextWindowPos(ImVec2(0, Graphics::height * ROM_Window::romH_factor), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * cpuW_factor, Graphics::height * cpuH_factor));
		ImGui::Begin("CPU", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Separator();
		ImGui::Columns(4, 0);
		ImGui::Text("PC ");
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Text("0x%s", pc);
		ImGui::Text("  %d", CPU::pc - RAM::buffer);
		ImGui::NextColumn();
		ImGui::Text("IR ");
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Text("0x%s", I);
		ImGui::Text("  %d", *((uint16_t*)(CPU::register_pointer + 16)));
		ImGui::NextColumn();
		ImGui::Text("Delay ");
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Text("0x%s", DT);
		ImGui::Text("  %d", CPU::delay);
		ImGui::NextColumn();
		ImGui::Text("Sound ");
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Text("0x%s", ST);
		ImGui::Text("  %d", CPU::sound);
		ImGui::Columns();
		ImGui::Separator();
		ImGui::Columns(8, 0);

		for (int i = 0; i < 8; i++) {
			new_register(i);

			if (i < 7) ImGui::NextColumn();
		}

		ImGui::Columns();
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Separator();

		ImGui::Columns(8, 0);

		for (int i = 8; i < 16; i++) {
			new_register(i);

			if (i < 15) ImGui::NextColumn();
		}

		ImGui::Columns();
		ImGui::Separator();
		ImGui::Text("%.1f FPS (%.3f ms/frame) ", CPU::current_framerate, 1000.f / CPU::current_framerate);
		ImGui::End();
	}
};

