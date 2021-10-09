#include "CPU.h"
#include <nfd.h>
#include <cstring>
#include <cmath>
#include <SDL_audio.h>
#include "Window_Manager.h"

int main(int argc, char* argv[]) {

	// Initializes the Graphics and RAM variables
	if (Graphics::initialize() && RAM::initialize()) {
		// Sets a new rng seed
		srand(time(0));

		// Tracks the time in ms at the beginning of the current frame (SDL_GetTicks())
		uint32_t frameStart;

		// Time in ms that the current frame took to complete (SDL_GetTicks() - frameStart)
		// Does not include the frame delay. i.e. If the frame took 2ms to finish, but the framerate
		// is capped at 17ms, frametime will be 2ms, not 17ms.
		uint32_t frametime;

		// Sets the program counter to address 0x200, which is where ROMs are loaded
		CPU::pc = RAM::buffer + RAM::interpreterSpaceInBytes;

		// How many frames to wait for in order to update the framerate by taking the avg of
		int sampling_interval = 10;

		// Tracks how many frames have been completed since taking the last FPS sample
		// Resets to 0 every "sampling_interval" frames
		int elapsed_frames = 0;

		// This variable keeps getting incremented by the current framerate,
		// which is calculated based solely on the completion time
		// (including the delay) of the current frame.
		// 1000.f / (SDL_GetTicks() - frameStart)
		// Resets to 0 every "sampling_interval" frames after, taking the average
		float sampled_framerate = 0;

		// Tracks how many frames have completed since
		// a instruction was run
		int frame_instruction = 1;

		// Tracks how many frames have completed since
		// the delay timer was last decremented
		int frame_delay = 1;

		// Tracks how many frames have completed since
		// the sound timer was last decremented
		int frame_sound = 1;

		// Clears the renderer to the color in "Graphics.h"
		// defined in the "glClearColor" call.
		// This color influences the background of the
		// debug info window, as it's somewhat transparent
		SDL_RenderClear(Graphics::renderer);

		// Vertices coordinates, texture coordinates (Y flipped)
		GLfloat vertices[] =
		{
			-0.5f, -0.5f,		0.0f, 1.0f,
			-0.5f,  0.5f,		0.0f, 0.0f,
			 0.5f,  0.5f,		1.0f, 0.0f,
			 0.5f, -0.5f,		1.0f, 1.0f
		};

		// Triangle indices, upper then lower
		GLuint indices[] =
		{
			0, 2, 1,
			0, 3, 2
		};

		// Vertex shader for the texture when not rendering
		// the debug windows.
		// Takes in a scale variable, which will dictate by
		// how much to upscale/downscale the rendered texture.
		const char* vxShader =
			"in vec2 aPos;\n"
			"in vec2 aTex;\n"
			"out vec2 texCoord;\n"
			"uniform float scale;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, 0.0, 1.0);\n"
			"	texCoord = aTex;\n"
			"}\n";

		// Fragment shader for the texture when not rendering
		// the debug windows.
		// Takes the texture coordinates as an argument
		// (their y values are flipped as you can see above),
		// as well as a 2D sampler
		const char* ftShader = 
			"out vec4 FragColor;\n"
			"in vec2 texCoord;\n"
			"uniform sampler2D tex;\n"
			"void main()\n"
			"{\n"
			"	FragColor = texture(tex, texCoord);\n"
			"}\n";


		// Used to append at the first line of both shaders the glsl version
		// Which can be found in the "glsl_ver" variable of the Graphics file
		const GLchar* vertexShaderSource[2] = {Graphics::glsl_ver, vxShader};
		const GLchar* fragmentShaderSource[2] = { Graphics::glsl_ver, ftShader };


		// Create and compile both shaders, then attach and link to a shader program
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

		// Deletes the shaders as they're not needed anymore
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// Setup the vertex array, vertex buffer and element array buffer objects
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

		// Initializes the shader program
		glUseProgram(shaderProgram);

		// Binds the "scale" vertex input to the uniID variable,
		// then modify it through the glUniform call.
		// Value 1 to fill the whole screen
		GLuint uniID = glGetUniformLocation(shaderProgram, "scale");
		glUniform1f(uniID, 1.f); // Makes texture stretch the whole screen

		while (true) {
			frameStart = SDL_GetTicks();

			// Handling of SDL events
			// Likely to be moved to a separate place in the future
			while (SDL_PollEvent(&Graphics::event)) {
				// Process ImGui events
				ImGui_ImplSDL2_ProcessEvent(&Graphics::event);

				switch (Graphics::event.type) {
					

					// Free all allocated memory once SDL_QUIT is triggered
					case SDL_QUIT:
						CPU::close();
						Graphics::close();
						RAM::close();
						return 1;
					case SDL_WINDOWEVENT:
						switch (Graphics::event.window.event) 
						{
						// Update the window dimensions variable to the new one
						// after being resized, then update the OpenGL viewport
						// in order to draw to and display the correct portion of the window
						case SDL_WINDOWEVENT_RESIZED:
							Graphics::width = Graphics::event.window.data1;
							Graphics::height = Graphics::event.window.data2;

							glViewport(0, 0, Graphics::width, Graphics::height);
							break;
						}
						break;
					case SDL_KEYDOWN:
					{
						switch (Graphics::event.key.keysym.scancode) {
						// Once 'H' has been pressed, toggle whether to render the debug window 
						case SDL_SCANCODE_H:
							Graphics::render_windows ^= 1;
							break;

						// (Yet to implement) Once 'T' has been pressed, toggle whether to draw tiles over emulator
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

						// Pause/resume the emulation if 'Space' is pressed
						case SDL_SCANCODE_SPACE:
							if (CPU::status == Status::paused) {
								CPU::status = Status::running;
							}
							else if (CPU::status == Status::running) {
								CPU::status = Status::paused;
							}
							break;

						// Pause/resume the emulation if 'P' is pressed
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

						// If the key that was pressed is one of the
						// emulator keys, trigger an emulator key press
						SDL_Scancode current_key = Graphics::event.key.keysym.scancode;
						uint8_t current_key_hexa = Graphics::scancode_to_hexa(current_key);
						CPU::HandlePressDown(current_key_hexa);
					}
					break;
					case SDL_KEYUP: {

						// If the key that was released is one of the
						// emulator keys, trigger an emulator key release
						SDL_Scancode current_key = Graphics::event.key.keysym.scancode;
						uint8_t current_key_hexa = Graphics::scancode_to_hexa(current_key);
						CPU::HandlePressUp(current_key_hexa);
					}
						break;
					default:
						break;
				}
			}

			// Clears the color buffer
			glClear(GL_COLOR_BUFFER_BIT);

			// Updates the texture
			Graphics::update_texture();

			// Renders ImGui windows if "render_windows" flag is set to true
			// It can be toogled by pressing 'H' or clicking the button in the
			// debug tile. It is turned on by default.
			if (Graphics::render_windows) {
				
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplSDL2_NewFrame(Graphics::window);
				ImGui::NewFrame();

				// Draws all windows in a row
				Window_Manager::draw_windows();
				
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
			else {
				// Binds texture
				glBindTexture(GL_TEXTURE_2D, Graphics::textureFbo);

				// Binds and draws VAO
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				// Unbinds VAO and texture to avoid confliction
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			// Updates the window
			SDL_GL_SwapWindow(Graphics::window);
			
			// Replaces all pixels with the new colors after a color has been changed in the debug menu
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
			

			// If emulation is running
			if (CPU::status == Status::running) {

				// Play beep if sound timer is not 0, stop it if it is
				if (CPU::sound != 0) SDL_PauseAudioDevice(Graphics::deviceId, false);
				if (CPU::sound == 0) SDL_PauseAudioDevice(Graphics::deviceId, true);

				// If we should run at least one instruction per second
				if (CPU::instructionsPerSecond > 0) {

					// Steps through the amount of instructions that should
					// be executed every frame if at least one instruction
					// should be executed per frame
					if (CPU::instructions_per_frame >= 1) {
						CPU::step_many((int)round(CPU::instructions_per_frame));
					}

					// Otherwise, wait until enough frames have completed
					// then step through a single instruction
					else {
						int frame_goal = (int)round(1 / (double)CPU::instructions_per_frame);

						if (frame_goal == frame_instruction) {
							frame_instruction = 1;
							CPU::step_one();
						}
						else if (frame_goal > frame_instruction) {
							frame_instruction++;
						}
						else if (frame_goal < frame_instruction) {
							frame_instruction = 1;
						}
					}
				}

				
				// If we should decrease the delay timer at least once per second
				if (CPU::delayDecPerSec > 0) {

					// If for every frame we should decrease at least once,
					// then decrement by the rounded approximation
					// of the amount we should decrease
					if (CPU::delay_decrement_per_frame >= 1) {
						CPU::decrement_delay((int)round(CPU::delay_decrement_per_frame));

					}

					// If for every frame we should decrease less than once,
					// calculate how many frames it should take to make a rounded approximation of
					// 1 decrement, then decrement by 1 every time the counter reaches
					// that goal.
					else {
						int frame_goal = (int)round(1 / (double)CPU::delay_decrement_per_frame);

						if (frame_goal == frame_delay) {
							frame_delay = 1;
							CPU::decrement_delay(1);
						}
						else if (frame_goal > frame_delay) {
							frame_delay++;
						}
						else if (frame_goal < frame_delay) {
							frame_delay = 1;
						}
					}
				}

				// If we should decrease the sound timer at least once per second
				if (CPU::soundDecPerSec > 0) {

					// If for every frame we should decrease at least once,
					// then decrement by the rounded approximation
					// of the amount we should decrease
					if (CPU::sound_decrement_per_frame >= 1) {
						CPU::decrement_sound((int)round(CPU::sound_decrement_per_frame));
					}

					// If for every frame we should decrease less than once,
					// calculate how many frames it should take to make a rounded approximation of
					// 1 decrement, then decrement by 1 every time the counter reaches
					// that goal.
					else {
						int frame_goal = (int)round(1 / (double)CPU::sound_decrement_per_frame);

						if (frame_goal == frame_sound) {
							frame_sound = 1;
							CPU::decrement_sound(1);
						}
						else if (frame_goal > frame_sound) {
							frame_sound++;
						}
						else if (frame_goal < frame_sound) {
							frame_sound = 1;
						}
					}
				}

				
			}
			// If the emulation is paused or not running, stop the sound
			else if (CPU::status == Status::paused || CPU::status == Status::stopped) {
				SDL_PauseAudioDevice(Graphics::deviceId, true);
			}

			// Calculates the frametime, not including the frame delay
			frametime = SDL_GetTicks() - frameStart;

			// Delays the frame in order to reach the desired FPS limit if the frame
			// finished too fast
			if (Graphics::frameDelay > frametime) {
				SDL_Delay(Graphics::frameDelay - frametime);
			}

			// Add sample to samples
			sampled_framerate += 1000.f / (SDL_GetTicks() - frameStart);

			// Update frame count since last sampling
			elapsed_frames++;

			// If the desired amount of frames have been completed
			// sample the framerate and store it in the CPU file,
			// this is the framerate that is shown in the debug window
			if (elapsed_frames == sampling_interval) {
				// Take the average framerate
				CPU::current_framerate = sampled_framerate / sampling_interval;

				// Reset elapsed frames and samples
				elapsed_frames = 0;
				sampled_framerate = 0;

				// Updates the amount of instructions to be executed per frame,
				// amount that the delay timer should be decremented per frame,
				// amount that the sound timer should be decremented per frame,
				// based on the current framerate and any possible change of some 
				// variables in the debug menu
				if (Graphics::FPS > 0) {
					CPU::instructions_per_frame = (double)CPU::instructionsPerSecond / CPU::current_framerate;
					CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / CPU::current_framerate;
					CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / CPU::current_framerate;
					/*
					if (Graphics::FPS < 60) {
						CPU::instructions_per_frame = (double) CPU::instructionsPerSecond / std::min((float)Graphics::FPS, CPU::current_framerate);
						CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / std::min((float) Graphics::FPS, CPU::current_framerate);
						CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / std::min((float) Graphics::FPS, CPU::current_framerate);
					}
					else {
						CPU::instructions_per_frame = (double) CPU::instructionsPerSecond / std::min(60.f, CPU::current_framerate);
						CPU::delay_decrement_per_frame = (double)CPU::delayDecPerSec / std::min(60.f, CPU::current_framerate);
						CPU::sound_decrement_per_frame = (double)CPU::soundDecPerSec / std::min(60.f, CPU::current_framerate);
					}
					*/
				}
				

			}
			
			// Prints the average of the last "sampling_interval" frames
#ifdef CH_DEBUG
			printf("Framerate: %f\n", CPU::current_framerate);
#endif
		}

	}
	else {
		std::cout << "Could not initialize." << std::endl;
	}

	return 0;	
}