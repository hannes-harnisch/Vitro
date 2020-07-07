workspace "Vitro ALT"
	architecture		"x64"
	platforms			{ "DirectX", "Vulkan" }
	configurations		{ "Debug", "Development", "Release" }
	flags				{ "MultiProcessorCompile" }
	startproject		"VitroTests"

outputdir = "%{cfg.buildcfg}_%{cfg.architecture}_%{cfg.system}"
build_optimization_flags = { "/GT", "/GL", "/Ot", "/Ob3" }
link_optimization_flags = { "/LTCG" }

group "Dependencies"
	include "External/assimp"
	include "External/imgui"
	include "External/stb"
group ""

project "VitroEngine"
	location			"VitroEngine"
	kind				"StaticLib"
	language			"C++"
	cppdialect			"C++17"
	staticruntime		"on"
	rtti				"off"
	objdir				(".bin_obj/" .. outputdir .. "/%{prj.name}")
	targetdir			(".bin/"	 .. outputdir .. "/%{prj.name}")
	libdirs				"External"
	pchsource			"%{prj.name}/_pch.cpp"
	pchheader			"_pch.h"
	links				{ "assimp", "imgui" }

	files
	{
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.h",
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.inl"
	}

	includedirs
	{
		"%{prj.name}",
		"External"
	}

	filter "system:windows"
		systemversion	"latest"
		defines			{ "VTR_SYSTEM_WINDOWS", "WIN32_LEAN_AND_MEAN" }

	filter "system:linux"
		systemversion	"latest"
		pic				"on"
		
	filter "configurations:Debug"
		runtime			"Debug"
		symbols			"on"
		defines			{ "VTR_DEBUG", "VTR_ENGINE_LOG_LEVEL=VTR_LOG_LEVEL_DEBUG" }
		
	filter "configurations:Development"
		runtime			"Debug"
		symbols			"on"
		optimize		"speed"
		buildoptions	(build_optimization_flags)
		linkoptions		(link_optimization_flags)
		defines			{ "VTR_DEBUG", "VTR_ENGINE_LOG_LEVEL=VTR_LOG_LEVEL_DEBUG" }

	filter "configurations:Release"
		runtime			"Release"
		optimize		"speed"
		buildoptions	(build_optimization_flags)
		linkoptions		(link_optimization_flags)
		defines			{ "VTR_RELEASE", "VTR_ENGINE_LOG_LEVEL=VTR_LOG_LEVEL_ERROR" }

	filter "platforms:DirectX"
		defines			"VTR_API_DIRECTX"
		links			"d3d11"

	filter "platforms:Vulkan"
		defines			"VTR_API_VULKAN"

project "VitroTests"
	location			"VitroTests"
	kind				"ConsoleApp"
	language			"C++"
	cppdialect			"C++17"
	staticruntime		"on"
	objdir				(".bin_obj/" .. outputdir .. "/%{prj.name}")
	targetdir			(".bin/"	 .. outputdir .. "/%{prj.name}")
	debugdir			(".bin/"	 .. outputdir .. "/%{prj.name}")
	links				"VitroEngine"

	files
	{
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.h",
		"%{prj.name}/**.hlsl"
	}

	includedirs
	{
		"VitroEngine"
	}
	
	filter "files:**.hlsl"
		flags			"ExcludeFromBuild"
		shadermodel		"5.0"
		
	filter "files:**Vertex.hlsl"
		removeflags		"ExcludeFromBuild"
		shadertype		"Vertex"
		
	filter "files:**Fragment.hlsl"
		removeflags		"ExcludeFromBuild"
		shadertype		"Pixel"

	filter "system:windows"
		systemversion	"latest"
		defines			{ "VTR_SYSTEM_WINDOWS", "WIN32_LEAN_AND_MEAN" }

	filter "system:linux"
		systemversion	"latest"
		pic				"on"
		defines			"VTR_SYSTEM_LINUX"
		
	filter "configurations:Debug"
		runtime			"Debug"
		symbols			"on"
		
	filter "configurations:Development"
		runtime			"Debug"
		symbols			"on"
		optimize		"speed"
		buildoptions	(build_optimization_flags)
		linkoptions		(link_optimization_flags)

	filter "configurations:Release"
		runtime			"Release"
		optimize		"speed"
		buildoptions	(build_optimization_flags)
		linkoptions		(link_optimization_flags)

	filter "platforms:DirectX"
		defines			"VTR_API_DIRECTX"