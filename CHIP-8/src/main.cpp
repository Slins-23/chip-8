#include "CPU.h"
#include <nfd.h>
#include <cstring>
#include <cmath>
#include <SDL_audio.h>
#include "Window_Manager.h"

int main(int argc, char* argv[]) {

	if (Graphics::initialize() && RAM::initialize()) {
		srand(time(0));
		uint32_t frameStart;
		uint32_t frametime;

		CPU::pc = RAM::buffer + RAM::interpreterSpaceInBytes;
		int sampled = 0;
		float sampled_framerate = 0;

		int frame_delay = 1;
		int frame_sound = 1;

		uint32_t time_start = SDL_GetTicks();

		SDL_RenderClear(Graphics::renderer);
		int frameK = SDL_GetTicks();

		const char* vxShader = 			//"layout (location = 0) in vec2 aPos;\n"
	//"layout (location = 1) in vec2 aTex;\n"
			"in vec2 aPos;\n"
			"in vec2 aTex;\n"
			"out vec2 texCoord;\n"
			"uniform float scale;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, 0.0, 1.0);\n"
			"	texCoord = aTex;\n"
			"}\n";

		const char* ftShader = 
			"out vec4 FragColor;\n"
			"in vec2 texCoord;\n"
			"uniform sampler2D tex;\n"
			"void main()\n"
			"{\n"
			"	FragColor = texture(tex, texCoord);\n"
			"}\n";

		const GLchar* vertexShaderSource[2] = {Graphics::glsl_ver, vxShader};
		const GLchar* fragmentShaderSource[2] = { Graphics::glsl_ver, ftShader };


		GLuint indices[] =
		{
			0, 2, 1, // Upper triangle
			0, 3, 2 // Lower triangle
		};

		// Vertices coordinates, texture coordinates (Y flipped)
		GLfloat vertices[] =
		{
			-0.5f, -0.5f,		0.0f, 1.0f,
			-0.5f,  0.5f,		0.0f, 0.0f,
			 0.5f,  0.5f,		1.0f, 0.0f,
			 0.5f, -0.5f,		1.0f, 1.0f
		};

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertexShader, 2, vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		glShaderSource(fragmentShader, 2, fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		glLinkProgram(shaderProgram);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		GLuint EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(shaderProgram);

		GLuint uniID = glGetUniformLocation(shaderProgram, "scale");
		GLuint texUni = glGetUniformLocation(shaderProgram, "tex");
		glUniform1i(texUni, 0);
		glUniform1f(uniID, 1.f); // Makes texture stretch the whole screen

		while (true) {
			frameStart = SDL_GetTicks();

			while (SDL_PollEvent(&Graphics::event)) {
				ImGui_ImplSDL2_ProcessEvent(&Graphics::event);

				switch (Graphics::event.type) {
					case SDL_QUIT:
						CPU::close();
						Graphics::close();
						RAM::close();
						return 1;
					case SDL_WINDOWEVENT:
						switch (Graphics::event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							int old_width = Graphics::width;
							int old_height = Graphics::height;
							Graphics::width = Graphics::event.window.data1;
							Graphics::height = Graphics::event.window.data2;

							Debug_Window::debugBtnSize = ImVec2(Graphics::width * Debug_Window::debugW_factor * 0.83, 0);
							glViewport(0, 0, Graphics::width, Graphics::height);
							break;
						}
						break;
					case SDL_KEYDOWN:
					{
						switch (Graphics::event.key.keysym.scancode) {
						case SDL_SCANCODE_H:
							Graphics::render_windows ^= 1;

							if (Graphics::render_windows) {
								//SDL_RenderClear(Graphics::renderer);
							}
							break;
						case SDL_SCANCODE_T:

							Graphics::show_tiles ^= 1; 

							/*

							if (Graphics::show_tiles) {
								Graphics::draw_tiles();
							}
							else {
								memcpy(Graphics::pixels, Graphics::copy_buffer, Graphics::width * Graphics::height * sizeof(uint32_t));
							}

							*/
							break;
						case SDL_SCANCODE_SPACE:
							if (CPU::status == Status::paused) {
								CPU::status = Status::running;
							}
							else if (CPU::status == Status::running) {
								CPU::status = Status::paused;
							}
							break;
						case SDL_SCANCODE_P:
							if (CPU::status == Status::paused) {
								CPU::status = Status::running;
							}
							else if (CPU::status == Status::running) {
								CPU::status = Status::paused;
							}
							break;
						default:
							break;
						}

						SDL_Scancode current_key = Graphics::event.key.keysym.scancode;
						uint8_t current_key_hexa = Graphics::scancode_to_hexa(current_key);
						CPU::HandlePressDown(current_key_hexa);
					}
					break;
					case SDL_KEYUP: {
						SDL_Scancode current_key = Graphics::event.key.keysym.scancode;
						uint8_t current_key_hexa = Graphics::scancode_to_hexa(current_key);
						CPU::HandlePressUp(current_key_hexa);
					}
						break;
					default:
						break;
				}
			}

			SDL_UpdateTexture(Graphics::texture, nullptr, Graphics::original_pixels, sizeof(uint32_t)* Graphics::horizontal_tiles);

			glClear(GL_COLOR_BUFFER_BIT);
			if (Graphics::render_windows) {
				
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplSDL2_NewFrame(Graphics::window);
				ImGui::NewFrame();

				Window_Manager::draw_windows();
				/*
				{
					windows.rom_info();
					windows.cpu_info();
					windows.stack_info();
					windows.debug_info();
					windows.keyboard_info();
					windows.display_info();
					windows.memory_info();
					windows.disassembler_info();
					
				}
				*/
				
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
			else {
				glBindTexture(GL_TEXTURE_2D, Graphics::textureFbo);
				
				// Necessary otherwise doesn't show anything
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				// Updates bound texture
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 32, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, Graphics::original_pixels);

				// Binds and draws VAO
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				// Unbinds VAO and texture to avoid confliction
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			SDL_GL_SwapWindow(Graphics::window);
			
			// Replacing all pixels with new colors after color has been changed in debug menu
			if (Graphics::background != Graphics::old_background || Graphics::foreground != Graphics::old_foreground) {
				for (int i = 0; i < Graphics::horizontal_tiles * Graphics::vertical_tiles; i++) {
					if (Graphics::original_pixels[i] == Graphics::old_background) {
						Graphics::original_pixels[i] = Graphics::background;
					}
					else if (Graphics::original_pixels[i] == Graphics::old_foreground) {
						Graphics::original_pixels[i] = Graphics::foreground;
					}
				}
			}
			

			if (CPU::status == Status::running) {
				if (CPU::sound != 0) SDL_PauseAudioDevice(Graphics::deviceId, false);
				if (CPU::sound == 0) SDL_PauseAudioDevice(Graphics::deviceId, true);
				CPU::step_many(CPU::instructions_per_frame);

				if (!CPU::delayIsZero) {
					if (CPU::delay_decrement_per_frame >= 1) {
						CPU::decrement_delay((int)round(CPU::delay_decrement_per_frame));
					}
					else {
						int res_d = (int)round(1 / (double)CPU::delay_decrement_per_frame);

						if (res_d == frame_delay) {
							frame_delay = 1;
							CPU::decrement_delay(1);
						}
						else if (res_d > frame_delay) {
							frame_delay++;
						}
						else if (res_d < frame_delay) {
							frame_delay = 1;
						}
					}
				}

				if (!CPU::soundIsZero) {
					if (CPU::sound_decrement_per_frame >= 1) {
						CPU::decrement_sound((int)round(CPU::sound_decrement_per_frame));
					}
					else {
						int res_d = (int)round(1 / (double)CPU::sound_decrement_per_frame);

						if (res_d == frame_sound) {
							frame_sound = 1;
							CPU::decrement_sound(1);
						}
						else if (res_d > frame_sound) {
							frame_sound++;
						}
						else if (res_d < frame_sound) {
							frame_sound = 1;
						}
					}
				}

				
			}
			else if (CPU::status == Status::paused || CPU::status == Status::stopped) {
				SDL_PauseAudioDevice(Graphics::deviceId, true);
			}

			frametime = SDL_GetTicks() - frameStart;

			if (Graphics::frameDelay > frametime) {
				SDL_Delay(Graphics::frameDelay - frametime);
			}

			sampled_framerate += 1000.f / (SDL_GetTicks() - frameStart);
			sampled++;

			if (sampled == 10) {
				CPU::current_framerate = sampled_framerate / 10;
				sampled = 0;
				sampled_framerate = 0;

				
				if (Graphics::FPS > 0) {
					if (Graphics::FPS < 60) {
						CPU::instructions_per_frame = CPU::instructionsPerSecond / std::min((float)Graphics::FPS, CPU::current_framerate);
						CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / std::min((float) Graphics::FPS, CPU::current_framerate);
						CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / std::min((float) Graphics::FPS, CPU::current_framerate);
					}
					else {
						CPU::instructions_per_frame = CPU::instructionsPerSecond / std::min(60.f, CPU::current_framerate);
						CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / std::min(60.f, CPU::current_framerate);
						CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / std::min(60.f, CPU::current_framerate);
					}
				}
				

			}
			
			printf("Framerate: %f\n", CPU::current_framerate);
		}

	}
	else {
		std::cout << "Could not initialize." << std::endl;
	}

	return 0;	
}