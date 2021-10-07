workspace "CHIP-8"
	configurations {"Debug", "Release"}
	platforms {"x86", "x64"}

	filter "platforms:x86"
		kind "StaticLib"
		architecture "x86"

	filter "platforms:x64"
		kind "StaticLib"
		architecture "x64"


outputdir = "%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}"

project "CHIP-8"
	location "CHIP-8"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{prj.name}/".. outputdir .. "/bin")
	objdir ("%{prj.name}/" .. outputdir .. "/obj")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glad/src/glad.c",
		"%{prj.name}/vendor/imgui/*.cpp",
		"%{prj.name}/vendor/imgui/backends/*",
		"%{prj.name}/vendor/nativefiledialog/*.h",
		"%{prj.name}/vendor/nativefiledialog/nfd_common.c",
		"%{prj.name}/vendor/nativefiledialog/include/nfd.h",
	}

	includedirs
	{
		"%{prj.name}/vendor/glad/include",
		"%{prj.name}/vendor/sdl/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/nativefiledialog/include",
	}

	links
	{
		"SDL2",
		"SDL2main",
	}

	filter "configurations:Debug"
		defines "CH_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "CH_RELEASE"
		optimize "On"

	filter "system:windows"
		files {"%{prj.name}/vendor/nativefiledialog/nfd_win.cpp"}
		systemversion "latest"

	filter "system:linux"
		files {"%{prj.name}/vendor/nativefiledialog/nfd_gtk.c"}
		buildoptions {"`pkg-config --cflags gtk+-3.0`"}
		linkoptions {"`pkg-config --libs gtk+-3.0`", "-pthread", "-ldl"}

	filter "system:macosx"
		files {"%{prj.name}/vendor/nativefiledialog/nfd_cocoa.m"}
		links {"Foundation.framework", "AppKit.framework"}

	filter {"platforms:x86", "system:windows"}
		libdirs
		{
			"%{prj.name}/vendor/sdl/lib/x86",
		}
		postbuildcommands 
		{ 
			("{COPY} %{prj.location}/vendor/sdl/lib/%{cfg.platform}/SDL2.dll %{prj.location}/" .. outputdir .. "/bin")
		}

	filter {"platforms:x64", "system:windows"}
		libdirs
		{
			"%{prj.name}/vendor/sdl/lib/x64",
		}
		postbuildcommands 
		{ 
			("{COPY} %{prj.location}/vendor/sdl/lib/%{cfg.platform}/SDL2.dll %{prj.location}/" .. outputdir .. "/bin")
		}
		
	filter {"configurations:Release", "system:windows"}
		linkoptions "/SUBSYSTEM:WINDOWS"
	
	filter {"configurations:Debug", "system:windows"}
		linkoptions "/SUBSYSTEM:CONSOLE"

--[[
newaction 
{
	trigger = "clean"
	description = "Clean all output files"
	execute = function ()
		files_to_delete =
		{
			"Makefile",
		}
}
--]]