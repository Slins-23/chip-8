#include <ostream>
#include <thread>
#include <chrono>
#include "RAM.h"
#include "Graphics.h"
#include <cstring>
#include <sstream>
#include <charconv>
#include <stdio.h>

std::string hex_to_decstr(std::string hex_val) {
	std::stringstream ss;
	ss << std::hex << hex_val;

	return ss.str();
}

#pragma once

struct Registers {
	uint8_t V0 = NULL;
	uint8_t V1 = NULL;
	uint8_t V2 = NULL;
	uint8_t V3 = NULL;
	uint8_t V4 = NULL;
	uint8_t V5 = NULL;
	uint8_t V6 = NULL;
	uint8_t V7 = NULL;
	uint8_t V8 = NULL;
	uint8_t V9 = NULL;
	uint8_t VA = NULL;
	uint8_t VB = NULL;
	uint8_t VC = NULL;
	uint8_t VD = NULL;
	uint8_t VE = NULL;
	uint8_t VF = NULL; // Flag register
	uint16_t I = NULL; // Index register
};

struct Keys {
	uint8_t K1 = 0x1; // 1
	uint8_t K2 = 0x2; // 2
	uint8_t K3 = 0x3; // 3
	uint8_t K4 = 0xC; // 4
	uint8_t K5 = 0x4; // Q
	uint8_t K6 = 0x5; // W
	uint8_t K7 = 0x6; // E
	uint8_t K8 = 0xD; // R
	uint8_t K9 = 0x7; // A
	uint8_t K10 = 0x8; // S
	uint8_t K11 = 0x9; // D
	uint8_t K12 = 0xE; // F
	uint8_t K13 = 0xA; // Z
	uint8_t K14 = 0x0; // X
	uint8_t K15 = 0xB; // C
	uint8_t K16 = 0xF; // V
};

enum Status {
	stopped,
	running,
	paused
};

enum Implementation {
	Optimal,
	Cosmac_VIP,
	Chip_48,
	Super_Chip,
	Amiga,
};

#pragma once
namespace CPU
{
	uint8_t* pc = nullptr;
	uint8_t delay = 0x00;
	uint8_t sound = 0x00;

	uint16_t keys = 0;
	uint8_t pressed_key = 0;
	bool is_held = false;
	bool halt = false;
	bool switched_on = false;

	uint16_t current_instruction = 0;

	std::thread delay_N_sound_thread;
	std::thread cpu_thread;

	bool closed = false;
	int iteration = 0;

	uint32_t frameStart = 0;
	int frametime = 0;

	int instructionsPerSecond = 700;
	int millisecondsPerInstruction = 1000 / instructionsPerSecond;
	uint8_t* register_pointer = (uint8_t*) new Registers;

	bool stop_cpu = false, stop_delay = false, stop_sound = false;
	bool playing_audio = false;

	int delayDecPerSec = 60;
	int soundDecPerSec = 60;

	int instructions_per_delay_decrement = round((double)instructionsPerSecond / delayDecPerSec);
	int instructions_per_sound_decrement = round((double)instructionsPerSecond / soundDecPerSec);

	uint8_t status = Status::stopped;

	int stepped_delay = 0;
	int stepped_sound = 0;

	bool delayIsZero, soundIsZero = false;

	int instructions_per_frame = instructionsPerSecond / Graphics::FPS;

	float delay_decrement_per_frame = (double)delayDecPerSec / Graphics::FPS;
	float sound_decrement_per_frame = (double)soundDecPerSec / Graphics::FPS;

	float current_framerate = 0;

	std::thread sound_thread;
	bool closing_sound_thread = false;

	// Optimal, Cosmac VIP, CHIP-48 or Super-Chip
	Implementation implementation = Implementation::Optimal;

	void decrement_delay(int times) {
		for (int i = 0; i < times; i++) {
			if (delay > 0) delay--;
		}
	}

	void decrement_sound(int times) {
		for (int i = 0; i < times; i++) {
			if (sound > 0) sound--;
		}
	}

	void update_delay_N_sound_manual() {
		if (delay > 0) delay--;
		if (sound > 0) sound--;
	}

