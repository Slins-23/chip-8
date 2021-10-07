#include "../CPU.h"
#include "Stack_Window.h"

#pragma once
namespace Memory_Window {
	float memoryH_factor = 0.4;
	float memoryW_factor = 0.4;

	void draw_memory() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * (Stack_Window::stackW_factor + Keyboard_Window::keyboardW_factor), Graphics::height * Display_Window::displayH_factor), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Memory_Window::memoryW_factor, Graphics::height * Memory_Window::memoryH_factor));
		ImGui::Begin("Memory", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		for (int i = 0; i < RAM::byte_rows; i++) {
			char addr[5];
			CPU::NdecToHex(addr, i, RAM::bytes_per_row);

			uint8_t* bytes = (uint8_t*)malloc(RAM::bytes_per_row);
			memcpy(bytes, RAM::rom_data + (16 * i), RAM::bytes_per_row);

			char bytesrep[48];

			for (int j = 0; j < RAM::bytes_per_row; j++) {
				char curbyte[5];
				CPU::decToHex(curbyte, bytes[j]);

				// 0x30 == '0'
				if (curbyte[1] == 0) {
					curbyte[1] = curbyte[0];
					curbyte[0] = '0';
					curbyte[2] = '\0';
				}


				memcpy(bytesrep + (j * 3), curbyte, 2);

				if (j == RAM::bytes_per_row - 1) {
					bytesrep[(3 * j) + 2] = '\0';
				}
				else {
					bytesrep[(3 * j) + 2] = ' ';
				}


			}

			char charrep[17];

			for (int j = 0; j < RAM::bytes_per_row; j++) {
				charrep[j] = bytes[j];
				if (charrep[j] < 0x20) charrep[j] = 0x2E; // 0x2E == '.'

				if (j == RAM::bytes_per_row - 1) {
					charrep[j + 1] = '\0';
				}
			}

			free(bytes);

			if (i >= 16) {
				ImGui::Text("0x%s0 | %s | %s", addr, bytesrep, charrep);
			}
			else {
				ImGui::Text("0x%s0  | %s | %s", addr, bytesrep, charrep);
			}
		}

		ImGui::Columns();
		ImGui::End();
	}
};