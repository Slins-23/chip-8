#include "CPU.h"
#include <nfd.h>
#include <cstring>
#include <cmath>
#include <SDL_audio.h>
#include "Debug.h"

int main(int argc, char* argv[]) {

	/*
	uint8_t nums[] = { 0xFF, 0x0A, 0xA0 };

	for (int j = 0; j < sizeof(nums) / sizeof(uint8_t); j++) {
		char str[8];
		_itoa_s(nums[j], str, 16);

		std::cout << "Num: " << (int)nums[j] << " (" << str << ")" << std::endl;
		for (int i = 0; i < 8; i++) {
			std::cout << "Bit " << (i + 1) << " (right to left): " << ((nums[j] >> i) & 1) << std::endl;
		}

		std::cout << '\n';
	}
	*/
	if (Graphics::initialize() && RAM::initialize()) {
		srand(time(0));
		//double frameStart;
		//double frametime;
		// std::chrono::microseconds::zero();
		//std::chrono::time_point<std::chrono::steady_clock> frameStart;
		//std::chrono::duration<double, std::micro> frametime;
		uint32_t frameStart;
		uint32_t frametime;

		// Sets window to initial stage, then start fetching, decoding, then executing instructions concurrently
		//Graphics::clear_window();
		//Graphics::update_renderer();
		//NFD_Init();

		//CPU::init_threads();
		CPU::pc = RAM::buffer + RAM::interpreterSpaceInBytes;
		Windows windows;
		//CPU::start_cpu_thread();
		int sampled = 0;
		float sampled_framerate = 0;

		int frame_delay = 1;
		int frame_sound = 1;

		uint32_t time_start = SDL_GetTicks();

		//Sound snd;

		
		//if (SDL_OpenAudio(&want, &have) != 0) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
		//if (want.format != have.format) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec");
		//have.silence = false;

		 // start playing sound
		 // wait while sound is playing
		//SDL_PauseAudio(1); // stop playing sound

		//SDL_CloseAudio();
		
		//CPU::start_sound_thread();
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
			//SDL_AudioDeviceID devid = SDL_OpenAudioDevice("Speakers (High Definition Audio Device)", false, &want, &have, 0);

			//std::cout << "Deviceid: " << devid << std::endl;
			//SDL_PauseAudioDevice(3, false);
			//toot(440, 1000);
			//toot(440, 1000);

			//SDL_Delay(1000);


			
			/*
			if (CPU::status == Status::running && (CPU::pc - RAM::buffer) % 2 != 0) {
				printf("Weird...\n");
			}
			*/

			//printf("Is held? %d\n", CPU::is_held);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			frameStart = SDL_GetTicks();
			//frameStart = std::chrono::high_resolution_clock::now();
			//glClear(GL_COLOR_BUFFER_BIT);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//Graphics::clear_window();



			//ImGui_ImplOpenGL3_NewFrame();
			while (SDL_PollEvent(&Graphics::event)) {
				ImGui_ImplSDL2_ProcessEvent(&Graphics::event);

				switch (Graphics::event.type) {
					case SDL_QUIT:
						//CPU::close_sound_thread();
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

							/*
							uint32_t* new_pixels = (uint32_t*)malloc(Graphics::width * Graphics::height * sizeof(uint32_t));
							free(Graphics::copy_buffer);
							Graphics::copy_buffer = (uint32_t*)malloc(Graphics::width * Graphics::height * sizeof(uint32_t));

							int old_tile_height = Graphics::tile_height;
							int old_tile_width = Graphics::tile_width;

							Graphics::horizontal_scale = Graphics::width / Graphics::horizontal_tiles;
							Graphics::vertical_scale = Graphics::height / Graphics::vertical_tiles;

							Graphics::tile_width = Graphics::horizontal_scale;
							Graphics::tile_height = Graphics::vertical_scale;

							for (int i = 0; i < Graphics::width * Graphics::height; i++) {
								new_pixels[i] = 0xFF0000FF;
							}

							for (int tile_y = 0; tile_y < Graphics::vertical_tiles; tile_y++) {
								for (int tile_x = 0; tile_x < Graphics::horizontal_tiles; tile_x++) {
									uint32_t current_tile_color = Graphics::pixels[(old_tile_height * tile_y * old_width) + (old_tile_width * tile_x)];
									Graphics::fill_tile(new_pixels, Graphics::width, Graphics::height, tile_x * Graphics::horizontal_scale, tile_y * Graphics::vertical_scale, Graphics::tile_width, Graphics::tile_height, current_tile_color);
								}
							}

							free(Graphics::pixels);
							Graphics::pixels = new_pixels;
							memcpy(Graphics::copy_buffer, Graphics::pixels, Graphics::width * Graphics::height * sizeof(uint32_t));

							

							SDL_DestroyTexture(Graphics::texture);
							Graphics::texture = SDL_CreateTexture(Graphics::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Graphics::width, Graphics::height);

							*/

							windows.debugBtnSize = ImVec2(Graphics::width * windows.debugW_factor * 0.83, 0);

							/*
							int old_width = Graphics::width;
							int old_height = Graphics::height;
							Graphics::width = Graphics::event.window.data1;
							Graphics::height = Graphics::event.window.data2;

							uint32_t* new_pixels = (uint32_t*)malloc(Graphics::width * Graphics::height * sizeof(uint32_t));
							free(Graphics::copy_buffer);
							Graphics::copy_buffer = (uint32_t*)malloc(Graphics::width * Graphics::height * sizeof(uint32_t));

							int old_tile_height = Graphics::tile_height;
							int old_tile_width = Graphics::tile_width;

							Graphics::horizontal_scale = Graphics::width / Graphics::horizontal_tiles;
							Graphics::vertical_scale = Graphics::height / Graphics::vertical_tiles;

							Graphics::tile_width = Graphics::horizontal_scale;
							Graphics::tile_height = Graphics::vertical_scale;

							for (int i = 0; i < Graphics::width * Graphics::height; i++) {
								new_pixels[i] = 0xFF0000FF;
							}

							for (int tile_y = 0; tile_y < Graphics::vertical_tiles; tile_y++) {
								for (int tile_x = 0; tile_x < Graphics::horizontal_tiles; tile_x++) {
									uint32_t current_tile_color = Graphics::pixels[(old_tile_height * tile_y * old_width) + (old_tile_width * tile_x)];
									Graphics::fill_tile(new_pixels, Graphics::width, Graphics::height, tile_x * Graphics::horizontal_scale, tile_y * Graphics::vertical_scale, Graphics::tile_width, Graphics::tile_height, current_tile_color);
								}
							}

							free(Graphics::pixels);
							Graphics::pixels = new_pixels;
							memcpy(Graphics::copy_buffer, Graphics::pixels, Graphics::width * Graphics::height * sizeof(uint32_t));

		

							SDL_DestroyTexture(Graphics::texture);
							Graphics::texture = SDL_CreateTexture(Graphics::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Graphics::width, Graphics::height);

							windows.debugBtnSize = ImVec2(Graphics::width * windows.debugW_factor * 0.83, 0);
							*/

							// Adjust viewport after window gets resized
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

						/*
						if (CPU::halt) {
							CPU::pressed_key = current_key_hexa;
							CPU::halt = false;
						}
						*/

						//Graphics::handle_key(current_key_hexa);
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

			/*
			glLoadIdentity();
			glRasterPos2i(-1, 1);
			glPixelZoom(1, -1);
			glBindTexture(GL_TEXTURE_2D, Graphics::textureId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Graphics::width, Graphics::height, GL_UNSIGNED_INT_8_8_8_8, GL_RGBA, Graphics::pixels);
			//glBindTexture(GL_TEXTURE_2D, 0);
			glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);
			glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);   glVertex3f(1.0f, -1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);   glVertex3f(1.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f, 1.0f, 0.0f);
			glEnd();
			*/

			// unbind texture
			//glBindTexture(GL_TEXTURE_2D, 0);
			//glDrawPixels(Graphics::width, Graphics::height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, Graphics::pixels);
			//glFlush();
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Graphics::width, Graphics::height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, Graphics::pixels);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Graphics::width, Graphics::height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, Graphics::pixels);

			//SDL_RenderClear(Graphics::renderer);
			//if (CPU::sound == 0) SDL_PauseAudioDevice(Graphics::deviceId, true);

			//SDL_UpdateTexture(Graphics::texture, nullptr, Graphics::pixels, sizeof(uint32_t)* Graphics::width);
			SDL_UpdateTexture(Graphics::texture, nullptr, Graphics::original_pixels, sizeof(uint32_t)* Graphics::horizontal_tiles);

			glClear(GL_COLOR_BUFFER_BIT);
			if (Graphics::render_windows) {
				
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplSDL2_NewFrame(Graphics::window);
				ImGui::NewFrame();


				{
					//Windows::average();
					windows.rom_info();
					windows.cpu_info();
					windows.stack_info();
					windows.debug_info();
					windows.keyboard_info();
					windows.display_info();
					windows.memory_info();
					windows.disassembler_info();
				}
				
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
			else {
				//glClearColor(0.5f, 0.2f, 0.2f, 1.f);
				//glUseProgram(shaderProgram);
				
				//glUniform1i(texUni, 0.3f);
				glBindTexture(GL_TEXTURE_2D, Graphics::textureFbo);
				
				// Necessary otherwise doesn't show anything
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				// Updates bound texture
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 32, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, Graphics::original_pixels);

				// Binds and draws VAO
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				//glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(vertices[0]));

				// Unbinds VAO and texture to avoid confliction
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);

				//SDL_RenderCopy(Graphics::renderer, Graphics::texture, nullptr, nullptr);
			}

			SDL_GL_SwapWindow(Graphics::window);
			

			//SDL_RenderClear(Graphics::renderer);
			//SDL_UpdateTexture(Graphics::texture, nullptr, Graphics::pixels, Graphics::width * sizeof(uint32_t));
			//SDL_RenderCopy(Graphics::renderer, Graphics::texture, nullptr, nullptr);

			//glUseProgram(0);
			//SDL_RenderPresent(Graphics::renderer);
			
			//Graphics::update_renderer();

			//glViewport(0, 0, (int) Graphics::io->DisplaySize.x, (int)Graphics::io->DisplaySize.y);
			//glClearColor(0.0, 0.0, 0.3f, 0.5f);
			

			
			//SDL_RenderFlush(Graphics::renderer);


			//SDL_RenderPresent(Graphics::renderer);
			
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

				/*
				for (int i = 0; i < Graphics::width * Graphics::height; i++) {
					if (Graphics::pixels[i] == Graphics::old_background) {
						Graphics::pixels[i] = Graphics::background;
						Graphics::copy_buffer[i] = Graphics::pixels[i];
					}
					else if (Graphics::pixels[i] == Graphics::old_foreground) {
						Graphics::pixels[i] = Graphics::foreground;
						Graphics::copy_buffer[i] = Graphics::pixels[i];
					}
				}
				*/
			}
			

			if (CPU::status == Status::running) {
				if (CPU::sound != 0) SDL_PauseAudioDevice(Graphics::deviceId, false);
				if (CPU::sound == 0) SDL_PauseAudioDevice(Graphics::deviceId, true);
				CPU::step_many(CPU::instructions_per_frame);

				//printf("Dec: %d\n", CPU::delay_decrement_per_frame);

				if (!CPU::delayIsZero) {
					if (CPU::delay_decrement_per_frame >= 1) {
						CPU::decrement_delay((int)round(CPU::delay_decrement_per_frame));
					}
					else {
						//printf("Del Dec P F: %f\n", CPU::delay_decrement_per_frame);
						int res_d = (int)round(1 / (double)CPU::delay_decrement_per_frame);

						//printf("Res_d: %d | Frame_delay: %d\n", res_d, frame_delay);

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
				/*
				* 
				if (!CPU::delayIsZero) {
					if (CPU::delay_decrement_per_frame >= 1) {
						CPU::decrement_delay((int)round(CPU::delay_decrement_per_frame));
					}
					else {
						//printf("Del Dec P F: %f\n", CPU::delay_decrement_per_frame);
						int res_d = (int)round(1 / (double)CPU::delay_decrement_per_frame);

						//printf("Res_d: %d | Frame_delay: %d\n", res_d, frame_delay);

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

				*/

				/*
				float frameRate = ImGui::GetIO().Framerate;

				//printf("Instructions per frame: %d\n", CPU::instructions_per_frame);

				sampled++;
				temp_fps += frameRate;

				if (temp_fps < Graphics::FPS && (sampled % 10) == 0) { // If framerate is less tha what we manually stipulated
					temp_fps /= 10;
					sampled = 0;
					CPU::instructions_per_frame = CPU::instructionsPerSecond / temp_fps;
					//printf("OLD DECPF1: %f\n", CPU::delay_decrement_per_frame);
					CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / temp_fps;
					//printf("NEW DECPF1: %f\n", CPU::delay_decrement_per_frame);
					CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / temp_fps;
				}
				else if (Graphics::FPS != 0 && (sampled % 10) == 0) {
					temp_fps /= 10;
					sampled = 0;
					CPU::instructions_per_frame = CPU::instructionsPerSecond / Graphics::FPS;
					//printf("OLD DECPF2: %f\n", CPU::delay_decrement_per_frame);
					CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / Graphics::FPS;
					//printf("NEW DECPF2: %f\n", CPU::delay_decrement_per_frame);
					CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / Graphics::FPS;
				}
				*/
			}

			//printf("Instructions per frame: %d\n", CPU::instructions_per_frame);

			
			//if ((temp_fps / sampled) < Graphics::FPS && (sampled % 10000) == 0) { // If framerate is less tha what we manually stipulated
			/*
			else if (Graphics::FPS != 0 && (sampled % 10000) == 0) {
				temp_fps = 0;
				sampled = 0;
				CPU::instructions_per_frame = CPU::instructionsPerSecond / Graphics::FPS;
				//printf("OLD DECPF2: %f\n", CPU::delay_decrement_per_frame);
				CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / Graphics::FPS;
				//printf("NEW DECPF2: %f\n", CPU::delay_decrement_per_frame);
				CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / Graphics::FPS;
			}
			*/

			/*
			if (CPU::sound_decrement_per_frame >= 1) {
				CPU::decrement_delay(round(CPU::sound_decrement_per_frame));
			}
			else {
				int res_d = round(1 / CPU::sound_decrement_per_frame);

				if (res_d == frame_sound) {
					frame_sound = 1;
					CPU::decrement_sound(1);
				}
				else {
					frame_sound++;
				}
			}
			*/

			frametime = SDL_GetTicks() - frameStart;
			//frametime = std::chrono::high_resolution_clock::now() - frameStart;
			//CPU::current_framerate = 1000000 / frametime.count();
			//printf("Framerate: %.2f\n", CPU::current_framerate);

			if (Graphics::frameDelay > frametime) {
				SDL_Delay(Graphics::frameDelay - frametime);
				//std::this_thread::sleep_for(Graphics::frameDelay - frametime);
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

			//Graphics::upscale_pixels();
			//cpu.fetch();
			//cpu.decode(0x00);
			//cpu.exec(0x00);

			//snd.stop();
		}

	}
	else {
		std::cout << "Could not initialize." << std::endl;
	}

	return 0;	
}

void print_bytes(uint16_t* buffer, int bit_count) {
	for (int i = 0; i < bit_count; i++) {
		std::cout << (((*buffer) >> 15 - (i % 16)) & 0x1);
		if (((i + 1) % 16) == 0) {
			std::cout << "  ";
			buffer++;
		}
		else if (((i + 1) % 4) == 0) {
			std::cout << " ";
		}
	}
}

int countSetBits(void* ptr, int start, int end) {
	uint8_t* _ptr = (uint8_t*)ptr;
	int set = 0;

	for (int i = 0; i < end - start; i++) {
		set += (((*_ptr) >> (7 - (i % 8))) & 0x1);
		// gets value of current byte
			// get proper cycle of i
			// bit shift to the right to get the bit from current iteration
			// and current bit with 1. if current bit is set, result is 1, otherwise is 0
		// add and assign the result to set

		if (((i + 1) % 8) == 0) {
			_ptr++; // Goes to next byte
		}
		else if (((i + 1) % 4) == 0) {
			// Reaches next half-byte
		}
	}

	return set;
}