	void update_delay_N_sound() {
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 / 60)));

		update_delay_N_sound_manual();
	}

	void step_one() {
		uint16_t instruction = (*pc << 8) | *(pc + 1);
		current_instruction = instruction;

		pc += 2;
		uint16_t next_instruction = (*pc << 8) | *(pc + 1);

		uint8_t first_nibble = instruction >> 12; // Tells which type of instruction
		uint8_t X = (instruction >> 8) & 0xF; // Looks up one of the 16 VX registers from V0 through VF;
		uint8_t Y = (instruction >> 4) & 0xF; // Looks up one of the 16 VY registers from V0 through VF;
		uint8_t N = instruction & 0xF; // Fourth nibble
		uint8_t NN = instruction & 0xFF; // Second byte (8-bit immediate number)
		uint16_t NNN = instruction & 0xFFF; // second, third, fourth nibbles (12-bit immediate memory address)

		uint8_t* vx = (uint8_t*)(register_pointer + X);
		uint8_t* vy = (uint8_t*)(register_pointer + Y);
		uint8_t* carry_flag = (uint8_t*)(register_pointer + 15);
		uint16_t* index_register = (uint16_t*)(register_pointer + 16);

		switch (first_nibble) {
		case (0x0):
			switch (NNN) {
			case (0x0E0):
				Graphics::clear_window();
				break;
			case (0x0EE):
				RAM::sp--;
				pc = RAM::buffer + *RAM::sp;
				*RAM::sp = 0;

				break;
			default:
				break;
			}
			break;
		case (0x1):
			pc = RAM::buffer + NNN;
			break;
		case (0x2):
			*RAM::sp = pc - RAM::buffer;
			RAM::sp++;
			pc = RAM::buffer + NNN;
			break;
		case (0x3):
			if (*vx == NN) {
				// Skip
				pc += 2;
			}
			else {
				// Don't skip
			}
			break;
		case (0x4):
			if (*vx != NN) {
				// Skip
				pc += 2;
			}
			else {
				// Don't skip
			}
			break;
		case (0x5):
			if (*vx == *vy) {
				// Skip
				pc += 2;
			}
			else {
				// Don't skip
			}
			break;
		case (0x6):
			*vx = NN;
			break;
		case (0x7):
			*vx += NN;
			/* C++ WAY

			if (*vx + NN <= 255) { // Add only if not already max size, otherwise reset register to 0
				*vx += NN;
			}
			else {
				*vx = (*vx + NN) - 255;
			}

			*/
			break;
		case (0x8):
			switch (N) {
			case 0x0:
				*vx = *vy;
				break;
			case 0x1:
				*vx |= *vy;
				break;
			case 0x2:
				*vx &= *vy;
				break;
			case 0x3:
				*vx ^= *vy;
				break;
			case 0x4:
				if (*vx + *vy > 255) { // If sum needs carry
					*carry_flag = 0x1; // Carry flag
				}
				else {
					*carry_flag = 0x0;
				}

				*vx += *vy;
				break;
			case 0x5:
				if (*vx > *vy) {
					*carry_flag = 0x1; // Carry flag
				}
				else {
					*carry_flag = 0x0;
				}

				*vx -= *vy;
				break;
			case 0x6:
				if (implementation == Implementation::Cosmac_VIP) {
					*vx = *vy;
					*carry_flag = *vx & 0x1;
					*vx >>= 1;
				}
				else if (implementation == Implementation::Optimal || implementation == Implementation::Chip_48 || implementation == Implementation::Super_Chip) {
					*carry_flag = *vx & 0x1;
					*vx >>= 1;
				}

				break;
			case 0x7:
				if (*vy > *vx) {
					*carry_flag = 0x1; // Carry flag
				}
				else {
					*carry_flag = 0x0;
				}

				*vx = *vy - *vx;
				break;
			case 0xE:
				if (implementation == Implementation::Cosmac_VIP) {
					*vx = *vy;
					*carry_flag = (*vx >> 7) & 0x1;
					*vx <<= 1;
				}
				else if (implementation == Implementation::Optimal || implementation == Implementation::Chip_48 || implementation == Implementation::Super_Chip) {
					*carry_flag = (*vx >> 7) & 0x1;
					*vx <<= 1;
				}
				break;
			}
			break;

		case (0x9):
			if (*vx != *vy) {
				// Skip
				pc += 2;
			}
			else {
				// Don't skip
			}
			break;
		case (0xA):
			*index_register = NNN;
			break;
		case (0xB):
			if (implementation == Implementation::Optimal || implementation == Implementation::Cosmac_VIP) {
				pc = RAM::buffer + (NNN + *register_pointer);
			}
			else if (implementation == Implementation::Chip_48 || implementation == Implementation::Super_Chip) {
				pc = RAM::buffer + (NNN + *vx);
			}
			
			break;
		case (0xC):
			*vx = (rand() % 256) & NN;
			//*vx = (rand() % NN) & NN;
			break;
		case (0xD):
			// N pixels tall sprite (8xN)
			// From the location that index register points to
			// At horizontal coordinate VX
			// At vertical coordinate VY
			// All bits in the sprite that are 1 should flip (not operator) the pixels they correspond to
			// If any of the pixels get turned off (0/black), VF (carry_flag) gets set to 1. Otherwise, it gets set to 0.

			Graphics::draw_sprite(*vx, *vy, N, RAM::buffer + *index_register, carry_flag); // N is up to 16
			break;
		case (0xE):
			switch (NN) {
			case (0x9E): {
				// Skip if key in register VX is pressed
				if ((keys >> *vx) & 1) {
					pc += 2;
				}
				else {
				}
			}
					   break;
			case (0xA1): {
				// Skip if key in register VX is not pressed
				if (!((keys >> *vx) & 1)) {
					pc += 2;
				}
				else {
				}
			}
				break;
			default:
				break;
			}
			break;
		case (0xF):
			switch (NN) {
			case (0x07):
				*vx = delay;
				break;
			case (0x15):
				delay = *vx;
				break;
			case (0x18):
				sound = *vx;
				break;
			case (0x1E):
				if (implementation == Implementation::Optimal || implementation == Implementation::Amiga) {
					if (*index_register + *vx > 4095) {
						*carry_flag = 0x1;
					}
					else {
						*carry_flag = 0x0;
					}

					*index_register += *vx;
				}
				else if (implementation == Implementation::Cosmac_VIP) {
					*index_register += *vx;
				}

				break;
			case (0x0A): {
				if (implementation == Implementation::Optimal) {
					// Non Cosmac Vip
					// Loops until any key is pressed
					if (!halt && !switched_on) {
						halt = true;
						switched_on = true;
					}

					if (!halt) {
						*vx = pressed_key;
						switched_on = false;
					}
					else {
						pc -= 2;
					}

					/* Cosmac Vip
					* if (key_pressed_and_released) {
					*	 *(register_pointer + X) = key_hexadecimal_identifier;
					* }
					*/
				}
				/*
				else if (implementation == Implementation::Cosmac_VIP) {
					if (!halt && !switched_on) {
						halt = true;
						switched_on = true;
					}

					if (!halt) {
						*vx = pressed_key;
						switched_on = false;
					}
					else {
						pc -= 2;
					}
				}
				*/
				
			}
					   break;
			case (0x29):
			{
				uint8_t character = (*vx) & 0x0F;
				*index_register = character * 5;
			}
			break;
			case (0x33):
			{
				if (*vx > 99) {
					uint8_t onep = *vx % 10;
					uint8_t tenp = ((*vx % 100) - onep) / 10;
					uint8_t hunp = ((*vx % 1000) - (tenp * 10) - onep) / 100;

					*(RAM::buffer + *index_register) = hunp;
					*(RAM::buffer + *index_register + 1) = tenp;
					*(RAM::buffer + *index_register + 2) = onep;
				}
				else if (*vx > 9) {
					uint8_t onep = *vx % 10;
					uint8_t tenp = ((*vx % 100) - onep) / 10;

					*(RAM::buffer + *index_register) = 0;
					*(RAM::buffer + *index_register + 1) = tenp;
					*(RAM::buffer + *index_register + 2) = onep;
				}
				else {
					*(RAM::buffer + *index_register) = 0;
					*(RAM::buffer + *index_register + 1) = 0;
					*(RAM::buffer + *index_register + 2) = *vx;
				}
			}
			break;
			case (0x55):
				if (implementation == Implementation::Optimal || implementation == Implementation::Chip_48 || implementation == Implementation::Super_Chip) {
					memcpy(RAM::buffer + *index_register, register_pointer, X + 1);
				}
				else if (implementation == Implementation::Cosmac_VIP) {
					memcpy(RAM::buffer + *index_register, register_pointer, X + 1);
					*index_register += X + 1;
				}
				

				/*
				// Modern, CHIP48 & Super-Chip
				for (int i = 0; i < X; i++) {
					memcpy(index_register + i, register_pointer + i, 1);
				}
				*/
				break;
			case (0x65):

				if (implementation == Implementation::Optimal || implementation == Implementation::Chip_48 || implementation == Implementation::Super_Chip) {
					memcpy(register_pointer, RAM::buffer + *index_register, X + 1);
				}
				else if (implementation == Implementation::Cosmac_VIP) {
					memcpy(register_pointer, RAM::buffer + *index_register, X + 1);
					*index_register += X + 1;
				}

				

				/*
				// Modern, CHIP48 & Super-Chip
				for (int i = 0; i < X; i++) {
					*(register_pointer + i) = *(index_register + i);
				}
				*/

				break;
			default:
				break;
			}
			break;
		default:
			break;
		}

		stepped_delay++;
		stepped_sound++;

		if (CPU::status == Status::paused) { // If stepping manually
			if ((stepped_delay % instructions_per_delay_decrement) == 0) // Enough instructions have ran that we should decrement timers
			{
				stepped_delay = 0;
				decrement_delay(1);
			}

			if ((stepped_sound % instructions_per_sound_decrement) == 0) // Enough instructions have ran that we should decrement timers
			{
				stepped_sound = 0;
				decrement_sound(1);
			}
		}
	}

	void step_many(int steps) {
		for (int i = 0; i < steps; i++) {
			step_one();
		}
	}

	void skip_one() {
		pc += 2;
	}

	void skip_many(int skips) {
		for (int i = 0; i < skips; i++) {
			skip_one();
		}
	}

	void fetch_decode_exec() {
		while (!stop_cpu) {
			while (Graphics::updating_colors) {

			}

			if (status == Status::running) {
				step_one();
			}
		}
	}

	void set_pressed(uint16_t hexc) {
		keys |= (1 << hexc);
	}

	void release_pressed(uint16_t hexc) {
		keys &= (0xFFFF ^ (1 << hexc));
	}

	void HandlePressDown(uint8_t hexc) {
		if (hexc < 16) {
			set_pressed(hexc);
			pressed_key = hexc;
			is_held = true;

			if (halt) {
				halt = false;
			}
		}
	}

	void HandlePressUp(uint8_t hexc) {
		if (hexc < 16) {
			release_pressed(hexc);

			if (hexc == pressed_key && is_held) {
				is_held = false;
			}
		}
	}



	void decToHex(char (&storage)[5], uint16_t dec) {
		int2str(dec, storage, 16, 1);

		/*
		for (int i = 0; i < 4; i++) {
			storage[i] = (char) toupper(storage[i]);
		}
		*/
		/*
		if (dec <= 0x000F) {
			char temp = storage[0];
			storage[0] = storage[1] = storage[2] = '0';
			storage[3] = temp;
		}
		else if (dec <= 0x00FF) {
			char temp1 = storage[0];
			char temp2 = storage[1];
			storage[0] = storage[1] = '0';
			storage[2] = temp1;
			storage[3] = temp2;
		}
		else if (dec <= 0x0FFF ) {
			char temp1 = storage[0];
			char temp2 = storage[1];
			char temp3 = storage[2];
			storage[0] = '0';
			storage[1] = temp1;
			storage[2] = temp2;
			storage[3] = temp3;
		}
		*/

		return;
	}

	// Free after use
	char* decToHex_alternative(uint16_t dec) {
		//char* addr = (char*)malloc(5);
		//_itoa_s(dec, storage, 16);
		char* storage = (char*) malloc(5);
		//int2str(dec, &storage[0], 16, 1);
		int2str(dec, storage, 16, 1);

		/*
		for (int i = 0; i < 4; i++) {
			storage[i] = (char) toupper(storage[i]);
		}
		*/
		/*
		if (dec <= 0x000F) {
			char temp = storage[0];
			storage[0] = storage[1] = storage[2] = '0';
			storage[3] = temp;
		}
		else if (dec <= 0x00FF) {
			char temp1 = storage[0];
			char temp2 = storage[1];
			storage[0] = storage[1] = '0';
			storage[2] = temp1;
			storage[3] = temp2;
		}
		else if (dec <= 0x0FFF ) {
			char temp1 = storage[0];
			char temp2 = storage[1];
			char temp3 = storage[2];
			storage[0] = '0';
			storage[1] = temp1;
			storage[2] = temp2;
			storage[3] = temp3;
		}
		*/

		return storage;
	}

	void NdecToHex(char(&storage)[5], uint16_t dec, uint8_t type) {
		//char* addr = (char*)malloc(5);
		//_itoa_s(dec, storage, type);
		int2str(dec, storage, 16, 1);

		/*
		for (int i = 0; i < 4; i++) {
			storage[i] = (char)toupper(storage[i]);
		}
		*/
		/*
		if (dec <= 0x000F) {
			char temp = storage[0];
			storage[0] = storage[1] = storage[2] = '0';
			storage[3] = temp;
		}
		else if (dec <= 0x00FF) {
			char temp1 = storage[0];
			char temp2 = storage[1];
			storage[0] = storage[1] = '0';
			storage[2] = temp1;
			storage[3] = temp2;
		}
		else if (dec <= 0x0FFF ) {
			char temp1 = storage[0];
			char temp2 = storage[1];
			char temp3 = storage[2];
			storage[0] = '0';
			storage[1] = temp1;
			storage[2] = temp2;
			storage[3] = temp3;
		}
		*/

		return;
	}

	void reset() {
		status = Status::stopped;
		stop_cpu = stop_sound = stop_delay = false;
		closed = false;

		delay = 0x00;
		sound = 0x00;
		keys = 0;
		pressed_key = 0;
		is_held = false;
		halt = false;
		switched_on = false;

		
		iteration = 0;

		frameStart = 0;
		frametime = 0;

		*(register_pointer) = NULL;
		*(register_pointer + 1) = NULL;
		*(register_pointer + 2) = NULL;
		*(register_pointer + 3) = NULL;
		*(register_pointer + 4) = NULL;
		*(register_pointer + 5) = NULL;
		*(register_pointer + 6) = NULL;
		*(register_pointer + 7) = NULL;
		*(register_pointer + 8) = NULL;
		*(register_pointer + 9) = NULL;
		*(register_pointer + 10) = NULL;
		*(register_pointer + 11) = NULL;
		*(register_pointer + 12) = NULL;
		*(register_pointer + 13) = NULL;
		*(register_pointer + 14) = NULL;
		*(register_pointer + 15) = NULL;
		*(register_pointer + 16) = NULL;

		pc = (uint8_t*)(RAM::buffer + RAM::interpreterSpaceInBytes);

		for (int i = 0; i < Graphics::horizontal_tiles * Graphics::vertical_tiles; i++) {
			Graphics::original_pixels[i] = Graphics::background;
		}
		
		// clear stack
		for (int i = 0; i < RAM::stack_size; i++) {
			RAM::sp_addr[i] = 0;
		}
	}

	void start() {
		reset();
		status = Status::running;
	}

	void decrement(int times) {
		delay -= times;
		sound -= times;
	} 

	const char* decToHex_and_pad(uint16_t dec) {
		char storage[5];
		int2str(dec, storage, 16, 1);

		if (storage[0] == '\0') {
			printf("Error: instruction is empty?\n");
			//exit(-1);
			//addr[0] = addr[1] = addr[2] = addr[3] = '0';
			//addr[4] = '\0';
		}
		else if (storage[1] == '\0') {
			//printf("Here\n");
			char temp1 = storage[0];

			storage[0] = storage[1] = storage[2] = '0';
			storage[3] = temp1;
			storage[4] = '\0';
		}
		else if (storage[2] == '\0') {
			char temp1 = storage[0];
			char temp2 = storage[1];

			storage[0] = storage[1] = '0';
			storage[2] = temp1;
			storage[3] = temp2;
			storage[4] = '\0';
		}
		else if (storage[3] == '\0') {
			char temp1 = storage[0];
			char temp2 = storage[1];
			char temp3 = storage[2];

			storage[0] = '0';
			storage[1] = temp1;
			storage[2] = temp2;
			storage[3] = temp3;
			storage[4] = '\0';
		}

		return storage;
	}

	void convert_register(char (&result)[9], uint8_t reg) {
		//char result[9] = "";
		//char* result = (char*) calloc(9, 1);
		char conversion_res[5] = "";
		CPU::decToHex(conversion_res, reg);
		result[0] = 'V'; // V
		result[1] = conversion_res[0]; // VX
		result[2] = ' '; // VX_
		result[3] = '('; // VX (

		uint8_t register_value = *(register_pointer + reg);
		char register_value_arr[4] = "";
		std::to_chars(register_value_arr, register_value_arr + 3, register_value);
		int final_char_idx = 0;

		if (register_value < 10) { // VX (.
			result[4] = register_value_arr[0];
			final_char_idx = 6;
		}
		else if (register_value < 100) { // VX (..
			result[4] = register_value_arr[0];
			result[5] = register_value_arr[1];
			final_char_idx = 7;
		}
		else { // VX (...
			result[4] = register_value_arr[0];
			result[5] = register_value_arr[1];
			result[6] = register_value_arr[2];
			final_char_idx = 8;
		}
		
		result[final_char_idx - 1] = ')'; // VX (__)
		result[final_char_idx] = '\0'; // VX (__)\0
	}

	// Free returned pointer after use
	void convert_addr(char (&result)[15], uint16_t addr) {
		char conversion_res[5] = "";
		decToHex(conversion_res, addr);

		size_t i = 0;
		while (conversion_res[i] != '\0') {
			i++;
		}

		result[0] = '0';
		result[1] = 'x';
		strcpy(result + 2, conversion_res);
		result[2 + i] = ' ';
		result[2 + i + 1] = '(';

		char register_value_arr[6] = ""; // Up to 65536 (5 characters), including null character
		std::to_chars(register_value_arr, register_value_arr + 5, addr);
		//strcpy(result + 2 + i + 2, register_value_arr);
		int final_char_idx = 0;
		if (addr < 10) { // VX (.
			result[2 + i + 2] = register_value_arr[0];
			final_char_idx = 2 + i + 4;
		}
		else if (addr < 100) { // VX (..
			result[2 + i + 2] = register_value_arr[0];
			result[2 + i + 3] = register_value_arr[1];
			final_char_idx = 2 + i + 5;
		}
		else if (addr < 1000) { // VX (...
			result[2 + i + 2] = register_value_arr[0];
			result[2 + i + 3] = register_value_arr[1];
			result[2 + i + 4] = register_value_arr[2];
			final_char_idx = 2 + i + 6;
		}
		else if (addr < 10000) {
			result[2 + i + 2] = register_value_arr[0];
			result[2 + i + 3] = register_value_arr[1];
			result[2 + i + 4] = register_value_arr[2];
			result[2 + i + 5] = register_value_arr[3];
			final_char_idx = 2 + i + 7;
		}
		else {
			result[2 + i + 2] = register_value_arr[0];
			result[2 + i + 3] = register_value_arr[1];
			result[2 + i + 4] = register_value_arr[2];
			result[2 + i + 5] = register_value_arr[3];
			result[2 + i + 6] = register_value_arr[4];
			final_char_idx = 2 + i + 8;
		}

		result[final_char_idx - 1] = ')';
		result[final_char_idx] = '\0';
	}

	void make_instruction_short(char (&result)[40],const char* operation, const char* format, uint16_t n1, uint16_t n2) {
		//if (format[0] != '0') assert(n1 != NULL);
		//if (format[2] != '0') assert(n2 != NULL);
		//char result[40] = "";
		//char* result = (char*)malloc(40);
		strcpy(result, operation);

		int i = 0;
		while (result[i] != '\0') {
			i++;
		}

		result[i] = ' ';

		if (format == "r,r") {
			char res1[9], res2[9];
			convert_register(res1, n1);
			convert_register(res2, n2);
			strcpy(result + i + 1, res1);
			while (result[i] != '\0') {
				i++;
			}

			result[i] = ',';
			result[i + 1] = ' ';
			strcpy(result + i + 2, res2);
		}
		else if (format == "r,a") {
			char res1[9], res2[15];
			convert_register(res1, n1);
			convert_addr(res2, n2);
			strcpy(result + i + 1, res1);
			while (result[i] != '\0') {
				i++;
			}
			result[i] = ',';
			result[i + 1] = ' ';
			strcpy(result + i + 2, res2);
		}
		else if (format == "a,r") {
			char res1[15], res2[9];
			convert_addr(res1, n1);
			convert_register(res2, n2);
			strcpy(result + i + 1, res1);
			while (result[i] != '\0') {
				i++;
			}
			result[i] = ',';
			result[i + 1] = ' ';
			strcpy(result + i + 2, res2);
		}
		else if (format == "a,a") {
			char res1[15], res2[15];
			convert_addr(res1, n1);
			convert_addr(res2, n2);

			strcpy(result + i + 1, res1);
			while (result[i] != '\0') {
				i++;
			}
			result[i] = ',';
			result[i + 1] = ' ';
			strcpy(result + i + 2, res2);
		}
		else if (format == "r,0") {
			char res1[9];
			convert_register(res1, n1);
			strcpy(result + i + 1, res1);
		}
		else if (format == "a,0") {
			char res1[15];
			convert_addr(res1, n1);
			strcpy(result + i + 1, res1);
		}
		else if (format == "0,0") {
			result[i] = '\0';
		}
		else {
			std::cout << "Invalid instruction format." << std::endl;
		}
	}

	void get_instruction_desc_from_value(char (&desc)[40], uint16_t instruction, bool short_desc) {
		uint8_t first_nibble = instruction >> 12; // Tells which type of instruction
		uint8_t X = (instruction >> 8) & 0xF; // Looks up one of the 16 VX registers from V0 through VF;
		uint8_t Y = (instruction >> 4) & 0xF; // Looks up one of the 16 VY registers from V0 through VF;
		uint8_t N = instruction & 0xF; // Fourth nibble
		uint8_t NN = instruction & 0xFF; // Second byte (8-bit immediate number)
		uint16_t NNN = instruction & 0xFFF; // second, third, fourth nibbles (12-bit immediate memory address)

		uint8_t* vx = (uint8_t*)(register_pointer + X);
		uint8_t* vy = (uint8_t*)(register_pointer + Y);
		uint8_t* carry_flag = (uint8_t*)(register_pointer + 15);
		uint16_t* index_register = (uint16_t*)(register_pointer + 16);

		const char* instruction_string = CPU::decToHex_and_pad(instruction);

		switch (first_nibble) {
		case (0x0):
			switch (NNN) {
			case (0x0E0): // 00E0 - CLS (Clears the display)
				//short_desc? "CLS" : "Clears the display";
				short_desc ? make_instruction_short(desc, "CLS", "0,0", NULL, NULL) : void();
				break;
			case (0x0EE): // 00EE - RET (Returns from subroutine)
				//short_desc ? "RET" : "Returns from subroutine";
				short_desc ? make_instruction_short(desc, "RET", "0,0", NULL, NULL) : void();
				break;
			default:
				break;
			}
			break;
		case (0x1): // 1NNN - JP NNN (Jumps to NNN)
			short_desc ? make_instruction_short(desc, "JP", "a,0", NNN, NULL) : void();
			//short_desc ? "JP " + instruction_string : "Jumps to " + instruction_string;
			break;
		case (0x2): // 2NNN - CALL NNN (Calls subroutine at NNN)
			short_desc ? make_instruction_short(desc, "CALL", "a,0", NNN, NULL) : void();
			//short_desc ? "CALL " + instruction_string : "Calls subroutine at " + instruction_string;
			break;
		case (0x3): // 3XNN - SE VX, NN (Skips next instruction if VX = NN)
			short_desc ?
				make_instruction_short(desc, "SE", "r,a", X, NN)
				: void()
				//"Skips next instruction if V" + CPU::decToHex(X) + " (" + std::to_string(*vx) + ") " + " = " + CPU::decToHex_and_pad(NN) + " (" + std::to_string(NN) + ")"
				;
			break;
		case (0x4): // 4XNN - SNE VX, NN (Skips next instruction if VX != NN)
			short_desc ? make_instruction_short(desc, "SNE", "r,a", X, NN) : void();
			break;
		case (0x5): 
			switch (N) {
			case (0x0): // 5XY0 - SE VX, VY (Skips next instruction if VX = VY)
				make_instruction_short(desc, "SE", "r,r", X, Y);
				break;
			}
			
			break;
		case (0x6): // 6XNN - LD VX, NN (Sets VX to NN)
			make_instruction_short(desc, "LD", "r,a", X, NN);
			break;
		case (0x7): // 7XNN - ADD VX, NN (Adds NN to VX)
			make_instruction_short(desc, "ADD", "r,a", X, NN);
			break;
		case (0x8):
			switch (N) {
			case 0x0: // 8XY0 - LD VX, VY (Sets VX to VY)
				make_instruction_short(desc, "LD", "r,r", X, Y);
				break;
			case 0x1: // 8XY1 - OR VX, VY (Sets VX to VX OR VY)
				make_instruction_short(desc, "OR", "r,r", X, Y);
				break;
			case 0x2: // 8XY2 - AND VX, VY (Sets VX to VX AND VY)
				make_instruction_short(desc, "AND", "r,r", X, Y);
				break;
			case 0x3: // 8XY3 - XOR VX, VY (Sets VX to VX XOR VY)
				make_instruction_short(desc, "XOR", "r,r", X, Y);
				break;
			case 0x4: // 8XY4 - ADD VX, VY (Adds VY to VX)
				make_instruction_short(desc, "ADD", "r,r", X, Y);
				break;
			case 0x5: // 8XY5 - SUB VX, VY (Subtracts VY from VX)
				make_instruction_short(desc, "SUB", "r,r", X, Y);
				break;
			case 0x6: // 8XY6 - SHR VX (Shifts VX right by 1)
				make_instruction_short(desc, "SHR", "r,0", X, NULL);
				break;
			case 0x7: // 8XY7 - SUBN VX, VY (Sets VX to VY - VX)
				make_instruction_short(desc, "SUBN", "r,r", X, Y);
				break;
			case 0xE: // 8XYE - SHL VX (Shifts VX left by 1)
				make_instruction_short(desc, "SHL", "r,0", X, NULL);
				break;
			}
			break;

		case (0x9): 
			switch (N) {
			case (0x0): // 9XY0 - SNE VX, VY (Skips next instruction if VX != VY)
				make_instruction_short(desc, "SNE", "r,r", X, Y);
				break;
			}
			
			break;
		case (0xA): // ANNN - LD I, NNN (Sets I to NNN)
		{
			char converted_addr[5] = "";
			CPU::decToHex(converted_addr, *index_register);

			strcpy(desc, "LD I (0x");
			strcpy(desc + 8, converted_addr);

			int null_char_idx = 0;
			while (converted_addr[null_char_idx] != '\0') null_char_idx++;
			strcpy(desc + 8 + null_char_idx, "), ");

			char converted_addr2[15] = "";
			convert_addr(converted_addr2, NNN);
			strcpy(desc + 8 + null_char_idx + 3, converted_addr2);
		}
			break;
		case (0xB): // BNNN - JP V0, NNN (Jumps to NNN + V0)
			make_instruction_short(desc, "JP", "r,a", 0, NNN);
			break;
		case (0xC): // CXNN - RND VX, NN (Sets VX to a random number between (inclusive) 0-255 then ANDS with NN)
		{
			make_instruction_short(desc, "RND", "r,a", X, NN);
		}
			break;
		case (0xD): // DXYN - DRW VX, VY, N (Draws sprite located at address I, of height N, at X coordinate in VX, and Y coordinate in VY)
		{
			make_instruction_short(desc, "DRW", "r,r", X, Y);
			int null_char_idx = 0;
			while (desc[null_char_idx] != '\0') null_char_idx++;
			desc[null_char_idx] = ',';
			desc[null_char_idx + 1] = ' ';

			char converted_num[4] = ""; // Size 4 because N can be up to 255, including the null character that gives 4 bytes
			std::to_chars(converted_num, converted_num + 3, N);
			strcpy(desc + null_char_idx + 2, converted_num);
		}
			break;
		case (0xE):
			switch (NN) {
			case (0x9E): // EX9E - SKP VX (Skips next instruction if key stored in VX is pressed)
				make_instruction_short(desc, "SKP", "r,0", X, NULL);
				break;
			case (0xA1): // EXA1 - SKNP VX (Skips next instruction if key stored in VX is NOT pressed)
				make_instruction_short(desc, "SKNP", "r,0", X, NULL);
				break;
			default:
				break;
			}
			break;
		case (0xF):
			switch (NN) {
			case (0x07): // FX07 - LD VX, DT (Sets VX to delay timer)
			{
				char converted_reg[9] = "";
				convert_register(converted_reg, X);
				strcpy(desc, "LD ");
				strcpy(desc + 3, converted_reg);
				int null_char_idx = 0;
				while (converted_reg[null_char_idx] != '\0') null_char_idx++;
				strcpy(desc + 3 + null_char_idx, ", DT (");
				char converted_delay[4] = "";
				std::to_chars(converted_delay, converted_delay + 3, delay);
				strcpy(desc + 3 + null_char_idx + 6, converted_delay);
				int new_null_char_idx = 0;
				while (converted_delay[new_null_char_idx] != '\0') new_null_char_idx++;
				desc[3 + null_char_idx + 6 + new_null_char_idx] = ')';
				desc[3 + null_char_idx + 6 + new_null_char_idx + 1] = '\0';
			}
				break;
			case (0x0A): // FX0A - LD VX, K (Waits for key press, set VX to the value of the pressed key)
			{
				char converted_reg[9] = "";
				convert_register(converted_reg, X);
				strcpy(desc, "LD ");
				strcpy(desc + 3, converted_reg);
				int null_char_idx = 0;
				while (converted_reg[null_char_idx] != '\0') null_char_idx++;
				strcpy(desc + 3 + null_char_idx, ", K");
				desc[3 + null_char_idx + 3] = '\0';
			}
				break;
			case (0x15): // FX15 - LD DT, VX (Sets delay timer to VX)
			{
				char converted_reg[9] = "";
				convert_register(converted_reg, X);
				strcpy(desc, "LD ");
				strcpy(desc + 3, "DT (");
				char converted_delay[4] = "";
				std::to_chars(converted_delay, converted_delay + 3, delay);
				strcpy(desc + 3 + 4, converted_delay);
				int null_char_idx = 0;
				while (converted_delay[null_char_idx] != '\0') null_char_idx++;
				strcpy(desc + 3 + 4 + null_char_idx, "), ");
				strcpy(desc + 3 + 4 + null_char_idx + 3, converted_reg);
			}
				break;
			case (0x18): // FX18 - LD ST, VX (Sets sound timer to VX)
			{
				char converted_reg[9] = "";
				convert_register(converted_reg, X);
				strcpy(desc, "LD ");
				strcpy(desc + 3, "ST (");
				char converted_sound[4] = "";
				std::to_chars(converted_sound, converted_sound + 3, sound);
				strcpy(desc + 3 + 4, converted_sound);
				int null_char_idx = 0;
				while (converted_sound[null_char_idx] != '\0') null_char_idx++;
				strcpy(desc + 3 + 4 + null_char_idx, "), ");
				strcpy(desc + 3 + 4 + null_char_idx + 3, converted_reg);
			}
				break;
			case (0x1E): // FX1E - ADD I, VX (Adds VX to I)
			{
				char converted_reg[9] = "";
				convert_register(converted_reg, X);
				strcpy(desc, "ADD ");
				strcpy(desc + 4, "I (0x");
				char converted_i[5] = "";
				CPU::decToHex(converted_i, *index_register);
				strcpy(desc + 4 + 5, converted_i);
				int null_char_idx = 0;
				while (converted_i[null_char_idx] != '\0') null_char_idx++;
				strcpy(desc + 4 + 5 + null_char_idx, "), ");
				strcpy(desc + 4 + 5 + null_char_idx + 3, converted_reg);
			}
				break;
			case (0x29): // FX29 - LD F, VX (Sets I to location of sprite for digit in VX)
			{
				char converted_X[5] = "";
				char converted_reg[5] = "";
				CPU::decToHex(converted_X, X);
				CPU::decToHex(converted_reg, *vx);
				strcpy(desc, "LD F, ");
				desc[6] = 'V';
				desc[7] = converted_reg[0];
				strcpy(desc + 8, " (");
				strcpy(desc + 10, converted_reg);
				int null_char_idx = 0;
				while (converted_reg[null_char_idx] != '\0') null_char_idx++;
				desc[10 + null_char_idx] = ')';
				desc[10 + null_char_idx + 1] = '\0';
			}
				break;
			case (0x33): // FX33 - LD B, VX (Stores the binary-coded decimal representation of VX in I, I+1, I+2, respectively)
			{
				char converted_reg[9] = "";
				convert_register(converted_reg, X);
				strcpy(desc, "LD B, ");
				strcpy(desc + 6, converted_reg);
			}
				break;
			case (0x55): // FX55 - LD [I], VX (Stores all values from the range (inclusive) V0-VX into the range (inclusive) I-I+X)
			{
				strcpy(desc, "LD [I], V");
				char converted_reg[5] = "";
				CPU::decToHex(converted_reg, X);
				desc[9] = converted_reg[0];
				desc[10] = '\0';
			}
				break;
			case (0x65): // FX65 - LD [VX], I (Stores all values from the range (inclusive) I-I+X into the range (inclusive) V0-VX)
			{
				strcpy(desc, "LD V");
				char converted_reg[5] = "";
				CPU::decToHex(converted_reg, X);
				desc[4] = converted_reg[0];
				strcpy(desc + 5, ", [I]");
			}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
};