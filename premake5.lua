output_dir				= '%{cfg.buildcfg}_%{cfg.architecture}_%{cfg.system}'

workspace 'Vitro'
	startproject		'Vitro'
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
	staticruntime		'On'
	files				{
							'%{prj.name}/**.cpp',
							'%{prj.name}/**.hpp',
							'%{prj.name}/**.hlsl',
							'%{prj.name}/**.hlsli',
							'%{prj.name}/**.natvis',
						}
	removefiles			'**/Platform**/'
	debugdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	targetdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	objdir				('.bin_int/' .. output_dir .. '/%{prj.name}')

	filter 'files:**.cpp'
		compileas		'Module'

	filter 'files:**.hlsl'
		buildmessage	'Compiling shader %{file.relpath}'
		buildcommands	'C:/VulkanSDK/1.2.189.2/bin/dxc %{file.relpath} -O3 /Fo %{cfg.targetdir}/%{file.basename}^'

	filter { 'files:**.hlsl', 'platforms:D3D12' }
		buildcommands	'.cso /D VT_GPU_API_D3D12 ^'
		buildoutputs	'%{cfg.targetdir}/%{file.basename}.cso'

	filter { 'files:**.hlsl', 'platforms:Vulkan' }
		buildcommands	'.spv -spirv /D VT_GPU_API_VULKAN ^'
		buildoutputs	'%{cfg.targetdir}/%{file.basename}.spv'

	filter 'files:**.vert.hlsl'
		buildcommands	'-T vs_6_0'

	filter 'files:**.frag.hlsl'
		buildcommands	'-T ps_6_0'

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

project 'Vitro'
	location			'%{prj.name}'
	kind				'ConsoleApp'
	includedirs			{ '%{prj.name}', 'Dependencies' }
	defines				'VT_ENGINE_NAME="%{prj.name}"'
	targetname			'%{prj.name}%{cfg.platform}'
	debugargs			{ '-debug-gpu-api' }

	-- filter 'Release'
		-- kind			'WindowedApp'
		-- entrypoint		'mainCRTStartup'

	filter 'system:Windows'
		systemversion	'latest'
		files			'%{prj.name}/**/PlatformWindows/*'
		links			'user32'
		defines			{
							'VT_SYSTEM_WINDOWS',
							'VT_SYSTEM_MODULE=Windows',
							'VT_SYSTEM_NAME=windows',
						}

	filter 'platforms:D3D12 or D3D12+Vulkan'
		links			{ 'd3d12', 'dxgi', 'D3D12MemoryAllocator' }
		files			'%{prj.name}/**/PlatformD3D12/*'

	filter 'platforms:D3D12'
		defines			{
							'VT_GPU_API_MODULE=D3D12',
							'VT_GPU_API_NAME=d3d12',
							'VT_SHADER_EXTENSION="cso"',
						}

	filter 'platforms:Vulkan or D3D12+Vulkan'
		files			'%{prj.name}/**/PlatformVulkan/*'
		includedirs		'C:/VulkanSDK/**/Include'

	filter 'platforms:Vulkan'
		defines			{
							'VT_GPU_API_MODULE=Vulkan',
							'VT_GPU_API_NAME=vulkan',
							'VT_SHADER_EXTENSION="spv"',
						}

	filter 'platforms:D3D12+Vulkan'
		defines			{
							'VT_DYNAMIC_GPU_API',
							'VT_GPU_API_MODULE=D3D12',
							'VT_GPU_API_NAME=d3d12',
							'VT_GPU_API_MODULE_SECONDARY=Vulkan',
							'VT_GPU_API_NAME_SECONDARY=vulkan',
						}

group 'Dependencies'

	project 'D3D12MemoryAllocator'
		location		'Dependencies'
		kind			'StaticLib'
		files			{
							'**/%{prj.name}/**/D3D12MemAlloc.cpp',
							'**/%{prj.name}/**/D3D12MemAlloc.h',
							'**.natvis',
						}
		warnings		'Off'