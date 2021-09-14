output_dir				= '%{cfg.buildcfg}_%{cfg.architecture}_%{cfg.system}'

workspace 'Vitro'
	startproject		'VitroEngine'
	architecture		'x64'
	configurations		{ 'Debug', 'Development', 'Release' }
	platforms			{ 'D3D12', 'Vulkan', 'D3D12+Vulkan' }
	flags				'MultiProcessorCompile'
	language			'C++'
	cppdialect			'C++latest'
	conformancemode		'On'
	warnings			'Extra'
	disablewarnings		'4201' -- anonymous structs
	floatingpoint		'Fast'
	toolset				'msc'
	files				{
							'%{prj.name}/**.cc',
							'%{prj.name}/**.hh',
							'%{prj.name}/**.hlsl'
						}
	removefiles			'**/Vendor/**'
	debugdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	targetdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	objdir				('.bin_int/' .. output_dir .. '/%{prj.name}')

	filter 'files:**.cc'
		compileas		'Module'

	filter 'files:**.hlsl'
		buildmessage	'Compiling shader %{file.relpath}'
		buildcommands	'C:/VulkanSDK/**/bin/dxc %{file.relpath} /Fo %{cfg.targetdir}/%{file.basename}.spv -O3 ^'
		buildoutputs	'%{cfg.targetdir}/%{file.basename}.spv'
		
	filter { 'files:**.hlsl', 'platforms:Vulkan or D3D12+Vulkan' }
		buildcommands	'-spirv ^'

	filter 'files:**.vert.hlsl'
		buildcommands	'-T vs_6_6'

	filter 'files:**.frag.hlsl'
		buildcommands	'-T ps_6_6'

	filter 'Debug'
		symbols			'On'
		runtime			'Debug'
		defines			'VT_DEBUG'

	filter 'Development'
		symbols			'On'
		optimize		'Speed'
		runtime			'Debug'
		defines			'VT_DEBUG'
		flags			'LinkTimeOptimization'

	filter 'Release'
		optimize		'Speed'
		runtime			'Release'
		flags			'LinkTimeOptimization'

project 'VitroEngine'
	location			'%{prj.name}'
	kind				'ConsoleApp'
	includedirs			{ '%{prj.name}', 'Dependencies' }
	defines				'VT_ENGINE_NAME="%{prj.name}"'

	filter 'Release'
		kind			'WindowedApp'
		entrypoint		'mainCRTStartup'

	filter 'system:Windows'
		systemversion	'latest'
		files			'%{prj.name}/**/Windows/**'
		defines			{
							'VT_SYSTEM_MODULE=Windows',
							'VT_SYSTEM_NAME=windows',
						}

	filter 'platforms:D3D12 or D3D12+Vulkan'
		links			{ 'd3d12', 'dxgi', 'D3D12MemoryAllocator' }
		files			'%{prj.name}/**/D3D12/**'

	filter 'platforms:D3D12'
		defines			{
							'VT_GPU_API_MODULE=D3D12',
							'VT_GPU_API_NAME=d3d12'
						}

	filter 'platforms:Vulkan or D3D12+Vulkan'
		files			'%{prj.name}/**/Vulkan/**'
		includedirs		'C:/VulkanSDK/**/Include'

	filter 'platforms:Vulkan'
		defines			{
							'VT_GPU_API_MODULE=Vulkan',
							'VT_GPU_API_NAME=vulkan'
						}

	filter 'platforms:D3D12+Vulkan'
		defines			{
							'VT_DYNAMIC_GPU_API',
							'VT_GPU_API_MODULE_PRIMARY=D3D12',
							'VT_GPU_API_NAME_PRIMARY=d3d12',
							'VT_GPU_API_MODULE=Vulkan',
							'VT_GPU_API_NAME=vulkan'
						}

group 'Dependencies'

	project 'D3D12MemoryAllocator'
		location		'Dependencies'
		kind			'StaticLib'
		files			{
							'**/%{prj.name}/**/D3D12MemAlloc.cpp',
							'**/%{prj.name}/**/D3D12MemAlloc.h'
						}
		warnings		'Off'