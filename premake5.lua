output_dir				= '%{cfg.buildcfg}_%{cfg.architecture}_%{cfg.system}'

workspace 'Vitro'
	startproject		'VitroMain'
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
							'%{prj.name}/**.hh'
						}
	removefiles			{ '%{prj.name}/**/**.*.*' }
	files				{ '%{prj.name}/**.hlsl' } -- Add shaders only after removing files with two dots
	objdir				('.bin_int/' .. output_dir .. '/%{prj.name}')
	targetdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	debugdir			('.bin/'	 .. output_dir .. '/%{prj.name}')

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

	filter 'configurations:Release'
		optimize		'Speed'
		runtime			'Release'

group 'Dependencies'
group ''

project 'VitroEngine'
	location			'%{prj.name}'
	kind				'StaticLib'
	includedirs			{ '%{prj.name}', 'Dependencies' }
	libdirs				'Dependencies'
	defines				'VT_ENGINE_NAME="%{prj.name}"'

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
							'VT_GRAPHICS_API_MODULE=D3D12',
							'VT_GRAPHICS_API_NAME=d3d12'
						}

project 'VitroMain'
	location			'%{prj.name}'
	kind				'ConsoleApp'
	links				'VitroEngine'

	filter 'configurations:Release'
		kind			'WindowedApp'
		entrypoint		'mainCRTStartup'

project 'VitroTests'
	location			'%{prj.name}'
	kind				'SharedLib'
	includedirs			{ 'VitroEngine' }
	links				'VitroEngine'

	filter 'configurations:Release'
		kind			'WindowedApp'
		entrypoint		'mainCRTStartup'