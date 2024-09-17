// minimalistic code to draw a single triangle, this is not part of the API.
#include "shaderc/shaderc.h" // needed for compiling shaders at runtime
#ifdef _WIN32 // must use MT platform DLL libraries on windows
#pragma comment(lib, "shaderc_combined.lib") 
#endif

void PrintLabeledDebugString(const char* label, const char* toPrint)
{
	std::cout << label << toPrint << std::endl;

	//OutputDebugStringA is a windows-only function 
#if defined WIN32 
	OutputDebugStringA(label);
	OutputDebugStringA(toPrint);
#endif
}

class Renderer
{

	struct Vertex
	{
		float x, y, z, w;
	};
	// proxy handles
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GVulkanSurface vlk;
	VkRenderPass renderPass;
	GW::CORE::GEventReceiver shutdown;

	// what we need at a minimum to draw a triangle
	VkDevice device = nullptr;
	VkPhysicalDevice physicalDevice = nullptr;
	VkBuffer vertexHandle = nullptr;
	VkDeviceMemory vertexData = nullptr;
	VkShaderModule vertexShader = nullptr;
	VkShaderModule fragmentShader = nullptr;
	VkPipeline pipeline = nullptr;
	VkPipelineLayout pipelineLayout = nullptr;

	unsigned int windowWidth, windowHeight;

	// TODO: Part 1c

	// TODO: Part 2a
	GW::MATH::GMatrix math;
	GW::MATH::GMATRIXF worldMatrix;

	// TODO: Part 2b
	struct SHADER_VARS
	{
		GW::MATH::GMATRIXF worldMatrix;
		float padding[44]; // Pad to 128 bytes (16 floats in GMATRIXF + 44 floats = 60 floats = 240 bytes)
	};
	// TODO: Part 3a
	// TODO: Part 3f 
// TODO: Part 2c // TODO: Part 4y
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	// TODO: Part 2e
	VkDescriptorSetLayout descriptorSetLayout;
	// TODO: Part 2f
	VkDescriptorPool descriptorPool;
	// TODO: Part 2g
	VkDescriptorSet descriptorSet;
	// TODO: Part 3c
	// TODO: Part 3d
	// TODO: Part 4a

public:
	Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GVulkanSurface _vlk)
	{
		win = _win;
		vlk = _vlk;

		// TODO: Part 2a
		math.Create();
		InitializeWorldMatrix();

		// TODO: Part 2e
		// TODO: Part 3a
		// TODO: Part 3c
		// TODO: Part 3d
		// TODO: Part 4a

		UpdateWindowDimensions();
		InitializeGraphics();
		BindShutdownCallback();
	}

private:




