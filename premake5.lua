output_dir				= '%{cfg.buildcfg}_%{cfg.architecture}_%{cfg.system}'

workspace 'Vitro'
	startproject		'VitroEngine'
	architecture		'x64'
	configurations		{ 'Debug', 'Development', 'Release' }
	flags				{ 'MultiProcessorCompile' }
	language			'C++'
	cppdialect			'C++latest'
	conformancemode		'On'
	warnings			'Extra'
	disablewarnings		'4201'
	floatingpoint		'Fast'
	files				{
							'%{prj.name}/**.cc',
							'%{prj.name}/**.hh',
							'%{prj.name}/**.hlsl'
						}
	removefiles			{
							'%{prj.name}/**/**.*.cc',
							'prj.name}/**/**.*.hh'
						}
	debugdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	targetdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	objdir				('.bin_int/' .. output_dir .. '/%{prj.name}')

	filter 'files:**.cc'
		compileas		'Module'

	filter 'files:**.hh'
		compileas		'HeaderUnit'

	filter 'files:**.hlsl'
		buildmessage	'Compiling shader %{file.relpath}'
		buildcommands	'C:/VulkanSDK/1.2.176.1/bin/dxc %{file.relpath} /Fo %{cfg.targetdir}/%{file.basename}.spv -spirv -O3 -T ^'
		buildoutputs	'%{cfg.targetdir}/%{file.basename}.spv'

	filter 'files:**.vert.hlsl'
		buildcommands	'vs_6_6'

	filter 'files:**.frag.hlsl'
		buildcommands	'ps_6_6'

	filter 'configurations:Debug'
		symbols			'On'
		runtime			'Debug'
		defines			'VT_DEBUG'

	filter 'configurations:Development'
		symbols			'On'
		optimize		'Speed'
		runtime			'Debug'
		defines			'VT_DEBUG'
		flags			'LinkTimeOptimization'

	filter 'configurations:Release'
		optimize		'Speed'
		runtime			'Release'
		flags			'LinkTimeOptimization'

project 'VitroEngine'
	location			'%{prj.name}'
	kind				'ConsoleApp'
	includedirs			{ '%{prj.name}', 'Dependencies' }
	defines				'VT_ENGINE_NAME="%{prj.name}"'
	links				'D3D12MemoryAllocator'

	filter 'configurations:Release'
		kind			'WindowedApp'
		entrypoint		'mainCRTStartup'

	filter 'system:Windows'
		systemversion	'latest'
		files			{
							'%{prj.name}/**/Windows.*.*',
							'%{prj.name}/**/D3D12.*.*',
							'%{prj.name}/**/Vulkan.*.*',
							'%{prj.name}/**/VulkanWindows.*.*'
						}
		includedirs		'C:/VulkanSDK/1.2.176.1/Include'
		defines			{
							'VT_SYSTEM_MODULE=Windows',
							'VT_SYSTEM_NAME=windows',
							'VT_GPU_API_MODULE=D3D12',
							'VT_GPU_API_NAME=d3d12'
						}

group 'Dependencies'

	project 'D3D12MemoryAllocator'
		location		'Dependencies'
		kind			'StaticLib'
		files			{
							'Dependencies/%{prj.name}/**/D3D12MemAlloc.cpp',
							'Dependencies/%{prj.name}/**/D3D12MemAlloc.h'
						}
		warnings		'Off'