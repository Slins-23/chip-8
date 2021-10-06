#include <stdlib.h>
#include <cstdint>
#include <iostream>
#include <SDL.h>
#include <cassert>
#include <glad/glad.h>
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include <SDL_audio.h>

struct Colors {
	static const uint32_t Tile = 0x10101010;
};

#pragma once
namespace Graphics
{
	//uint32_t background = 0x2F7F82FF;
	//uint32_t foreground = 0x184345FF;
	uint32_t background = 0x000000FF;
	uint32_t foreground = 0xFFFFFFFF;

	uint32_t old_background = background;
	uint32_t old_foreground = foreground;

	float imBG[4] = { static_cast<float>(static_cast<uint8_t>(background >> 24) / 255), static_cast<float>(static_cast<uint8_t>(background >> 16) / 255), static_cast<float>(static_cast<uint8_t>(background >> 8) / 255), static_cast<float>(static_cast<uint8_t>(background) / 255) };
	float imFG[4] = { static_cast<float>(static_cast<uint8_t>(foreground >> 24) / 255), static_cast<float>(static_cast<uint8_t>(foreground >> 16) / 255), static_cast<float>(static_cast<uint8_t>(foreground >> 8) / 255), static_cast<float>(static_cast<uint8_t>(foreground) / 255) };



	//ImVec4 imBG = ImVec4(background >> 24, background >> 16, background >> 8, background);
	//ImVec4 imFG = ImVec4(static_cast<uint8_t>(foreground >> 24), static_cast<uint8_t>(foreground >> 16), static_cast<uint8_t>(foreground >> 8), static_cast<uint8_t>(foreground));


	uint32_t* pixels; // RGBA
	uint32_t* copy_buffer;
	uint32_t* original_pixels;

	int avgFPS = 0;
	int FPS = 60;
	//std::chrono::duration<std::chrono::microseconds>(1000000 / Graphics::FPS)
	//std::chrono::duration<double, std::micro> frameDelay (1000000 / Graphics::FPS);
	uint32_t frameDelay = 1000 / Graphics::FPS;
	const int horizontal_tiles = 64;
	const int vertical_tiles = 32;

	bool show_tiles = false;
	bool render_windows = true;

	bool VSync = true;

	bool updating_colors = false;

	//const float scale = 15.34567;

	//int horizontal_scale = scale;
	//int vertical_scale = scale;

	int width = 1280;
	int height = 720;

	int horizontal_scale = width / horizontal_tiles;
	int vertical_scale = height / vertical_tiles;

	int tile_width = horizontal_scale;
	int tile_height = vertical_scale;

	//int tile_width = width / horizontal_tiles;
	//int tile_height = height / vertical_tiles;

	//const int horizontal_scale = width / horizontal_tiles;
	//const int vertical_scale = height / vertical_tiles;

	//const int width = horizontal_tiles * horizontal_scale;
	//const int height = vertical_tiles * vertical_scale;


	const char* title = "CHIP-8";

	ImGuiIO* io;

	SDL_Window* window;
	SDL_GLContext gl_context;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event event = SDL_Event();

	SDL_AudioSpec target_spec;
	SDL_AudioSpec current_spec;
	SDL_AudioDeviceID deviceId;
	int current_sample_number = 0;

	GLenum error = GL_NO_ERROR;

	unsigned int fbo;
	unsigned int fbo2;
	unsigned int textureFbo;
	unsigned int rbo;

	int sample_rate = 44100;
	int amplitude = 28000;
	int samples = 2048;
	double sample_frequency = 44100;
	double sine_frequency = 440;
	double samples_per_sine = sample_frequency / sine_frequency;
	int current_sample = 0;

	char glsl_ver[32] = "";
	

