#include "../CPU.h"
#include "Display_Window.h"

#pragma once
namespace Disassembler_Window {
	float disassemblerH_factor = 1.0;
	float disassemblerW_factor = 0.2;

	bool follow_pc = true;

	void draw_disassembler() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * (ROM_Window::romW_factor + Debug_Window::debugW_factor + Display_Window::displayW_factor), 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * disassemblerW_factor, Graphics::height * disassemblerH_factor));
		ImGui::Begin("Disassembler", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::Checkbox("Follow PC", &follow_pc);
		ImGui::Separator();

		uint16_t current_instruction = ((*CPU::pc) << 8) | *(CPU::pc + 1);

		if (CPU::pc != nullptr) {
			char instr[5];
			CPU::decToHex(instr, current_instruction);

			if (instr[0] == '\0') {
				printf("Error: instruction is empty?\n"); // Should not happen. Means instruction is empty
			}
			else if (instr[1] == '\0') {
				char temp1 = instr[0];

				instr[0] = instr[1] = instr[2] = '0';
				instr[3] = temp1;
				instr[4] = '\0';
			}
			else if (instr[2] == '\0') {
				char temp1 = instr[0];
				char temp2 = instr[1];

				instr[0] = instr[1] = '0';
				instr[2] = temp1;
				instr[3] = temp2;
				instr[4] = '\0';
			}
			else if (instr[3] == '\0') {
				char temp1 = instr[0];
				char temp2 = instr[1];
				char temp3 = instr[2];

				instr[0] = '0';
				instr[1] = temp1;
				instr[2] = temp2;
				instr[3] = temp3;
				instr[4] = '\0';
			}

			ImGui::Text("Current instruction: %s", instr);

			ImGui::Separator();
		}

		ImGui::BeginChild("Disassembler", ImVec2(), NULL, ImGuiWindowFlags_HorizontalScrollbar);

		int k = 1;
		for (int i = 0; i < RAM::totalSpaceInBytes; i += k) {
			ImGui::Selectable("", i == CPU::pc - RAM::buffer);
			if (follow_pc && i == CPU::pc - RAM::buffer) {
				ImGui::SetScrollHereY(0);
			}
			ImGui::SameLine();

			char addr[5], instr[5];
			CPU::decToHex(addr, i);

			char desc[40] = "";

			if (i < RAM::interpreterSpaceInBytes) {
				CPU::decToHex(instr, RAM::buffer[i]);
			}
			else if ((i - RAM::interpreterSpaceInBytes) > RAM::rom_size) {
				instr[0] = '0';
				instr[1] = '0';
				instr[2] = '\0';
			}
			else {
				uint16_t instruction = (RAM::buffer[i] << 8) | (RAM::buffer[i + 1]);
				CPU::decToHex(instr, instruction);

				if (instr[0] == '\0') {
					printf("Error: instruction is empty?\n");
				}
				else if (instr[1] == '\0') {
					char temp1 = instr[0];

					instr[0] = instr[1] = instr[2] = '0';
					instr[3] = temp1;
					instr[4] = '\0';
				}
				else if (instr[2] == '\0') {
					char temp1 = instr[0];
					char temp2 = instr[1];

					instr[0] = instr[1] = '0';
					instr[2] = temp1;
					instr[3] = temp2;
					instr[4] = '\0';
				}
				else if (instr[3] == '\0') {
					char temp1 = instr[0];
					char temp2 = instr[1];
					char temp3 = instr[2];

					instr[0] = '0';
					instr[1] = temp1;
					instr[2] = temp2;
					instr[3] = temp3;
					instr[4] = '\0';
				}

				CPU::get_instruction_desc_from_value(desc, instruction, true);
			}



			if (i < RAM::interpreterSpaceInBytes) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0, 0, 1.0));

				if (i <= 0xF) {
					ImGui::Text("0x%s   | %s   | %s", addr, instr, desc);
				}
				else if (i <= 0xFF) {
					ImGui::Text("0x%s  | %s   | %s", addr, instr, desc);
				}
				else {
					ImGui::Text("0x%s | %s   | %s", addr, instr, desc);
				}


				ImGui::PopStyleColor();
			}
			else if ((i - RAM::interpreterSpaceInBytes) > RAM::rom_size) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 1.0, 1.0));
				ImGui::Text("0x%s | %s   | %s", addr, instr, desc);
				ImGui::PopStyleColor();
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1.0, 0, 1.0));
				ImGui::Text("0x%s | %s | %s", addr, instr, desc);
				ImGui::PopStyleColor();
			}
		}

		ImGui::EndChild();
		ImGui::End();
	}
};