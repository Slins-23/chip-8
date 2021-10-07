#include "../Graphics.h"
#include "Debug_Window.h"

#pragma once
namespace Display_Window {
	float displayH_factor = 0.6;
	float displayW_factor = 0.4;

	void draw_display() {
		ImGui::SetNextWindowPos(ImVec2(Graphics::width * (ROM_Window::romW_factor + Debug_Window::debugW_factor), 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Graphics::width * displayW_factor, Graphics::height * displayH_factor));
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
};