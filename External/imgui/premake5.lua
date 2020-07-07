project "imgui"
	kind				"StaticLib"
	language			"C++"
	cppdialect			"C++17"
	staticruntime		"on"
	targetdir			("../../.bin/"	  .. outputdir .. "/%{prj.name}")
	objdir				("../../.bin_obj/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"",
		".."
	}

	files
	{
		"**.cpp",
		"**.h",
		"**.hpp",
		"**.inl"
	}

	filter "system:windows"
		systemversion	"latest"

	filter "system:linux"
		systemversion	"latest"
		pic				"on"

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