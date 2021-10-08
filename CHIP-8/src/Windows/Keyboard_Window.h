//#include "Graphics.h"
#include "../CPU.h" // Should give error if included alongside "Graphics.h", as graphics includes itself
#include "Stack_Window.h"

#pragma once
namespace Keyboard_Window {
	float keyboardH_factor = 0.4;
	float keyboardW_factor = 0.2;

	ImVec2 btnSize;

	int keyboard_spacing(int row, int vertical_spacing, int initialY_padding) {
		return row * (btnSize.y + vertical_spacing) + initialY_padding;
	}

	void MakeNCheckBtn(const char* hex_letter, const char* alternative, uint8_t hex_code) {
		char final_str[6];
		final_str[0] = *hex_letter;
		final_str[1] = ' ';
		final_str[2] = '(';
		final_str[3] = *alternative;
		final_str[4] = ')';
		final_str[5] = '\0';

		if ((CPU::keys >> hex_code) & 1) {
			//ImGui::ImGui // color this button or make a colored button. find default active / hovered color?
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			ImGui::Button(final_str, ImVec2(btnSize.x, btnSize.y));
			ImGui::PopStyleColor();
		}
		else {
			ImGui::Button(final_str, ImVec2(btnSize.x, btnSize.y));
		}

		//ImGui::Button(final_str, ImVec2(btnSize, btnSize));
		if (ImGui::IsItemActivated()) {
			CPU::HandlePressDown(hex_code);
		}

		if (ImGui::IsItemDeactivated()) {
			CPU::HandlePressUp(hex_code);
		}
	}

	void MakeNCheckBtn(const char* hex_letter, uint8_t hex_code) {
		if ((CPU::keys >> hex_code) & 1) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			ImGui::Button(hex_letter, ImVec2(btnSize.x, btnSize.y));
			//ImGui::GetStyle().Colors
			ImGui::PopStyleColor();
		}
		else {
			ImGui::Button(hex_letter, ImVec2(btnSize.x, btnSize.y));
		}

		if (ImGui::IsItemActivated()) {
			CPU::HandlePressDown(hex_code);
		}

		if (ImGui::IsItemDeactivated()) {
			CPU::HandlePressUp(hex_code);
		}
	}

	void draw_keyboard() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * Stack_Window::stackW_factor, Graphics::height * Debug_Window::debugH_factor), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * keyboardW_factor, Graphics::height * keyboardH_factor));
		ImGui::Begin("Keyboard", NULL, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::SetCursorPos(ImVec2(0, 0));
		ImVec2 tile_dimensions = ImGui::GetContentRegionAvail();

		int initialX_padding = 0.1 * tile_dimensions.x;
		int initialY_padding = 0.1 * tile_dimensions.y;
		int horizontal_padding = 0.1 * tile_dimensions.x;

		//initialY_padding += (8.5 / 253) * tile_dimensions.y;
		//initialX_padding -= (2.5 / 240) * tile_dimensions.x;
		int btnW = (tile_dimensions.x - (2 * initialX_padding)) / 4.f; // Width of each button
		int btnH = ((tile_dimensions.y - (2 * initialY_padding)) / 4.f);

		btnH *= 0.90;

		int vertical_padding = 0.045 * tile_dimensions.y;

		btnSize = ImVec2(btnW, btnH);

		ImGui::SetCursorPos(ImVec2(initialX_padding * 0.8, initialY_padding * 1.15)); //keyboard_spacing(0, vertical_padding, initialY_padding)));
		MakeNCheckBtn("1", 0x1);
		ImGui::SameLine();
		MakeNCheckBtn("2", 0x2);
		ImGui::SameLine();
		MakeNCheckBtn("3", 0x3);
		ImGui::SameLine();
		MakeNCheckBtn("C", "4", 0xC);

		ImGui::SetCursorPos(ImVec2(initialX_padding * 0.8, keyboard_spacing(1, vertical_padding, initialY_padding * 1.15)));
		MakeNCheckBtn("4", "Q", 0x4);
		ImGui::SameLine();
		MakeNCheckBtn("5", "W", 0x5);
		ImGui::SameLine();
		MakeNCheckBtn("6", "E", 0x6);
		ImGui::SameLine();
		MakeNCheckBtn("D", "R", 0xD);

		ImGui::SetCursorPos(ImVec2(initialX_padding * 0.8, keyboard_spacing(2, vertical_padding, initialY_padding * 1.15)));
		MakeNCheckBtn("7", "A", 0x7);
		ImGui::SameLine();
		MakeNCheckBtn("8", "S", 0x8);
		ImGui::SameLine();
		MakeNCheckBtn("9", "D", 0x9);
		ImGui::SameLine();
		MakeNCheckBtn("E", "F", 0xE);

		ImGui::SetCursorPos(ImVec2(initialX_padding * 0.8, keyboard_spacing(3, vertical_padding, initialY_padding * 1.15)));
		MakeNCheckBtn("A", "Z", 0xA);
		ImGui::SameLine();
		MakeNCheckBtn("0", "X", 0x0);
		ImGui::SameLine();
		MakeNCheckBtn("B", "C", 0xB);
		ImGui::SameLine();
		MakeNCheckBtn("F", "V", 0xF);

		ImGui::End();
	}



};