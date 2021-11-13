output_dir = '%{cfg.buildcfg}_%{cfg.architecture}_%{cfg.system}'

workspace 'Vitro'
	startproject		'Vitro'
	architecture		'x64'
	configurations		{ 'Debug', 'Development', 'Release' }
	platforms			{ 'D3D12', 'Vulkan', 'D3D12+Vulkan' }
	flags				'MultiProcessorCompile'
	language			'C++'
	cppdialect			'C++latest'
	cdialect			'C11'
	conformancemode		'On'
	warnings			'Extra'
	disablewarnings		'4201' -- anonymous structs
	floatingpoint		'Fast'
	toolset				'MSC'
	staticruntime		'On'
	files				{
							'%{prj.name}/**.cpp',
							'%{prj.name}/**.hpp',
							'%{prj.name}/**.hlsl',
							'%{prj.name}/**.hlsli',
						}
	removefiles			'**/Platform**/'
	debugdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	targetdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	objdir				('.bin_int/' .. output_dir .. '/%{prj.name}')

	filter 'files:**.cpp'
		compileas		'Module' -- Change to ModulePartition eventually

	filter 'files:**.hlsl'
		buildmessage	'Compiling shader %{file.relpath}'
		buildcommands	('..\\.bin\\' .. output_dir .. '\\VitroHlslBuilder\\VitroHlslBuilder %{file.abspath} --api=%{cfg.platform} --out=%{cfg.targetdir} --sm=5_1')

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

	filter 'system:Windows'
		systemversion	'latest'
		files			'%{prj.name}/**PlatformWindows/*'
		links			'user32'
		defines			{
							'VT_SYSTEM_WINDOWS',
							'VT_SYSTEM_MODULE=Windows',
							'VT_SYSTEM_NAME=windows',
						}

	filter 'platforms:D3D12'
		defines			{
							'VT_GPU_API_MODULE=D3D12',
							'VT_GPU_API_NAME=d3d12',
							'VT_GPU_API_D3D12',
							'VT_SHADER_EXTENSION="cso"',
						}

	filter 'platforms:Vulkan'
		defines			{
							'VT_GPU_API_MODULE=Vulkan',
							'VT_GPU_API_NAME=vulkan',
							'VT_GPU_API_VULKAN',
							'VT_SHADER_EXTENSION="spv"',
						}

	filter 'platforms:D3D12+Vulkan'
		defines			{
							'VT_DYNAMIC_GPU_API',
							'VT_GPU_API_MODULE=D3D12',
							'VT_GPU_API_NAME=d3d12',
							'VT_GPU_API_MODULE_SECONDARY=Vulkan',
							'VT_GPU_API_NAME_SECONDARY=vulkan',
							'VT_GPU_API_D3D12',
							'VT_GPU_API_VULKAN',
						}

	filter { 'files:**.hlsl', 'platforms:D3D12 or D3D12+Vulkan' }
		buildoutputs	'%{cfg.targetdir}/%{file.basename}.cso'

	filter { 'files:**.hlsl', 'platforms:Vulkan or D3D12+Vulkan' }
		buildoutputs	'%{cfg.targetdir}/%{file.basename}.spv'

project 'Vitro'
	location			'%{prj.name}'
	kind				'ConsoleApp'
	includedirs			{ '', 'Dependencies' }
	defines				'VT_ENGINE_NAME="%{prj.name}"'
	targetname			'%{prj.name}%{cfg.platform}'
	links				{
							'VitroCore',
							'VitroHlslBuilder',
							'tinyobjloader',
						}

	filter 'Debug or Development'
		debugargs		{ '--debug-gpu-api' }

	filter 'Release'
		kind			'WindowedApp'
		entrypoint		'mainCRTStartup'

	filter 'platforms:D3D12 or D3D12+Vulkan'
		links			{ 'd3d12', 'dxgi', 'D3D12MemoryAllocator' }
		files			'%{prj.name}/**/PlatformD3D12/*'

	filter 'platforms:Vulkan or D3D12+Vulkan'
		links			'VulkanMemoryAllocator'
		files			'%{prj.name}/**/PlatformVulkan/*'
		includedirs		'C:/VulkanSDK/**/Include'

project 'VitroCore'
	location			'%{prj.name}'
	kind				'StaticLib'
	includedirs			{ '', 'Dependencies' }

project 'VitroHlslBuilder'
	location			'%{prj.name}'
	kind				'ConsoleApp'
	includedirs			{ '' }
	links				'VitroCore'
	
	filter 'platforms:D3D12 or D3D12+Vulkan'
		links			'D3DCompiler'

group 'Dependencies'

deploc		 = 'Dependencies/%{prj.name}'
depsubmoddir = 'Dependencies/%{prj.name}/%{prj.name}'

project 'NatvisFiles'
	location			(deploc)
	kind				'None'
	files				'**.natvis'

project 'tinyobjloader'	
	location			(deploc)
	warnings			'Off'
	kind				'StaticLib'
	files				(depsubmoddir .. '/tiny_obj_loader.cc')

project 'D3D12MemoryAllocator'
	location			(deploc)
	warnings			'Off'
	kind				'None'
	includedirs			(depsubmoddir .. '/include')
	files				(depsubmoddir .. '/src/D3D12MemAlloc.cpp')

	filter 'platforms:D3D12 or D3D12+Vulkan'
		kind			'StaticLib'

project 'VulkanMemoryAllocator'
	location			(deploc)
	warnings			'Off'
	kind				'None'
	files				(deploc .. '/VulkanMemoryAllocator.cpp')
	includedirs			'C:/VulkanSDK/**/Include'

	filter 'platforms:Vulkan or D3D12+Vulkan'
		kind			'StaticLib'