#include <cmath>
#include <corecrt_math_defines.h>
	// part 2a 
	void InitializeWorldMatrix()
	{
		GW::MATH::GMATRIXF   rotationMatrix, translatedMatrix;

		// Start with an identity matrix
		math.IdentityF(worldMatrix);  // or IdentityD if needed for GMATRIXD

		// Convert 90 degrees to radians for double precision
		float rotationAngle = 90.0 * (M_PI / 180.0);

		// Rotate the matrix 90 degrees around the X axis (local rotation)
		math.RotateXLocalF(worldMatrix, rotationAngle, rotationMatrix);

		// Create a translation vector for Y axis translation (-0.5 units)
		GW::MATH::GVECTORF translationVector = { 0.0, -0.5, 0.0, 0.0 };

		// Apply the translation to the rotated matrix
		math.TranslateLocalF(rotationMatrix, translationVector, translatedMatrix);

		// Store the final result in worldMatrix
		worldMatrix = translatedMatrix;
	}



	// part 2c self 
	void CreateUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(SHADER_VARS);

		unsigned int imageCount;
		if (vlk.GetSwapchainImageCount(imageCount) != GW::GReturn::SUCCESS) {
			throw std::runtime_error("Failed to get swapchain image count");
		}

		uniformBuffers.resize(imageCount);
		uniformBuffersMemory.resize(imageCount);

		for (size_t i = 0; i < imageCount; i++)
		{
			if (GvkHelper::create_buffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				&uniformBuffers[i], &uniformBuffersMemory[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create uniform buffer");
			}
		}
	}
	
	void UpdateWindowDimensions()
	{
		win.GetClientWidth(windowWidth);
		win.GetClientHeight(windowHeight);
	}
	//part 2e self
	void CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = 0;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &layoutBinding;

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout!");
		}
	}
	// 2f self 
	void CreateDescriptorPool()
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = 1;

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool!");
		}
	}
	// 2g self 
	void CreateDescriptorSet()
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;

		if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate descriptor set!");
		}
	}
	// 2h self 
	void UpdateDescriptorSet()
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[0];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(SHADER_VARS);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}

	void GetHandlesFromSurface()
	{
		if (vlk.GetDevice((void**)&device) != GW::GReturn::SUCCESS ||
			vlk.GetPhysicalDevice((void**)&physicalDevice) != GW::GReturn::SUCCESS) {
			throw std::runtime_error("Failed to get Vulkan device handles");
		}
		vlk.GetPhysicalDevice((void**)&physicalDevice);
		vlk.GetRenderPass((void**)&renderPass);
	}
	
	void InitializeVertexBuffer()
	{
		const int gridDensity = 25;
		const float gridSpacing = 1.0f / gridDensity;
		const int totalLines = (gridDensity + 1) * 2; // Horizontal and vertical lines
		const int verticesPerLine = 2;
		const int totalVertices = totalLines * verticesPerLine;

		std::vector<Vertex> verts(totalVertices);
		int vertexIndex = 0;

		// Create horizontal lines
		for (int i = 0; i <= gridDensity; ++i)
		{
			float y = i * gridSpacing - 0.5f;
			verts[vertexIndex++] = { -0.5f, y, 0.0f, 1.0f };
			verts[vertexIndex++] = { 0.5f, y, 0.0f, 1.0f };
		}

		// Create vertical lines
		for (int i = 0; i <= gridDensity; ++i)
		{
			float x = i * gridSpacing - 0.5f;
			verts[vertexIndex++] = { x, -0.5f, 0.0f, 1.0f };
			verts[vertexIndex++] = { x, 0.5f, 0.0f, 1.0f };
		}

		CreateVertexBuffer(verts.data(), sizeof(Vertex) * totalVertices);
	}

	void InitializeGraphics()
	{
		GetHandlesFromSurface();
		InitializeVertexBuffer();
		// TODO: Part 2d
		CreateUniformBuffers();
		// 2e self
		CreateDescriptorSetLayout();

		// TODO: Part 2f // TODO: Part 4y
		CreateDescriptorPool();
		// TODO: Part 2g // TODO: Part 4y
		CreateDescriptorSet();
		// TODO: Part 2h // TODO: Part 4y 
		UpdateDescriptorSet();

		CompileShaders();
		InitializeGraphicsPipeline();
	}

	void UpdateUniformBuffer(uint32_t currentImage) {
		SHADER_VARS shaderVars;
		shaderVars.worldMatrix = worldMatrix;

		void* data;
		vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(shaderVars), 0, &data);
		memcpy(data, &shaderVars, sizeof(shaderVars));
		vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
	}


	void CreateVertexBuffer(const void* data, unsigned int sizeInBytes)
	{
		GvkHelper::create_buffer(physicalDevice, device, sizeInBytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&vertexHandle, &vertexData);
		GvkHelper::write_to_buffer(device, vertexData, data, sizeInBytes); // Transfer triangle data to the vertex buffer. (staging would be prefered here)
	}

	void CompileShaders()
	{
		// Intialize runtime shader compiler HLSL -> SPIRV
		shaderc_compiler_t compiler = shaderc_compiler_initialize();
		shaderc_compile_options_t options = CreateCompileOptions();

		CompileVertexShader(compiler, options);
		CompileFragmentShader(compiler, options);

		// Free runtime shader compiler resources
		shaderc_compile_options_release(options);
		shaderc_compiler_release(compiler);
	}

	shaderc_compile_options_t CreateCompileOptions()
	{
		shaderc_compile_options_t retval = shaderc_compile_options_initialize();
		shaderc_compile_options_set_source_language(retval, shaderc_source_language_hlsl);
		shaderc_compile_options_set_invert_y(retval, true);	// TODO: Part 3e
#ifndef NDEBUG
		shaderc_compile_options_set_generate_debug_info(retval);
#endif
		return retval;
	}

	void CompileVertexShader(const shaderc_compiler_t& compiler, const shaderc_compile_options_t& options)
	{
		std::string vertexShaderSource = ReadFileIntoString("../VertexShader.hlsl");

		shaderc_compilation_result_t result = shaderc_compile_into_spv( // compile
			compiler, vertexShaderSource.c_str(), vertexShaderSource.length(),
			shaderc_vertex_shader, "main.vert", "main", options);

		if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
		{
			PrintLabeledDebugString("Vertex Shader Errors: \n", shaderc_result_get_error_message(result));
			abort(); //Vertex shader failed to compile! 
			return;
		}

		GvkHelper::create_shader_module(device, shaderc_result_get_length(result), // load into Vulkan
			(char*)shaderc_result_get_bytes(result), &vertexShader);

		shaderc_result_release(result); // done
	}

	void CompileFragmentShader(const shaderc_compiler_t& compiler, const shaderc_compile_options_t& options)
	{
		std::string fragmentShaderSource = ReadFileIntoString("../FragmentShader.hlsl");

		shaderc_compilation_result_t result = shaderc_compile_into_spv( // compile
			compiler, fragmentShaderSource.c_str(), fragmentShaderSource.length(),
			shaderc_fragment_shader, "main.frag", "main", options);

		if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
		{
			PrintLabeledDebugString("Fragment Shader Errors: \n", shaderc_result_get_error_message(result));
			abort(); //Fragment shader failed to compile! 
			return;
		}

		GvkHelper::create_shader_module(device, shaderc_result_get_length(result), // load into Vulkan
			(char*)shaderc_result_get_bytes(result), &fragmentShader);

		shaderc_result_release(result); // done
	}

	// Create Pipeline & Layout (Thanks Tiny!)
	void InitializeGraphicsPipeline()
	{
		VkPipelineShaderStageCreateInfo stage_create_info[2] = {};

		// Create Stage Info for Vertex Shader
		stage_create_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stage_create_info[0].module = vertexShader;
		stage_create_info[0].pName = "main";

		// Create Stage Info for Fragment Shader
		stage_create_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stage_create_info[1].module = fragmentShader;
		stage_create_info[1].pName = "main";


		VkPipelineInputAssemblyStateCreateInfo assembly_create_info = CreateVkPipelineInputAssemblyStateCreateInfo();
		VkVertexInputBindingDescription vertex_binding_description = CreateVkVertexInputBindingDescription();

		// TODO: Part 1c
		VkVertexInputAttributeDescription vertex_attribute_descriptions[1];
		vertex_attribute_descriptions[0].binding = 0;
		vertex_attribute_descriptions[0].location = 0;
		vertex_attribute_descriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertex_attribute_descriptions[0].offset = 0;

		VkPipelineVertexInputStateCreateInfo input_vertex_info = CreateVkPipelineVertexInputStateCreateInfo(&vertex_binding_description, 1, vertex_attribute_descriptions, 1);
		VkViewport viewport = CreateViewportFromWindowDimensions();
		VkRect2D scissor = CreateScissorFromWindowDimensions();
		VkPipelineViewportStateCreateInfo viewport_create_info = CreateVkPipelineViewportStateCreateInfo(&viewport, 1, &scissor, 1);
		VkPipelineRasterizationStateCreateInfo rasterization_create_info = CreateVkPipelineRasterizationStateCreateInfo();
		VkPipelineMultisampleStateCreateInfo multisample_create_info = CreateVkPipelineMultisampleStateCreateInfo();
		VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = CreateVkPipelineDepthStencilStateCreateInfo();
		VkPipelineColorBlendAttachmentState color_blend_attachment_state = CreateVkPipelineColorBlendAttachmentState();
		VkPipelineColorBlendStateCreateInfo color_blend_create_info = CreateVkPipelineColorBlendStateCreateInfo(&color_blend_attachment_state, 1);

		VkDynamicState dynamic_states[2] =
		{
			// By setting these we do not need to re-create the pipeline on Resize
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamic_create_info = CreateVkPipelineDynamicStateCreateInfo(dynamic_states, 2);

		CreatePipelineLayout();

		// Pipeline State... (FINALLY) 
		VkGraphicsPipelineCreateInfo pipeline_create_info = {};
		pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_create_info.stageCount = 2;
		pipeline_create_info.pStages = stage_create_info;
		pipeline_create_info.pInputAssemblyState = &assembly_create_info;
		pipeline_create_info.pVertexInputState = &input_vertex_info;
		pipeline_create_info.pViewportState = &viewport_create_info;
		pipeline_create_info.pRasterizationState = &rasterization_create_info;
		pipeline_create_info.pMultisampleState = &multisample_create_info;
		pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
		pipeline_create_info.pColorBlendState = &color_blend_create_info;
		pipeline_create_info.pDynamicState = &dynamic_create_info;
		pipeline_create_info.layout = pipelineLayout;
		pipeline_create_info.renderPass = renderPass;
		pipeline_create_info.subpass = 0;
		pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

		vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline);
	}

	VkPipelineShaderStageCreateInfo CreateVertexShaderStageCreateInfo()
	{
		VkPipelineShaderStageCreateInfo retval;
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		retval.stage = VK_SHADER_STAGE_VERTEX_BIT;
		retval.module = vertexShader;
		retval.pName = "main";
		return retval;
	}

	VkPipelineInputAssemblyStateCreateInfo CreateVkPipelineInputAssemblyStateCreateInfo()
	{
		VkPipelineInputAssemblyStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		retval.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; // TODO: Part 1b  // changed to VK_PRIMITIVE_TOPOLOGY_LINE_LIST
		retval.primitiveRestartEnable = false;
		return retval;
	}

	VkVertexInputBindingDescription CreateVkVertexInputBindingDescription()
	{
		VkVertexInputBindingDescription retval = {};
		retval.binding = 0;
		retval.stride = sizeof(Vertex); 	//TODO: Part 1c
		retval.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return retval;
	}

	VkPipelineVertexInputStateCreateInfo CreateVkPipelineVertexInputStateCreateInfo(
		VkVertexInputBindingDescription* inputBindingDescriptions, unsigned int bindingCount,
		VkVertexInputAttributeDescription* vertexAttributeDescriptions, unsigned int attributeCount)
	{
		VkPipelineVertexInputStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		retval.vertexBindingDescriptionCount = bindingCount;
		retval.pVertexBindingDescriptions = inputBindingDescriptions;
		retval.vertexAttributeDescriptionCount = attributeCount;
		retval.pVertexAttributeDescriptions = vertexAttributeDescriptions;
		return retval;
	}

	VkViewport CreateViewportFromWindowDimensions()
	{
		VkViewport retval = {};
		retval.x = 0;
		retval.y = 0;
		retval.width = static_cast<float>(windowWidth);
		retval.height = static_cast<float>(windowHeight);
		retval.minDepth = 0;
		retval.maxDepth = 1;
		return retval;
	}

	VkRect2D CreateScissorFromWindowDimensions()
	{
		VkRect2D retval = {};
		retval.offset.x = 0;
		retval.offset.y = 0;
		retval.extent.width = windowWidth;
		retval.extent.height = windowHeight;
		return retval;
	}

	VkPipelineViewportStateCreateInfo CreateVkPipelineViewportStateCreateInfo(VkViewport* viewports, unsigned int viewportCount, VkRect2D* scissors, unsigned int scissorCount)
	{
		VkPipelineViewportStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		retval.viewportCount = viewportCount;
		retval.pViewports = viewports;
		retval.scissorCount = scissorCount;
		retval.pScissors = scissors;
		return retval;
	}

	VkPipelineRasterizationStateCreateInfo CreateVkPipelineRasterizationStateCreateInfo()
	{
		VkPipelineRasterizationStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		retval.rasterizerDiscardEnable = VK_FALSE;
		retval.polygonMode = VK_POLYGON_MODE_FILL;
		retval.lineWidth = 1.0f;
		retval.cullMode = VK_CULL_MODE_BACK_BIT;
		retval.frontFace = VK_FRONT_FACE_CLOCKWISE;
		retval.depthClampEnable = VK_FALSE;
		retval.depthBiasEnable = VK_FALSE;
		retval.depthBiasClamp = 0.0f;
		retval.depthBiasConstantFactor = 0.0f;
		retval.depthBiasSlopeFactor = 0.0f;
		return retval;
	}

	VkPipelineMultisampleStateCreateInfo CreateVkPipelineMultisampleStateCreateInfo()
	{
		VkPipelineMultisampleStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		retval.sampleShadingEnable = VK_FALSE;
		retval.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		retval.minSampleShading = 1.0f;
		retval.pSampleMask = VK_NULL_HANDLE;
		retval.alphaToCoverageEnable = VK_FALSE;
		retval.alphaToOneEnable = VK_FALSE;
		return retval;
	}

	VkPipelineDepthStencilStateCreateInfo CreateVkPipelineDepthStencilStateCreateInfo()
	{
		VkPipelineDepthStencilStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		retval.depthTestEnable = VK_TRUE;
		retval.depthWriteEnable = VK_TRUE;
		retval.depthCompareOp = VK_COMPARE_OP_LESS;
		retval.depthBoundsTestEnable = VK_FALSE;
		retval.minDepthBounds = 0.0f;
		retval.maxDepthBounds = 1.0f;
		retval.stencilTestEnable = VK_FALSE;
		return retval;
	}

	VkPipelineColorBlendAttachmentState CreateVkPipelineColorBlendAttachmentState()
	{
		VkPipelineColorBlendAttachmentState retval = {};
		retval.colorWriteMask = 0xF;
		retval.blendEnable = VK_FALSE;
		retval.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
		retval.dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
		retval.colorBlendOp = VK_BLEND_OP_ADD;
		retval.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		retval.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
		retval.alphaBlendOp = VK_BLEND_OP_ADD;
		return retval;
	}

	VkPipelineColorBlendStateCreateInfo CreateVkPipelineColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState* attachments, unsigned int attachmentCount)
	{
		VkPipelineColorBlendStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		retval.logicOpEnable = VK_FALSE;
		retval.logicOp = VK_LOGIC_OP_COPY;
		retval.attachmentCount = attachmentCount;
		retval.pAttachments = attachments;
		retval.blendConstants[0] = 0.0f;
		retval.blendConstants[1] = 0.0f;
		retval.blendConstants[2] = 0.0f;
		retval.blendConstants[3] = 0.0f;
		return retval;
	}

	VkPipelineDynamicStateCreateInfo CreateVkPipelineDynamicStateCreateInfo(VkDynamicState* dynamicStates, unsigned int dynamicStateCount)
	{
		VkPipelineDynamicStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		retval.dynamicStateCount = dynamicStateCount;
		retval.pDynamicStates = dynamicStates;
		return retval;
	}

	void CreatePipelineLayout()
	{
		// TODO: Part 2e

		VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = 1; // TODO: Part 2e
		pipeline_layout_create_info.pSetLayouts = &descriptorSetLayout; // TODO: Part 2e
		pipeline_layout_create_info.pushConstantRangeCount = 0;
		pipeline_layout_create_info.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void BindShutdownCallback()
	{
		// GVulkanSurface will inform us when to release any allocated resources
		shutdown.Create(vlk, [&]() {
			if (+shutdown.Find(GW::GRAPHICS::GVulkanSurface::Events::RELEASE_RESOURCES, true)) {
				CleanUp(); // unlike D3D we must be careful about destroy timing
			}
			});
	}


public:
	void Render()
	{
		uint32_t currentImageIndex;
		if (vlk.GetSwapchainCurrentImage(currentImageIndex) != GW::GReturn::SUCCESS) {
			throw std::runtime_error("Failed to get current swapchain image index");
		}
		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
		// TODO: Part 4x
		SetUpPipeline(commandBuffer);

		// TODO: Part 2b
		SHADER_VARS shaderVars;
		shaderVars.worldMatrix = worldMatrix;

		void* data;
		vkMapMemory(device, uniformBuffersMemory[currentImageIndex], 0, sizeof(shaderVars), 0, &data); 
		memcpy(data, &shaderVars, sizeof(shaderVars));  // Copy the updated data into the uniform buffer
		vkUnmapMemory(device, uniformBuffersMemory[currentImageIndex]); 

		// TODO: Part 2i // TODO: Part 4y
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
       
// TODO: Part 3g
		const int gridDensity = 25;
		const int totalLines = (gridDensity + 1) * 2;
		const int verticesPerLine = 2;
		const int totalVertices = totalLines * verticesPerLine;

		vkCmdDraw(commandBuffer, totalVertices, 1, 0, 0); // TODO: Part 1b		// changed to 6 points 
	}

	// TODO: Part 4b
	// TODO: Part 4c
	// TODO: Part 4d
	// TODO: Part 4e
	// TODO: Part 4f
	// TODO: Part 4g

private:
	VkCommandBuffer GetCurrentCommandBuffer()
	{
		unsigned int currentBuffer;
		vlk.GetSwapchainCurrentImage(currentBuffer);

		VkCommandBuffer commandBuffer;
		vlk.GetCommandBuffer(currentBuffer, (void**)&commandBuffer);
		return commandBuffer;
	}

	void SetUpPipeline(VkCommandBuffer& commandBuffer)
	{
		UpdateWindowDimensions();
		SetViewport(commandBuffer);
		SetScissor(commandBuffer);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		BindVertexBuffers(commandBuffer);
	}

	void SetViewport(const VkCommandBuffer& commandBuffer)
	{
		VkViewport viewport = CreateViewportFromWindowDimensions();
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	void SetScissor(const VkCommandBuffer& commandBuffer)
	{
		VkRect2D scissor = CreateScissorFromWindowDimensions();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void BindVertexBuffers(VkCommandBuffer& commandBuffer)
	{
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexHandle, offsets);
	}


	//Cleanup callback function (passed to VKSurface, will be called when the pipeline shuts down)
	void CleanUp()
	{
		// wait till everything has completed
		vkDeviceWaitIdle(device);

		// Release allocated buffers, shaders & pipeline
		vkDestroyBuffer(device, vertexHandle, nullptr);
		vkFreeMemory(device, vertexData, nullptr);
		// TODO: Part 2d
		for (size_t i = 0; i < uniformBuffers.size(); i++)
		{
			vkDestroyBuffer(device, uniformBuffers[i], nullptr);
			vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
		}

		// 2e 
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		// TODO: Part 2f
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		vkDestroyShaderModule(device, vertexShader, nullptr);
		vkDestroyShaderModule(device, fragmentShader, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyPipeline(device, pipeline, nullptr);


	}
};
