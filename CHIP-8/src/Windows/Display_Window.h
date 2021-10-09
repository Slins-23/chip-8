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
		//glBindFramebuffer(GL_FRAMEBUFFER, Graphics::fbo);
		glBindTexture(GL_TEXTURE_2D, Graphics::textureFbo);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Graphics::horizontal_tiles, Graphics::vertical_tiles, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, Graphics::original_pixels);


		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Graphics::rbo);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Graphics::textureFbo, 0);


		//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
			//ImGui::Image((ImTextureID)Graphics::fbo, size);
		ImGui::Image((ImTextureID)Graphics::textureFbo, size);
			// Need to find a way to draw tiles over this Image, whether that be by manually modifying the resulting pixels, or drawing non-filled quads above it. How to draw quads over this image?
			//ImDrawList* dL = ImGui::GetWindowDrawList();
			//dL->AddRectFilled(ImVec2(-1.f, 1.f), ImVec2(1.f, 1.f), ImU32(0xFF0000FF), 0.f, NULL);

			/*
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glColor3f(1.0f, 1.0f, 0.0f);
			glBegin(GL_QUADS);
			glVertex2f(-1.f, 1.f);
			glVertex2f(0.f, 1.f);
			glVertex2f(0.f, 0.5f);
			glVertex2f(-1.f, 0.5f);
			glEnd();
			*/
		//}
		//else {
		//	printf("Could not initialize framebuffer.\n");
			//exit(-1);
		//}



		glBindTexture(GL_TEXTURE_2D, 0);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ImGui::EndChild();
		ImGui::End();
	}
};