#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <ios>
#include <fstream>
#include <sys/stat.h>
#include <cstdint>
#include <cstring>
#include <cmath>

#pragma once

char _dig_vec_upper[] =
"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char _dig_vec_lower[] =
"0123456789abcdefghijklmnopqrstuvwxyz";

// Using this custom function for the decimal -> hexadecimal conversion, as "itoa" which was previously used is not cross-platform and used strings instead of character arrays. Sprintf is slightly slower for values less than 4096 (0x200), int2str is slower for values (~10 times) greater than that, and it keeps getting slower proportionately to the value.
// From https://github.com/mysql/mysql-server/blob/5.7/strings/int2str.c
char* int2str(long int val, char* dst, int radix, int upcase) {
	char buffer[65];
	char* p;
	long int new_val;
	char* dig_vec = upcase ? _dig_vec_upper : _dig_vec_lower;
	uint32_t uval = (uint32_t)val;

	if (radix < 0)
	{
		if (radix < -36 || radix > -2)
			return nullptr;
		if (val < 0)
		{
			*dst++ = '-';
			/* Avoid integer overflow in (-val) for LLONG_MIN (BUG#31799). */
			uval = (uint32_t)0 - uval;
		}
		radix = -radix;
	}
	else if (radix > 36 || radix < 2)
		return nullptr;

	/*
	  The slightly contorted code which follows is due to the fact that
	  few machines directly support unsigned long / and %.  Certainly
	  the VAX C compiler generates a subroutine call.  In the interests
	  of efficiency (hollow laugh) I let this happen for the first digit
	  only; after that "val" will be in range so that signed integer
	  division will do.  Sorry 'bout that.  CHECK THE CODE PRODUCED BY
	  YOUR C COMPILER.  The first % and / should be unsigned, the second
	  % and / signed, but C compilers tend to be extraordinarily
	  sensitive to minor details of style.  This works on a VAX, that's
	  all I claim for it.
	*/
	p = &buffer[sizeof(buffer) - 1];
	*p = '\0';
	new_val = uval / (uint32_t)radix;
	*--p = dig_vec[(uint8_t)(uval - (uint32_t)new_val * (uint32_t)radix)];
	val = new_val;
	while (val != 0)
	{
		ldiv_t res;
		res = ldiv(val, radix);
		*--p = dig_vec[res.rem];
		val = res.quot;
	}
	while ((*dst++ = *p++) != 0);
	return dst - 1;
}

const uint8_t font[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

namespace RAM {
	const size_t totalSpaceInBytes = 0x1000;
	const size_t interpreterSpaceInBytes = 0x200;
	const size_t romSpaceInBytes = totalSpaceInBytes - interpreterSpaceInBytes;
	const size_t fontSpaceInBytes = 0x50;

	const char* ROMS_FOLDER = "B:\\Programming\\C++\\CHIP-8\\CHIP-8\\ROMS\\";
	char rom_path[261];
	char rom_title[261];
	uint8_t* buffer;
	uint8_t* rom_data = nullptr;
	uint16_t rom_size;
	uint16_t byte_rows;
	uint8_t stack_size = 16;
	uint16_t* sp;
	uint16_t* sp_addr;
	int bytes_per_row = 16;

	void print() {
		for (int i = 0; i < totalSpaceInBytes; i++) {
			if (i > 511 && i < 4095) {
				char current_addr[5], current_byte[3], next_byte[3];
				int2str(i, current_addr, 16, 1);
				int2str(buffer[i], current_byte, 16, 1);
				int2str(buffer[i + 1], next_byte, 16, 1);
				//_itoa_s(i, current_addr, 16);
				//_itoa_s(buffer[i], current_byte, 16);
				//_itoa_s(buffer[i + 1], next_byte, 16);

				if (buffer[i] < 16) {
					current_byte[1] = current_byte[0];
					current_byte[0] = '0';
					current_byte[2] = '\0';
				}

				if (buffer[i + 1] < 16) {
					next_byte[1] = next_byte[0];
					next_byte[0] = '0';
					next_byte[2] = '\0';
				}

				char new_str[5];
				//memcpy(new_str, current_byte, 2);
				//memcpy(new_str + 2, next_byte, 3);
				// Endianness

				int q = 0;
				for (q; q < 3; q++) {
					if (current_byte[q] == '\0') break;
					else new_str[q] = current_byte[q];
				}

				for (int j = 0; j < 3; j++) {
					new_str[q + j] = next_byte[j];
				}

				std::cout << "Buffer " << i << " (0x" << current_addr << "): " << new_str << std::endl;
				i++;
			}
			else if (i == 4095) {
				break;
			}
			else {
				char str[4], str2[3];
				int2str(i, str, 16, 1);
				int2str(buffer[i], str2, 16, 1);
				//_itoa_s(i, str, 16);
				//_itoa_s(buffer[i], str2, 16);
				std::cout << "Buffer " << i << " (0x" << str << "): " << str2 << std::endl;
			}
		}
	}

	void load_rom(char* path) {

		memset(buffer + interpreterSpaceInBytes, 0x00, romSpaceInBytes);

		std::string str_path = path;
		strcpy(rom_path, path);

		// Paths on Windows and Unix systems are not the same: Directory separator in Windows is a backslash '\', on Unix it's a forwardslash '/'.
#ifdef _WIN32
		strcpy(rom_title, str_path.substr(str_path.find_last_of("\\") + 1).c_str());
#else
		strcpy(rom_title, str_path.substr(str_path.find_last_of("/") + 1).c_str());
#endif


		struct stat results;

		if (stat(path, &results) == 0) {
			//std::cout << "File size: " << results.st_size << std::endl;
		}
		else {
			throw;
		}

		if (rom_data != nullptr) {
			free(rom_data);
		}

		rom_size = results.st_size;
		rom_data = (uint8_t*)malloc(rom_size);
		std::ifstream file(path, std::ios::in | std::ios::binary);

		byte_rows = ceil(rom_size / bytes_per_row);
		
		if (!file.read((char*) rom_data, rom_size)) {
			std::cout << "Error. Could not read file " << path << std::endl;
			std::cout << file.gcount() << " bytes" << " out of " << rom_size << " read." << std::endl;
			throw;
		};

		memcpy(buffer + interpreterSpaceInBytes, rom_data, rom_size);
		free(path);
	}

	bool initialize() {
		sp = (uint16_t*)calloc(stack_size + 1, sizeof(uint16_t)); // 1 extra to avoid pointer overflowing
		sp_addr = sp;
		buffer = (uint8_t*)malloc(totalSpaceInBytes);
		memcpy(buffer, font, sizeof(font));
		memset(buffer + sizeof(font), 0xFF, interpreterSpaceInBytes - sizeof(font));
		memset(buffer + interpreterSpaceInBytes, 0x00, romSpaceInBytes);
		return 1;
	}

	void close() {
		free(rom_data);
		free(sp_addr);
		free(buffer);
	}


};