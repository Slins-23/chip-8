#include "Graphics.h"
#include "CPU.h"
#include <nfd.h>
#include <algorithm>

struct Windows {
	float romH_factor = 0.2;
	float romW_factor = 0.25;

	float cpuH_factor = 0.4;
	float cpuW_factor = 0.25;

	float stackH_factor = 0.4;
	float stackW_factor = 0.2;

	float debugH_factor = 0.6;
	float debugW_factor = 0.15;

	float keyboardH_factor = 0.4;
	float keyboardW_factor = 0.2;

	//float displayH_factor = 0.6;
	//float displayW_factor = 0.45;

	float displayH_factor = 0.6;
	float displayW_factor = 0.4;

	float memoryH_factor = 0.4;
	float memoryW_factor = 0.4;

	float disassemblerH_factor = 1.0;
	float disassemblerW_factor = 0.2;

	//const int btnSize = 50;


	int step = 10;
	int skip = 10;

	bool follow_pc = true;

	ImVec2 debugBtnSize = ImVec2(Graphics::width * Windows::debugW_factor * 0.83, 0);

	nfdchar_t* path;

	ImVec2 btnSize;

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
		//printf("Color: %d\n", ImGui::GetStyle().Colors[ImGuiCol_Button]);

		if ((CPU::keys >> hex_code) & 1) {
			//printf("HERE\n"); Problem occurs because the hover color takes precedence over active color, therefore overwrites
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			ImGui::Button(hex_letter, ImVec2(btnSize.x, btnSize.y));
			//ImGui::GetStyle().Colors
			ImGui::PopStyleColor();
		}
		else {
			ImGui::Button(hex_letter, ImVec2(btnSize.x, btnSize.y));
		}

		if (ImGui::IsItemActivated()) {
			//printf("ACT\n");
			CPU::HandlePressDown(hex_code);
		}