	void allocate_buffer() {
		//pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));
		//pixels = (uint32_t*) calloc(width * height, sizeof(uint32_t));
		pixels = (uint32_t*) malloc(width * height * sizeof(uint32_t));
		copy_buffer = (uint32_t*) malloc(width * height * sizeof(uint32_t));
		original_pixels = (uint32_t*)malloc(horizontal_tiles * vertical_tiles * sizeof(uint32_t));


		int total_tiles = horizontal_tiles * vertical_tiles;
		for (int i = 0; i < width * height; i++) {
			pixels[i] = background;
			copy_buffer[i] = pixels[i];

			if (i < total_tiles) original_pixels[i] = background;
		}

		//memset(pixels, background, width * height * sizeof(uint32_t));
		//memset(copy_buffer, pixels[0], width * height * sizeof(uint32_t));

		/*
		for (int i = 0; i < (width * height); i++) {
			if (i < floor(width * height / 2)) {
				pixels[i] = 0xFFFFFFFF;
			}
			else {
				pixels[i] = 0x00000000;
			}

		}
		*/
	}

	void free_buffer() {
		free(copy_buffer);
		free(pixels);
		free(original_pixels);
	}

	void audio_callback(void* data, Uint8* buffer, int length)
	{

		for (int i = 0; i < length; i++)
		{
			buffer[i] = (std::sin(current_sample / samples_per_sine * M_PI * 2) + 1) * 127.5;
			current_sample++;
		}
	}

	bool initialize() {
		//assert(width % horizontal_tiles == 0);
		//assert(height % vertical_tiles == 0);

		if (SDL_Init(SDL_INIT_VIDEO) != -1) {
			std::cout << "Successfully initialized SDL video." << std::endl;
		}
		else {
			std::cout << "Could not initialize SDL video. Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return 0;
		}

		if (SDL_Init(SDL_INIT_AUDIO) != -1) {
			if (SDL_GetNumAudioDevices(false) > 0) {
				SDL_zero(target_spec);
				target_spec.freq = sample_frequency;
				target_spec.format = AUDIO_U8;
				target_spec.channels = 1;
				target_spec.samples = samples;
				target_spec.callback = audio_callback;
				//target_spec.userdata = this;

				// Only works when last argument (int allowed_changes) is 0
				deviceId = SDL_OpenAudioDevice(NULL, false, &target_spec, &current_spec, 0);

				if (deviceId == 0) {
					std::cout << "Could not initialize audio device. Error: " << SDL_GetError() << std::endl;
				}
				else {
					std::cout << "Successfully initialized audio device." << std::endl;
				}
			}
			else {
				std::cout << "Could not find suitable audio device." << std::endl;
			}

			/*
			target_spec.freq = sample_rate;
			target_spec.format = AUDIO_F32LSB;
			target_spec.channels = 1;
			target_spec.samples = 2048;
			target_spec.callback = audio_callback;
			target_spec.userdata = &current_sample_number;

			if (SDL_GetNumAudioDevices(false) > 0) {
				deviceId = SDL_OpenAudioDevice(NULL, false, &target_spec, &current_spec, 0);

				if (deviceId == 0) {
					std::cout << "Could not initialize audio device. Error: " << SDL_GetError() << std::endl;
					SDL_Quit();
					return 0;
				}

				SDL_PauseAudioDevice(deviceId, false);
				std::cout << "Successfully initialized audio device." << std::endl;
			}
			*/
			/*
			target_spec.freq = 64 * 60; // Sample rate
			//target_spec.format = AUDIO_U8;
			target_spec.format = AUDIO_F32LSB;
			//target_spec.format = AUDIO_S16SYS;
			target_spec.channels = 1;
			target_spec.samples = 64; // Not sample rate
			//target_spec.callback = audio_callback;
			//target_spec.userdata = &current_sample_number;

			

			if (SDL_GetNumAudioDevices(false) > 0) {
				deviceId = SDL_OpenAudioDevice(NULL, false, &target_spec, &current_spec, SDL_AUDIO_ALLOW_ANY_CHANGE);


				if (deviceId == 0) {
					std::cout << "Could not initialize audio device. Error: " << SDL_GetError() << std::endl;
					SDL_Quit();
					return 0;
				}

				SDL_PauseAudioDevice(deviceId, false);
				std::cout << "Successfully initialized audio device." << std::endl;
			}
			else {
				std::cout << "Could not find suitable audio device." << std::endl;
				SDL_Quit();
				return 0;
			}

			///*
			if (target_spec.format != current_spec.format) {
				std::cout << "Invalid target audio format. Error: " << SDL_GetError() << std::endl;
				std::cout << "Spec format: " << current_spec.format;
				SDL_Quit();
				return 0;
			}
			//

			*/

			std::cout << "Successfully initialized SDL audio." << std::endl;
		}
		else {
			std::cout << "Could not initialize SDL audio. Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return 0;
		}

		SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI );
		//window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);


