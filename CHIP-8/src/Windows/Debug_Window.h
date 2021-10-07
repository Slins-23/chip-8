#include "../CPU.h"
#include <nfd.h>
#include "Stack_Window.h"

#pragma once
namespace Debug_Window {
	nfdchar_t* path;
	float debugH_factor = 0.6;
	float debugW_factor = 0.15;

	int step = 10;
	int skip = 10;

	ImVec2 debugBtnSize = ImVec2(Graphics::width * debugW_factor * 0.83, 0);

	void draw_debug() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * ROM_Window::romW_factor, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * Debug_Window::debugW_factor, Graphics::height * Debug_Window::debugH_factor));
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
};