		if (ImGui::IsItemDeactivated()) {
			//printf("DEAC\n");
			CPU::HandlePressUp(hex_code);
		}
	}

	int keyboard_spacing(int row, int vertical_spacing, int initialY_padding) {
		return row * (btnSize.y + vertical_spacing) + initialY_padding;
	}

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

	void average() {
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width / 3, Graphics::height - 20));
		ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Text("Testing...");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

	void rom_info() {
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Windows::romW_factor, Graphics::height * Windows::romH_factor));
		//| ImGuiWindowFlags_HorizontalScrollbar
		ImGui::Begin("ROM", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::TextWrapped("ROM Title: %s", RAM::rom_title);
		ImGui::TextWrapped("ROM Path: %s", RAM::rom_path);
		ImGui::TextWrapped("Rom Size (Bytes): %d", RAM::rom_size);
		ImGui::End();
	}

	void cpu_info() {
		char pc[5], I[5], DT[5], ST[5];
		char V0[5], V1[5], V2[5], V3[5], V4[5], V5[5], V6[5], V7[5];
		char V8[5], V9[5], VA[5], VB[5], VC[5], VD[5], VE[5], VF[5];

		CPU::decToHex(pc, CPU::pc - RAM::buffer);
		CPU::decToHex(I, *((uint16_t*) (CPU::register_pointer + 16)));
		CPU::decToHex(DT, CPU::delay);
		CPU::decToHex(ST, CPU::sound);
		
		/*
		CPU::decToHex(V0, *CPU::register_pointer);
		CPU::decToHex(V1, *(CPU::register_pointer + 1));
		CPU::decToHex(V2, *(CPU::register_pointer + 2));
		CPU::decToHex(V3, *(CPU::register_pointer + 3));
		CPU::decToHex(V4, *(CPU::register_pointer + 4));
		CPU::decToHex(V5, *(CPU::register_pointer + 5));
		CPU::decToHex(V6, *(CPU::register_pointer + 6));
		CPU::decToHex(V7, *(CPU::register_pointer + 7));
		CPU::decToHex(V8, *(CPU::register_pointer + 8));
		CPU::decToHex(V9, *(CPU::register_pointer + 9));
		CPU::decToHex(VA, *(CPU::register_pointer + 10));
		CPU::decToHex(VB, *(CPU::register_pointer + 11));
		CPU::decToHex(VC, *(CPU::register_pointer + 12));
		CPU::decToHex(VD, *(CPU::register_pointer + 13));
		CPU::decToHex(VE, *(CPU::register_pointer + 14));
		CPU::decToHex(VF, *(CPU::register_pointer + 15));
		*/

		ImGui::SetNextWindowPos(ImVec2(0, Graphics::height * Windows::romH_factor), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Windows::cpuW_factor, Graphics::height * Windows::cpuH_factor));
		ImGui::Begin("CPU", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Separator();
		ImGui::Columns(4, 0);
		ImGui::Text("PC ");
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Text("0x%s", pc);
		ImGui::Text("  %d", CPU::pc - RAM::buffer);
		//ImGui::TextWrapped("PC: 0x%s (%d)", pc, CPU::pc - RAM::buffer);
		ImGui::NextColumn();
		ImGui::Text("IR ");
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Text("0x%s", I);
		ImGui::Text("  %d", *((uint16_t*) (CPU::register_pointer + 16)));
		//ImGui::TextWrapped("I: 0x%s (%d)", I, *(CPU::register_pointer + 16));
		ImGui::NextColumn();
		ImGui::Text("Delay ");
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Text("0x%s", DT);
		ImGui::Text("  %d", CPU::delay);
		//ImGui::TextWrapped("DT: 0x%s (%d)", DT, CPU::delay);
		ImGui::NextColumn();
		ImGui::Text("Sound ");
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::Text("0x%s", ST);
		ImGui::Text("  %d", CPU::sound);
		//ImGui::TextWrapped("ST: 0x%s (%d)", ST, CPU::sound);
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
		//ImGui::Text("%.1f FPS (%.3f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS (%.3f ms/frame) ", CPU::current_framerate, 1000.f / CPU::current_framerate);
		ImGui::End();
	}

	void stack_info() {
		ImGui::SetNextWindowPos(ImVec2(0, Graphics::height * (Windows::romH_factor + Windows::cpuH_factor)), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Windows::stackW_factor, Graphics::height * Windows::stackH_factor));
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

	void debug_info() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * Windows::romW_factor, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Windows::debugW_factor, Graphics::height * Windows::debugH_factor));
		ImGui::Begin("Debug", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);
		//debugBtnSize = ImVec2(Graphics::width * Windows::debugW_factor - 35, 0);
		debugBtnSize = ImVec2(ImGui::GetContentRegionAvail().x, 0);

		if (ImGui::Button("Load ROM", debugBtnSize)) {
			static const nfdchar_t* filters = "ch8";
			//nfdresult_t result = NFD_OpenDialog(&path, filters, 1, NULL);
			nfdresult_t result = NFD_OpenDialog(filters, nullptr, &path);

			if (result == NFD_OKAY) {
				CPU::reset();
				RAM::load_rom(path);
			}
			else if (result == NFD_CANCEL) {

			}
			else {
				printf("Error: %s\n", NFD_GetError());
				exit(-1);
			}

		}

		if (CPU::status == Status::stopped && RAM::rom_data != nullptr) {
			if (ImGui::Button("Start ROM", debugBtnSize)) {
				CPU::start();
			}
		}
		else {
			if (CPU::status == Status::stopped) {
				ImGui::BeginDisabled();
				ImGui::Button("Start ROM", debugBtnSize);
				ImGui::EndDisabled();
			}
			else if (CPU::status == Status::running) {
				if (ImGui::Button("Pause (P / Space)", debugBtnSize)) CPU::status = Status::paused;
			}
			else if (CPU::status == Status::paused) {
				if (ImGui::Button("Resume (P / Space)", debugBtnSize)) CPU::status = Status::running;
			}
		}

		/*
		if (CPU::status == Status::stopped) {
			ImGui::BeginDisabled();
			ImGui::Button("Pause (P / Space)", debugBtnSize);
			ImGui::EndDisabled();
		}
		else if (CPU::status == Status::running) {
			if (ImGui::Button("Pause (P / Space)", debugBtnSize)) CPU::status = Status::paused;
		}
		else if (CPU::status == Status::paused) {
			if (ImGui::Button("Resume (P / Space)", debugBtnSize)) CPU::status = Status::running;
		}
		*/

		if (CPU::status == Status::stopped) {
			ImGui::BeginDisabled();
			ImGui::Button("Reload ROM", debugBtnSize);
			ImGui::Button("Reset ROM", debugBtnSize);
			ImGui::EndDisabled();
		}
		else {
			if (ImGui::Button("Reload ROM", debugBtnSize)) {
				CPU::start();
			}

			if (ImGui::Button("Reset ROM", debugBtnSize)) {
				CPU::reset();
			}
		}



		ImGui::Text("Status: ");
		ImGui::SameLine();

		switch (CPU::status) {
		case Status::running:
			ImGui::Text("Running");
			break;
		case Status::paused:
			ImGui::Text("Paused");
			break;
		case Status::stopped:
			ImGui::Text("Stopped");
			break;
		default:
			break;
		}

		ImGui::Separator();

		if (CPU::status == Status::running || RAM::rom_data == nullptr) {
			ImGui::BeginDisabled();
		}

		if (ImGui::Button("Step One", debugBtnSize)) {
			CPU::status = Status::paused;
			CPU::step_one();
		}


		char stp[10] = "Step ";
		std::to_chars(stp + 5, stp + 9, step);

		if (ImGui::Button(stp, ImVec2(debugBtnSize.x, 0))) {
			CPU::status = Status::paused;
			CPU::step_many(step);
		}

		ImGui::PushItemWidth(debugBtnSize.x);

		if (ImGui::InputInt("##Step", &step, 1, 10)) {
			if (step < 0) step = abs(step);
		}

		ImGui::Dummy(ImVec2(0, 5));

		if (ImGui::Button("Skip One", debugBtnSize)) {
			CPU::status = Status::paused;
			CPU::skip_one();
		}

		char skp[10] = "Skip ";
		std::to_chars(skp + 5, skp + 9, skip);

		if (ImGui::Button(skp, ImVec2(debugBtnSize.x, 0))) {
			CPU::status = Status::paused;
			CPU::skip_many(skip);
		}

		ImGui::PushItemWidth(debugBtnSize.x);

		if (ImGui::InputInt("##Skip", &skip, 1, 10)) {
			if (skip < 0) skip = abs(skip);
		}

		if (CPU::status == Status::running || RAM::rom_data == nullptr) {
			ImGui::EndDisabled();
		}

		ImGui::Separator();

		ImGui::Text("Instructions/s: ");

		ImGui::SameLine();
		ImGui::PushItemWidth(40);
		if (ImGui::InputInt("##Instructions", &CPU::instructionsPerSecond, 0)) {
			if (Graphics::FPS > 0) {
				if (CPU::instructionsPerSecond < 0) CPU::instructionsPerSecond = abs(CPU::instructionsPerSecond);

				// 50 | 65 -> 50
				// 50 | 40 -> 40

				// 70 | 80 -> 60
				
				// 70 | 65 -> 60
				// 70 | 40 -> 40
				if (Graphics::FPS < 60) {
					CPU::instructions_per_frame = CPU::instructionsPerSecond / std::min((float)Graphics::FPS, CPU::current_framerate);
				}
				else {
					CPU::instructions_per_frame = CPU::instructionsPerSecond / std::min(CPU::current_framerate, 60.f);
				}

				if (CPU::delayDecPerSec > 0) {
					CPU::instructions_per_delay_decrement = CPU::instructionsPerSecond / CPU::delayDecPerSec;
				}
				else {
					CPU::instructions_per_delay_decrement = 0;
				}

				if (CPU::soundDecPerSec > 0) {
					CPU::instructions_per_sound_decrement = CPU::instructionsPerSecond / CPU::soundDecPerSec;
				}
				else {
					CPU::instructions_per_sound_decrement = 0;
				}
				

				/*
				if (Graphics::FPS < 60 && CPU::current_framerate >= Graphics::FPS) {
					CPU::instructions_per_frame = CPU::instructionsPerSecond / Graphics::FPS;
				}
				else if (Graphics::FPS < 60 && CPU::current_framerate < Graphics::FPS) {
					CPU::instructions_per_frame = CPU::instructionsPerSecond / CPU::current_framerate;
				}
				else if (Graphics::FPS >= 60 && CPU::current_framerate >= Graphics::FPS) {
					CPU::instructions_per_frame = CPU::instructionsPerSecond / 60;
				}
				else if (Graphics::FPS >= 60 && CPU::current_framerate < Graphics::FPS && CPU::current_framerate > 60) {
					CPU::instructions_per_frame = CPU::instructionsPerSecond / 60;
				}
				else if (Graphics::FPS >= 60 && CPU::current_framerate < Graphics::FPS && CPU::current_framerate <= 60) {
					CPU::instructions_per_frame = CPU::instructionsPerSecond / CPU::current_framerate;
				}
				*/


			}


		}

		ImGui::Text("Delay dec/s: ");

		ImGui::SameLine();
		ImGui::PushItemWidth(30);

		if (ImGui::InputInt("##Delay", &CPU::delayDecPerSec, 0)) {
			if (CPU::delayDecPerSec < 0) CPU::delayDecPerSec = abs(CPU::delayDecPerSec);
			CPU::delayIsZero = (CPU::delayDecPerSec == 0 ? true : false);

			if (!CPU::delayIsZero) {
				CPU::instructions_per_delay_decrement = round((double)CPU::instructionsPerSecond / CPU::delayDecPerSec);

				/*
				//float framerate_current = ImGui::GetIO().Framerate;
				if (CPU::current_framerate < Graphics::FPS && Graphics::FPS > 0) {
					//CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / ImGui::GetIO().Framerate;
					CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / CPU::current_framerate;
				}
				else if (Graphics::FPS > 0 && CPU::current_framerate >= 60) {
					//CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / Graphics::FPS;
					CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / 60;
				}
				else {
					CPU::delay_decrement_per_frame = 0;
				}
				*/

				if (Graphics::FPS > 0) {
					if (Graphics::FPS < 60) {
						CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / std::min((float)Graphics::FPS, CPU::current_framerate);
					}
					else {
						CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / std::min(CPU::current_framerate, 60.f);
					}
				}

			}
		}

		ImGui::Text("Sound dec/s: ");

		ImGui::SameLine();
		ImGui::PushItemWidth(30);
		if (ImGui::InputInt("##Sound", &CPU::soundDecPerSec, 0)) {
			if (CPU::soundDecPerSec < 0) CPU::soundDecPerSec = abs(CPU::soundDecPerSec);
			CPU::soundIsZero = (CPU::soundDecPerSec == 0 ? true : false);

			if (!CPU::soundIsZero) {
				CPU::instructions_per_sound_decrement = round((double)CPU::instructionsPerSecond / CPU::soundDecPerSec);

				//float framerate_current = ImGui::GetIO().Framerate;
				/*
				if (CPU::current_framerate < Graphics::FPS && Graphics::FPS > 0) {
					CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / CPU::current_framerate;
				}
				else if (CPU::current_framerate >= Graphics::FPS && Graphics::FPS > 0) {
					//CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / Graphics::FPS;
					CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / 60;
				}
				else {
					CPU::sound_decrement_per_frame = 0;
				}
				*/
				if (Graphics::FPS > 0) {
					if (Graphics::FPS < 60) {
						CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / std::min((float)Graphics::FPS, CPU::current_framerate);
					}
					else {
						CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / std::min(CPU::current_framerate, 60.f);
					}
				}

			}
		}

		ImGui::Text("Beep Freq: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(50);

		if (ImGui::InputDouble("##SOUNDFREQ", &Graphics::sine_frequency, 0, 0, "%.2f", 0)) {
			if (Graphics::sine_frequency < 0) Graphics::sine_frequency = abs(Graphics::sine_frequency);
			Graphics::samples_per_sine = Graphics::sample_frequency / Graphics::sine_frequency;
		}

		ImGui::Text("FPS Limit: ");

		ImGui::SameLine();
		ImGui::PushItemWidth(40);
		if (ImGui::InputInt("##FPS", &Graphics::FPS, 0)) {
			if (Graphics::FPS < 0) Graphics::FPS = abs(Graphics::FPS);
			if (Graphics::FPS > 0) {
				
				//Graphics::frameDelay = std::chrono::duration<double, std::micro>(1000000 / Graphics::FPS);
				Graphics::frameDelay = 1000 / Graphics::FPS;

				//printf("Framerate: %f | FPS: %d\n", framerate, Graphics::FPS);


				// 600 | 700 -> ok
				// 600 | 400 -> ok
				// 600 | 30 -> ok

				// 60 | 80 -> ok
				// 60 | 40 -> ok

				// 30 | 40 -> ok
				// 30 | 15 -> t

				
				if (Graphics::FPS < 60) {
					CPU::instructions_per_frame = CPU::instructionsPerSecond / std::min((float)Graphics::FPS, CPU::current_framerate);
					CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / std::min((float)Graphics::FPS, CPU::current_framerate);
					CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / std::min((float)Graphics::FPS, CPU::current_framerate);
				}
				else {
					CPU::instructions_per_frame = CPU::instructionsPerSecond / std::min(60.f, CPU::current_framerate);
					CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / std::min(60.f, CPU::current_framerate);
					CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / std::min(60.f, CPU::current_framerate);
				}
				

			}
			else {
				//Graphics::frameDelay = (pow(2, 32) / 2) - 1;
				//CPU::delay_decrement_per_frame = 0;
				//CPU::sound_decrement_per_frame = 0;
			}

		}

		ImGui::Text("VSync: ");

		ImGui::SameLine();
		if (ImGui::Checkbox("", &Graphics::VSync)) {
			if (Graphics::VSync) {
				SDL_GL_SetSwapInterval(1);
			}
			else {
				SDL_GL_SetSwapInterval(0);
			}
		}

		ImGui::Text("BG Color: ");

		ImGui::SameLine();
		if (ImGui::ColorEdit4("##imBG", Graphics::imBG, ImGuiColorEditFlags_NoInputs)) {
			Graphics::old_background = Graphics::background;
			uint32_t tmp = static_cast<uint8_t>(Graphics::imBG[0] * 255) << 24 | static_cast<uint8_t>(Graphics::imBG[1] * 255) << 16 | static_cast<uint8_t>(Graphics::imBG[2] * 255) << 8 | static_cast<uint8_t>(Graphics::imBG[3] * 255);
			if (tmp != Graphics::foreground)  Graphics::background = tmp;
		}
		//ImGui::SameLine();
		/*
		if (ImGui::ColorButton("#BGC", Graphics::imBG, NULL)) {
			Graphics::background =(static_cast<uint8_t>(Graphics::imBG.x) << 24) | (static_cast<uint8_t>(Graphics::imBG.y) << 16) | (static_cast<uint8_t>(Graphics::imBG.z) << 8) | (static_cast<uint8_t>(Graphics::imBG.w));
		}
		*/

		ImGui::Text("FG Color: ");

		ImGui::SameLine();
		if (ImGui::ColorEdit4("##imFG", Graphics::imFG, ImGuiColorEditFlags_NoInputs)) {
			Graphics::old_foreground = Graphics::foreground;
			uint32_t tmp = static_cast<uint8_t>(Graphics::imFG[0] * 255) << 24 | static_cast<uint8_t>(Graphics::imFG[1] * 255) << 16 | static_cast<uint8_t>(Graphics::imFG[2] * 255) << 8 | static_cast<uint8_t>(Graphics::imFG[3] * 255);
			if (tmp != Graphics::background) Graphics::foreground = tmp;
		}





		/*
		if (ImGui::ColorButton("#FGC", Graphics::imFG, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueWheel)) {
			Graphics::foreground = (static_cast<uint8_t>(Graphics::imFG.x) << 24) | (static_cast<uint8_t>(Graphics::imFG.y) << 16) | (static_cast<uint8_t>(Graphics::imFG.z) << 8) | (static_cast<uint8_t>(Graphics::imFG.w));
		}
		*/

		if (ImGui::Button("Toggle Debug (H)", debugBtnSize)) {
			Graphics::render_windows ^= 1;

			if (Graphics::render_windows) {
				//SDL_RenderClear(Graphics::renderer);
			}
		}

		ImGui::Separator();

		if (ImGui::Button("Reset Settings", debugBtnSize)) {
			Graphics::FPS = 60;

			Graphics::old_background = Graphics::background;
			Graphics::old_foreground = Graphics::foreground;
			Graphics::background = 0x000000FF;
			Graphics::foreground = 0xFFFFFFFF;

			Graphics::imBG[0] = static_cast<uint8_t>(Graphics::background >> 24) / 255;
			Graphics::imBG[1] = static_cast<uint8_t>(Graphics::background >> 16) / 255;
			Graphics::imBG[2] = static_cast<uint8_t>(Graphics::background >> 8) / 255;
			Graphics::imBG[3] = static_cast<uint8_t>(Graphics::background) / 255;

			Graphics::imFG[0] = static_cast<uint8_t>(Graphics::foreground >> 24) / 255;
			Graphics::imFG[1] = static_cast<uint8_t>(Graphics::foreground >> 16) / 255;
			Graphics::imFG[2] = static_cast<uint8_t>(Graphics::foreground >> 8) / 255;
			Graphics::imFG[3] = static_cast<uint8_t>(Graphics::foreground) / 255;

			Graphics::VSync = true;

			CPU::delayDecPerSec = CPU::soundDecPerSec = 60;
			CPU::instructionsPerSecond = 700;
			CPU::millisecondsPerInstruction = 1000 / CPU::instructionsPerSecond;
			CPU::instructions_per_delay_decrement = round((double)CPU::instructionsPerSecond / CPU::delayDecPerSec);
			CPU::instructions_per_sound_decrement = round((double)CPU::instructionsPerSecond / CPU::soundDecPerSec);
			CPU::delayIsZero = CPU::soundIsZero = false;
			CPU::instructions_per_frame = CPU::instructionsPerSecond / Graphics::FPS;
			CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / Graphics::FPS;
			CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / Graphics::FPS;
			Graphics::frameDelay = 1000 / Graphics::FPS;

			Graphics::sine_frequency = 440.0;
			Graphics::samples_per_sine = Graphics::sample_frequency / Graphics::sine_frequency;
		}

		ImGui::End();
	}

	void keyboard_info() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * Windows::stackW_factor, Graphics::height * Windows::debugH_factor), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Windows::keyboardW_factor, Graphics::height * Windows::keyboardH_factor));
		ImGui::Begin("Keyboard", NULL, ImGuiWindowFlags_NoScrollWithMouse |  ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

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
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("2", 0x2);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("3", 0x3);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("C", "4", 0xC);

		ImGui::SetCursorPos(ImVec2(initialX_padding * 0.8, keyboard_spacing(1, vertical_padding, initialY_padding * 1.15)));
		MakeNCheckBtn("4", "Q", 0x4);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("5", "W", 0x5);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("6", "E", 0x6);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("D", "R", 0xD);

		ImGui::SetCursorPos(ImVec2(initialX_padding * 0.8, keyboard_spacing(2, vertical_padding, initialY_padding * 1.15)));
		MakeNCheckBtn("7", "A", 0x7);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("8", "S", 0x8);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("9", "D", 0x9);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("E", "F", 0xE);

		ImGui::SetCursorPos(ImVec2(initialX_padding * 0.8, keyboard_spacing(3, vertical_padding, initialY_padding * 1.15)));
		MakeNCheckBtn("A", "Z", 0xA);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("0", "X", 0x0);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("B", "C", 0xB);
		ImGui::SameLine();
		//ImGui::SameLine(horizontal_padding);
		MakeNCheckBtn("F", "V", 0xF);

		ImGui::End();
	}

	void display_info() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * (Windows::romW_factor + Windows::debugW_factor), 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Windows::displayW_factor, Graphics::height * Windows::displayH_factor));
		ImGui::Begin("Display", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::BeginChild("Emulator");
		ImVec2 size = ImGui::GetWindowSize();
		glBindFramebuffer(GL_FRAMEBUFFER, Graphics::fbo);
		glBindTexture(GL_TEXTURE_2D, Graphics::textureFbo);
		//glClear(GL_COLOR_BUFFER_BIT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Graphics::horizontal_tiles, Graphics::vertical_tiles, 0, GL_RGBA, GL_UNSIGNED_BYTE, Graphics::original_pixels);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, Graphics::pixels);
		// 0 after height should be 1 for border
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Graphics::rbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Graphics::textureFbo, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
			//ImGui::Image((ImTextureID)Graphics::fbo, size);
			//ImGui::Image((ImTextureID)Graphics::fbo, ImVec2(size.x, size.y));
			ImGui::Image((ImTextureID)Graphics::fbo, size);



			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);

		}
		else {
			printf("Could not initialize framebuffer.\n");
			exit(-1);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ImGui::EndChild();
		ImGui::End();
	}

	void memory_info() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * (Windows::stackW_factor + Windows::keyboardW_factor), Graphics::height * Windows::displayH_factor), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Windows::memoryW_factor, Graphics::height * Windows::memoryH_factor));
		ImGui::Begin("Memory", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//ImGui::PushItemWidth(0.9 * memoryW_factor / 2);
		/*
		ImGui::SetNextItemWidth(-1);
		if (ImGui::SliderInt("##", &RAM::bytes_per_row, 1, 16, "%d")) {
			RAM::byte_rows = ceil(RAM::rom_size / RAM::bytes_per_row);
		}
		*/

		/*
		ImGui::Columns(3);
		ImGui::SetColumnWidth(0, (0.1 * (memoryW_factor * Graphics::width)));
		ImGui::SetColumnWidth(1, (0.7 * (memoryW_factor * Graphics::width)));
		*/
		//ImGui::SetColumnWidth(2, (0.6 * (memoryW_factor * Graphics::width)));
		//ImGui::SetColumnWidth(0, 50);
		//ImGui::SetColumnWidth(1, 300);
		for (int i = 0; i < RAM::byte_rows; i++) {
			char addr[5];
			CPU::NdecToHex(addr, i, RAM::bytes_per_row);
			//ImGui::Text("0x%s | ", addr);

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

			//ImGui::Text("%s |", bytesrep);
			//free(bytes);

			//bytes = (uint8_t*)malloc(RAM::bytes_per_row);
			//memcpy(bytes, RAM::rom_data + (16 * i), RAM::bytes_per_row);

			char charrep[17];

			for (int j = 0; j < RAM::bytes_per_row; j++) {
				charrep[j] = bytes[j];
				if (charrep[j] < 0x20) charrep[j] = 0x2E; // 0x2E == '.'

				if (j == RAM::bytes_per_row - 1) {
					charrep[j + 1] = '\0';
				}
			}

			//ImGui::Text("%s", charrep);
			free(bytes);

			if (i >= 16) {
				ImGui::Text("0x%s0 | %s | %s", addr, bytesrep, charrep);
			}
			else {
				ImGui::Text("0x%s0  | %s | %s", addr, bytesrep, charrep);
			}
		}

		/*
		for (int i = 0; i < RAM::byte_rows; i++) {
			char addr[5];
			CPU::NdecToHex(addr, i, RAM::bytes_per_row);
			ImGui::Text("0x%s", addr);
		}

		//ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
		ImGui::NextColumn();

		for (int i = 0; i < RAM::byte_rows; i++) {
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

			ImGui::Text("%s", bytesrep);
			free(bytes);
		}

		//ImGui::PushItemWidth(20);


		ImGui::NextColumn();

		for (int i = 0; i < RAM::byte_rows; i++) {
			uint8_t* bytes = (uint8_t*)malloc(RAM::bytes_per_row);
			memcpy(bytes, RAM::rom_data + (16 * i), RAM::bytes_per_row);

			char charrep[17];

			for (int j = 0; j < RAM::bytes_per_row; j++) {
				charrep[j] = bytes[j];
				if (charrep[j] < 0x20) charrep[j] = 0x2E; // 0x2E == '.'

				if (j == RAM::bytes_per_row - 1) {
					charrep[j + 1] = '\0';
				}
			}

			ImGui::Text("%s", charrep);
			free(bytes);
		}
		*/

		/*

		for (int i = 0; i < RAM::byte_rows; i++) {
			uint8_t* bytes = (uint8_t*)malloc(RAM::bytes_per_row);
			memcpy(bytes, RAM::rom_data + (16 * i), RAM::bytes_per_row);

			char addr[5];
			CPU::decToHex(addr, i);

			char bytesrep[48];
			char charrep[17];

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


				charrep[j] = bytes[j];

				if (charrep[j] < 0x20) charrep[j] = 0x2E; // 0x2E == '.'

				if (j == RAM::bytes_per_row - 1) {
					bytesrep[(3 * j) + 2] = '\0';
					charrep[j + 1] = '\0';
				}
				else {
					bytesrep[(3 * j) + 2] = ' ';
				}


			}

			if (i < 16) {
				ImGui::Text("0x%s0  |", addr);
			}
			else {
				ImGui::Text("0x%s0 |", addr);
			}

			ImGui::SameLine();
			ImGui::Text("%s |", bytesrep);
			ImGui::SameLine();
			ImGui::Text(" %s", charrep);
			//ImGui::TextWrapped(" %s", charrep);

			free(bytes);
		}

		*/

		/*
		ImGui::Columns(3);

		for (int i = 0; i < RAM::byte_rows; i++) {
			ImGui::Column
		}

		for (int i = 0; i < RAM::rom_size; i++) {
			char addr[5];
			CPU::decToHex(addr, RAM::rom_data[i]);

			ImGui::Text(" %s ", addr);
			ImGui::SameLine();
		}
		*/

		ImGui::Columns();
		ImGui::End();

		// use columns
	}

	void disassembler_info() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * (Windows::romW_factor + Windows::debugW_factor + Windows::displayW_factor), 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Windows::disassemblerW_factor, Graphics::height * Windows::disassemblerH_factor));
		ImGui::Begin("Disassembler", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::Checkbox("Follow PC", &follow_pc);
		ImGui::Separator();

		uint16_t current_instruction = ((*CPU::pc) << 8) | *(CPU::pc + 1);

		if (CPU::pc != nullptr) {
			char instr[5];
			CPU::decToHex(instr, current_instruction);

			if (instr[0] == '\0') {
				exit(-1);
				//addr[0] = addr[1] = addr[2] = addr[3] = '0';
				//addr[4] = '\0';
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


		/*
		static int activeItem = 0;
		static const char* items[2];
		// 512 + ((4096 - 512) / 2)
		items[0] = "Ok";
		items[1] = "Not";

		ImGui::ListBox("", &activeItem, items, IM_ARRAYSIZE(items));
		*/

		ImGui::BeginChild("Disassembler", ImVec2(), NULL, ImGuiWindowFlags_HorizontalScrollbar);

		int k = 1;
		for (int i = 0; i < RAM::totalSpaceInBytes; i += k) {
			//if (i == RAM::interpreterSpaceInBytes) k = 2;

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

				//printf("I: %d | Val[i]: %d | CPU::decToHex(Val[i]): %s\n", i, instruction, instr);
				//printf("I[0]: %s", *instr);
				//exit(-1);

				if (instr[0] == '\0') {
					printf("Error: instruction is empty?\n");
					//exit(-1);
					//addr[0] = addr[1] = addr[2] = addr[3] = '0';
					//addr[4] = '\0';
				}
				else if (instr[1] == '\0') {
					//printf("Here\n");
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

			//if (desc != "") free(desc); // Important...
		}

		/*
		*
		for (int i = 512; i < RAM::totalSpaceInBytes; i += 2) {
			ImGui::Selectable("", i == CPU::pc - RAM::buffer);
			if (follow_pc && i == CPU::pc - RAM::buffer) {
				//printf("TETO\n");
				ImGui::SetScrollHereY(0);
			}
			else {
				//printf("i: %d | CPU::pc: %d | RAM::buffer: %d\n", i, CPU::pc, RAM::buffer);
			}

			ImGui::SameLine();

			char addr[5];
			CPU::decToHex(addr, i);

			if ((i - 512) > RAM::rom_size) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 1.0, 1.0));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1.0));
			}

			ImGui::Text("0x%s", addr);
			ImGui::PopStyleColor();
		}

		*/

		/*
		ImGui::Columns(3);
		for (int i = 0; i < 512; i++) {
			ImGui::Selectable("", i == CPU::pc - RAM::buffer, ImGuiSelectableFlags_SpanAllColumns);
			if (follow_pc && i == CPU::pc - RAM::buffer) {
				ImGui::SetScrollHereY(0);
			}

			ImGui::SameLine();

			char addr[5];
			CPU::decToHex(addr, i);

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0, 0, 1.0));
			ImGui::Text("0x%s", addr);
			ImGui::PopStyleColor();
		}

		for (int i = 512; i < RAM::totalSpaceInBytes; i += 2) {
			ImGui::Selectable("", i == CPU::pc - RAM::buffer, ImGuiSelectableFlags_SpanAllColumns);
			if (follow_pc && i == CPU::pc - RAM::buffer) {
				//printf("TETO\n");
				ImGui::SetScrollHereY(0);
			}
			else {
				//printf("i: %d | CPU::pc: %d | RAM::buffer: %d\n", i, CPU::pc, RAM::buffer);
			}

			ImGui::SameLine();

			char addr[5];
			CPU::decToHex(addr, i);

			if ((i - 512) > RAM::rom_size) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 1.0, 1.0));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1.0));
			}

			ImGui::Text("0x%s", addr);
			ImGui::PopStyleColor();
		}

		ImGui::NextColumn();

		for (int i = 0; i < 512; i++) {
			char addr[5];
			CPU::decToHex(addr, RAM::buffer[i]);

			ImGui::Text("%s", addr);
		}

		for (int i = 512; i < RAM::totalSpaceInBytes; i += 2) {
			char addr[5];
			uint16_t instruction = (RAM::buffer[i] << 8) | (RAM::buffer[i + 1]);
			CPU::decToHex(addr, instruction);

			if (addr[3] == '\0') {
				char temp1 = addr[0];
				char temp2 = addr[1];
				char temp3 = addr[2];

				addr[0] = '0';
				addr[1] = temp1;
				addr[2] = temp2;
				addr[3] = temp3;
				addr[4] = '\0';
			}
			else if (addr[2] == '\0') {
				char temp1 = addr[0];
				char temp2 = addr[1];

				addr[0] = addr[1] = '0';
				addr[2] = temp1;
				addr[3] = temp2;
				addr[4] = '\0';
			}
			else if (addr[1] == '\0') {
				char temp1 = addr[0];

				addr[0] = addr[1] = addr[2] = '0';
				addr[3] = temp1;
				addr[4] = '\0';
			}
			else if (addr[0] == '\0') {
				exit(-1);
				//addr[0] = addr[1] = addr[2] = addr[3] = '0';
				//addr[4] = '\0';
			}

			ImGui::Text("%s", addr);
		}

		ImGui::NextColumn();
		*/

		// Put instruction description here
		ImGui::EndChild();
		ImGui::End();

		// use columns
	}
};