		if (window != nullptr) {
			std::cout << "Successfully initialized SDL window." << std::endl;
		}
		else {
			SDL_Quit();
			std::cout << "Could not initialize SDL window. Error: " << SDL_GetError() << std::endl;
			return 0;
		}

		///*
		//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
		//SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

		/*
		SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE
		);
		*/

		const char* glsl_version = "";
#ifdef __APPLE__
		// GL 3.0 Core + GLSL 130
		glsl_version = "#version 150\n";
		SDL_GL_SetAttribute( // required on Mac OS
			SDL_GL_CONTEXT_FLAGS,
			SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
		);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif __linux__
		// GL 3.0 + GLSL 130
		glsl_version = "#version 130\n";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif _WIN32
		// GL 3.0 + GLSL 130
		glsl_version = "#version 130\n";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
		glsl_version = "#version 130\n";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

		strcpy(Graphics::glsl_ver, glsl_version);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


		gl_context = SDL_GL_CreateContext(window);

		if (gl_context != NULL) {
			std::cout << "Successfully initialized SDL OpenGL context." << std::endl;
		}
		else {
			SDL_DestroyWindow(window);
			SDL_Quit();
			std::cout << "Could not initialize SDL OpenGL context. Error: " << SDL_GetError() << std::endl;
			return 0;
		}

		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			std::cerr << "Could not initialize GLAD. Error: " << SDL_GetError() << std::endl;
			return 0;
		}
		else
		{
			std::cout << "Successfully initialized GLAD." << std::endl;
		}

		/*

		glViewport(0, 0, width, height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glOrtho(0, width, height, 0, -1.0, 1.0);
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glShadeModel(GL_FLAT);

		*/

		/*
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DITHER);
		glDisable(GL_BLEND);
		*/

		//SDL_GL_SetSwapInterval(0);

		// VSync is enabled by default.

		//SDL_GL_SetSwapInterval(0);


		//SDL_GL_SetSwapInterval(0);
		/*
		if (SDL_GL_SetSwapInterval(1) < 0) {
			std::cout << "Could not set VSync. Error: " << SDL_GetError() << std::endl;
		} else {
			std::cout << "Successfully enabled VSync." << std::endl;
		}
		*/



		//SDL_GL_MakeCurrent(window, gl_context);

		/*
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, (GLvoid*)pixels);
		*/

		//glViewport(0, 0, width, height);
		// colors are set in RGBA, but as float
		ImVec4 background = ImVec4(140.f / 255, 27 / 255, 36 / 255, 0.3f);
		glClearColor(background.x, background.y, background.z, background.w);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		io = &ImGui::GetIO(); (void)io;

		ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
		ImGui_ImplOpenGL3_Init(glsl_version);

		///*

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);

		if (renderer != nullptr) {
			std::cout << "Successfully initialized SDL renderer." << std::endl;
		}
		else {
			SDL_DestroyWindow(window);
			SDL_Quit();
			std::cout << "Could not initialize SDL renderer. Error: " << SDL_GetError() << std::endl;
			return 0;
		}

		uint8_t r = 33;
		uint8_t g = 35;
		uint8_t b = 144;
		uint8_t a = 255;
		SDL_SetRenderDrawColor(Graphics::renderer, r, g, b, a);

		//texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, horizontal_tiles, vertical_tiles);
		//SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		
		//texture = SDL_CreateTexture(renderer, SDL_PixelFormatEnum::)

		if (texture != nullptr) {
			std::cout << "Successfully initialized SDL texture." << std::endl;
		}
		else {
			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			std::cout << "Could not initialize SDL texture. Error: " << SDL_GetError() << std::endl;
			return 0;
		}

		glGenFramebuffers(1, &fbo);
		glGenFramebuffers(1, &fbo2);
		glGenRenderbuffers(1, &rbo);
		glGenTextures(1, &textureFbo);

		//*/
		allocate_buffer();

		std::cout << "Successfully initialized graphics module." << std::endl;
		return 1;
	}

	bool close() {
		try {
			SDL_CloseAudioDevice(deviceId);
			glDeleteRenderbuffers(1, &rbo);
			glDeleteTextures(1, &textureFbo);
			glDeleteFramebuffers(1, &fbo);
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();
			SDL_GL_DeleteContext(gl_context);
			SDL_CloseAudio();
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
			free_buffer();
			std::cout << "Successfully closed SDL." << std::endl;
			std::cout << "Successfully denitialized graphics module." << std::endl;
			return 1;
		}
		catch (std::exception &e) {
			std::cout << "Could not close SDL. Error: " << e.what() << " | SDL: " << SDL_GetError() << std::endl;
			return 0;
		}
	}

	void clear_window() {
		//for (int i = 0; i < width * height; i++) {
		for (int i = 0; i < horizontal_tiles * vertical_tiles; i++) {
			//copy_buffer[i] = pixels[i] = background;

			if (i < vertical_tiles * horizontal_tiles) original_pixels[i] = background;
		}

		//memset(pixels, background, width * height * sizeof(uint32_t));
		//glClear(GL_COLOR_BUFFER_BIT);
		//memset(pixels, 0, width * height * sizeof(uint32_t));
		/*
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		*/
	}

	void update_renderer() {
		SDL_UpdateTexture(texture, nullptr, pixels, width * sizeof(uint32_t));
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		//SDL_RenderPresent(renderer);
		//SDL_RenderFlush(renderer);

		/*

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (pixels[y * width + x] != 0) {
					printf("1");
				}
				else {
					printf("0");
				}
			}

			printf("\n");
		}

		printf("\n");

		*/
	}
	
	float convert_x(int x) {
		return (2.f / width) * x - 1;
	}

	float convert_y(int y) {
		return (-2.f / height) * y + 1;
	}

	bool is_wrong(float num) {
		return (num <= -1 || num >= 1);
	}

	void draw_new(int x, int y) {
		float left_top_x = convert_x(x);
		float left_top_y = convert_y(y);

		float right_top_x = convert_x(x + tile_width);
		float right_top_y = left_top_y;

		float right_bottom_x = right_top_x;
		float right_bottom_y = convert_y(y + tile_height);

		float left_bottom_x = left_top_x;
		float left_bottom_y = right_bottom_y;

		/*

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		*/



		//glClear(GL_COLOR_BUFFER_BIT);

		//std::this_thread::sleep_for(std::chrono::seconds(5));


		printf("(%.4f, %.4f)				(%.4f, %.4f)\n\n\n", left_top_x, left_top_y, right_top_x, right_top_y);
		printf("(%.4f, %.4f)				(%.4f, %.4f)\n\n", left_bottom_x, left_bottom_y, right_bottom_x, right_bottom_y);



		/*
		if (is_wrong(left_top_x) || is_wrong(left_top_y) || is_wrong(right_top_x) || is_wrong(right_top_y) || is_wrong(right_bottom_x) || is_wrong(right_bottom_y) || is_wrong(left_bottom_x) || is_wrong(left_bottom_y)) {
			exit(-1);
		}

		*/

		//glClearColor(1.f, 1.f, 0.f, 1.f);
		//glClear(GL_COLOR_BUFFER_BIT);

		/*

		glBegin(GL_QUADS);
		glVertex2f(left_top_x, left_top_y);
		glVertex2f(right_top_x, right_top_y);
		glVertex2f(right_bottom_x, right_bottom_y);
		glVertex2f(left_bottom_x, left_bottom_y);
		glEnd();


		*/

		/*

		std::this_thread::sleep_for(std::chrono::seconds(15));

		glBegin(GL_QUADS);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.5f, -0.5f);
		glVertex2f(0.5f, 0.5f);
		glVertex2f(-0.5f, 0.5f);
		glEnd();

		*/

		/*

		for (int y = 0; y <= height - tile_height; y += tile_height) {
			for (int x = 0; x <= width - tile_width; x += tile_width) {
				int left_top_x = x;
				int left_top_y = y;

				int right_top_x = left_top_x + tile_width;
				int right_top_y = left_top_y;

				int right_bottom_x = right_top_x;
				int right_bottom_y = right_top_y + tile_height;

				int left_bottom_x = left_top_x;
				int left_bottom_y = left_top_y + tile_height;

				glBegin(GL_QUADS);
				glVertex2f(convert_x(left_top_x), convert_y(left_top_y));
				glVertex2f(convert_x(right_top_x), convert_y(right_top_y));
				glVertex2f(convert_x(right_bottom_x), convert_y(right_bottom_y));
				glVertex2f(convert_x(left_top_x), convert_y(left_bottom_y));
				glEnd();
			}
		}d

		*/
	}

	void update() {
		

		//update_renderer();
		
		//glLoadIdentity();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glLoadIdentity();
		//glRasterPos2i(-1, 1);
		//glPixelZoom(1, -1);

		//glFlush();
		//draw_new(0, 0);
		/*
		std::this_thread::sleep_for(std::chrono::seconds(5));
		//SDL_GL_SwapWindow(window);
		std::this_thread::sleep_for(std::::seconds(5));
		*/
	}


	void fill_tile(int x, int y, uint32_t color) {
		for (int newY = y; newY - y < tile_height; newY++) {
			for (int newX = x; newX - x < tile_width; newX++) {
				if (newY <= Graphics::height && newY >= 0 && newX <= Graphics::width && newX >= 0)	pixels[newY * width + newX] = color;
			}
		}

		//draw_new(x, y);
	}

	void fill_tile(uint32_t* buffer, int width, int height, int x, int y, int tile_width, int tile_height, uint32_t color) {
		for (int newY = y; newY - y < tile_height; newY++) {
			for (int newX = x; newX - x < tile_width; newX++) {
				if (newY <= height && newY >= 0 && newX <= width && newX >= 0) buffer[newY * width + newX] = color;
			}
		}

		//draw_new(x, y);
	}

	void draw_sprite(uint8_t x, uint8_t y, uint8_t sprite_height, uint8_t* index_register, uint8_t* carry_flag) {
		// N pixels tall sprite (8xN)
		// From the location that index register points to
		// At horizontal coordinate VX
		// At vertical coordinate VY
		// All bits in the sprite that are 1 should flip (not operator) the pixels they correspond to
		// If any of the pixels get turned off (0/black), VF (carry_flag) gets set to 1. Otherwise, it gets set to 0.
		//x *= scale;
		//y *= scale;

		//x = (x > width) || (x < 0) ? x % width : x;
		//y = (y > height) || (y < 0) ? y % height : y;

		x = (x > horizontal_tiles) || (x < 0) ? x % horizontal_tiles : x;
		y = (y > vertical_tiles) || (y < 0) ? y % vertical_tiles : y;

		//printf("X: %d | Y: %d | H: %d\n", x, y, sprite_height);

		bool turned_off = false;

		if (sprite_height > 0) {
			uint8_t* bytes= (uint8_t*) malloc(sprite_height);
			memcpy(bytes, index_register, sprite_height);

			//printf("X: %d | Y: %d\n", x, y);

			/*
			for (int i = 0; i < width * height; i++) {
				printf("%d: %d\n", i, pixels[i]);
			}

			printf("\n\n\n");
			*/

			for (uint8_t byte = 0; byte < sprite_height; byte++) {
				uint8_t final_pixel_Y_coordinate;
				if (y + byte >= Graphics::vertical_tiles) {
					final_pixel_Y_coordinate = (y + byte) % Graphics::vertical_tiles;
				}
				else {
					final_pixel_Y_coordinate = y + byte;
				}
				

				for (uint8_t bit = 0; bit < 8; bit++) {
					bool current_bit = (bytes[byte] >> (7 - bit)) & 1;
					uint8_t final_pixel_X_coordinate;
					if (x + bit >= Graphics::horizontal_tiles) {
						final_pixel_X_coordinate = (x + bit) % Graphics::horizontal_tiles;
					}
					else {
						final_pixel_X_coordinate = x + bit;
					}


					//printf("final_X: %d | final_Y: %d\n", final_pixel_X_coordinate, final_pixel_Y_coordinate);
					//printf("final_X * vs: %d | final_Y * hz: %d\n", final_pixel_X_coordinate * horizontal_scale, final_pixel_Y_coordinate * vertical_scale);

					/*

					if (pixels[final_pixel_Y_coordinate * width + final_pixel_X_coordinate] == 0xFFFFFFFF) {
						pixels[final_pixel_Y_coordinate * width + final_pixel_X_coordinate] = 0;
						turned_off = true;
					}
					else if (pixels[final_pixel_Y_coordinate * width + final_pixel_X_coordinate] == 0x00000000) {
						pixels[final_pixel_Y_coordinate * width + final_pixel_X_coordinate] = 0xFFFFFFFF;
					}
					else {
						throw;
					}

					*/

					if (current_bit && original_pixels[final_pixel_Y_coordinate * horizontal_tiles + final_pixel_X_coordinate] == background) {
						//fill_tile(final_pixel_X_coordinate * horizontal_scale, final_pixel_Y_coordinate * vertical_scale, foreground);
						original_pixels[final_pixel_Y_coordinate * horizontal_tiles + final_pixel_X_coordinate] = foreground;
					}
					else if (current_bit && original_pixels[final_pixel_Y_coordinate * horizontal_tiles + final_pixel_X_coordinate] == foreground) {
						//fill_tile(final_pixel_X_coordinate * horizontal_scale, final_pixel_Y_coordinate * vertical_scale, background);
						original_pixels[final_pixel_Y_coordinate * horizontal_tiles + final_pixel_X_coordinate] = background;
						turned_off = true;
					}
					
					/*
					if (current_bit && pixels[final_pixel_Y_coordinate * vertical_scale * width + final_pixel_X_coordinate * horizontal_scale] == background) {
						fill_tile(final_pixel_X_coordinate * horizontal_scale, final_pixel_Y_coordinate * vertical_scale, foreground);
					}
					else if (current_bit && pixels[final_pixel_Y_coordinate * vertical_scale * width + final_pixel_X_coordinate * horizontal_scale] == foreground) {
						fill_tile(final_pixel_X_coordinate * horizontal_scale, final_pixel_Y_coordinate * vertical_scale, background);
						turned_off = true;						
					}
					*/
					
					//printf("%d", current_bit);
				}
				
				//printf("\n");
			}

			if (turned_off) {
				*carry_flag = 1;
			}
			else {
				*carry_flag = 0;
			}


			
			/*
			for (int i = 0; i < width * height; i++) {
				printf("%d: %d\n", i, pixels[i]);
			}

			*/

			/*

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					if (pixels[y * width + x] != 0) {
						printf("1");
					}
					else {
						printf("0");
					}
				}

				printf("\n");
			}

			printf("\n");

			*/
			

			free(bytes);
		}
	}

	void draw_top(int x, int y) {
		if (x < 0 || x >= Graphics::width || y < 0 || y >= Graphics::height) return;

		for (int newX = x; (newX - x) < tile_width; newX++) {
			pixels[y * width + newX] = Colors::Tile;
		}
	}

	void draw_left(int x, int y) {
		if (x < 0 || x >= Graphics::width || y < 0 || y >= Graphics::height) return;

		for (int newY = y; (newY - y) < tile_height; newY++) {
			pixels[newY * width + x] = Colors::Tile;
		}
	}

	void draw_bottom(int x, int y) {
		int newY = y + (tile_height - 1);
		if (x < 0 || x >= Graphics::width || newY < 0 || newY >= Graphics::height) return;
		
		for (int newX = x; (newX - x) < tile_width; newX++) {
			pixels[newY * width + newX] = Colors::Tile;
		}
	}

	void draw_right(int x, int y) {
		int newX = x + (tile_width - 1);
		if (newX < 0 || newX >= Graphics::width || y < 0 || y >= Graphics::height) return;

		for (int newY = y; (newY - y) < tile_height; newY++) {
			pixels[newY * width + newX] = Colors::Tile;
		}
	}

	void draw_tiles() {
		memcpy(copy_buffer, pixels, width * height * sizeof(uint32_t));

		for (int y = 0; y < height; y += tile_height) {
			for (int x = 0; x < width; x += tile_height) {
				draw_top(x, y);
				draw_left(x, y);
				draw_bottom(x, y);
				draw_right(x, y);
			}
		}
	}

	/*
	static SDL_Event* get_event() {
		return &this->event;
	}
	*/


	uint8_t scancode_to_hexa(SDL_Scancode code) {
		switch (code) {
		case SDL_SCANCODE_1:
			return 0x1;
		case SDL_SCANCODE_2:
			return 0x2;
		case SDL_SCANCODE_3:
			return 0x3;
		case SDL_SCANCODE_4:
			return 0xC;
		case SDL_SCANCODE_Q:
			return 0x4;
		case SDL_SCANCODE_W:
			return 0x5;
		case SDL_SCANCODE_E:
			return 0x6;
		case SDL_SCANCODE_R:
			return 0xD;
		case SDL_SCANCODE_A:
			return 0x7;
		case SDL_SCANCODE_S:
			return 0x8;
		case SDL_SCANCODE_D:
			return 0x9;
		case SDL_SCANCODE_F:
			return 0xE;
		case SDL_SCANCODE_Z:
			return 0xA;
		case SDL_SCANCODE_X:
			return 0x0;
		case SDL_SCANCODE_C:
			return 0xB;
		case SDL_SCANCODE_V:
			return 0xF;
		default:
			//std::cout << "Unknown scancode: scancode_to_hexa" << std::endl;
			return 99;
		}
	}

	SDL_Scancode hexa_to_scancode(uint8_t key) {
		switch (key) {
		case 0x1:
			return SDL_SCANCODE_1;
		case 0x2:
			return SDL_SCANCODE_2;
		case 0x3:
			return SDL_SCANCODE_3;
		case 0xC:
			return SDL_SCANCODE_4;
		case 0x4:
			return SDL_SCANCODE_Q;
		case 0x5:
			return SDL_SCANCODE_W;
		case 0x6:
			return SDL_SCANCODE_E;
		case 0xD:
			return SDL_SCANCODE_R;
		case 0x7:
			return SDL_SCANCODE_A;
		case 0x8:
			return SDL_SCANCODE_S;
		case 0x9:
			return SDL_SCANCODE_D;
		case 0xE:
			return SDL_SCANCODE_F;
		case 0xA:
			return SDL_SCANCODE_Z;
		case 0x0:
			return SDL_SCANCODE_X;
		case 0xB:
			return SDL_SCANCODE_C;
		case 0xF:
			return SDL_SCANCODE_V;
		default:
			//std::cout << "Unknown scancode: hexa_to_scancode" << std::endl;
			return (SDL_Scancode) 99;
		}
	}

	void handle_key(uint8_t key) {
		//send_key_press_to_stack(key);
	}

	void print() {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				std::cout << "(" << x << ", " << y << "): " << pixels[y * width + x] << std::endl;
			}
		}
	}
};

