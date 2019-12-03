#include "GeometrySubpass.h"
#include "FrameBuffer.h"
#include "CommandBuffer.h"
#include "PhyscialDevice.h"
#include "LogicalDevice.h"
#include "GraphicsHelpers.h"
#include "MeshRenderer.h"
#include "SwapChain.h"

#define FRAME_BUF_DIM 2048

namespace Fling
{
	GeometrySubpass::GeometrySubpass(const LogicalDevice* t_Dev, const Swapchain* t_Swap, std::shared_ptr<Fling::Shader> t_Vert, std::shared_ptr<Fling::Shader> t_Frag)
		: Subpass(t_Dev, t_Swap, t_Vert, t_Frag)
	{

	}

	GeometrySubpass::~GeometrySubpass()
	{
		// Clean up any allocated UBO's 
		// Clean up any allocated descriptor sets
	}

	void GeometrySubpass::Draw(CommandBuffer& t_CmdBuf, FrameBuffer& t_FrameBuf, entt::registry& t_reg)
	{
		// Begin render pass

		// vkCmdBindPipeline
		// Update UBO's 
		t_reg.view<MeshRenderer>().each([&](MeshRenderer& t_MeshRend)
		{
			Fling::Model* Model = t_MeshRend.m_Model;
			if (!Model)
			{
				return;
			}

			// UPDATE UNIFORM BUF of the mesh --------

			VkBuffer vertexBuffers[1] = { Model->GetVertexBuffer()->GetVkBuffer() };
			VkDeviceSize offsets[1] = { 0 };

			// Bind the descriptor set for rendering a mesh using the dynamic offset
			//vkCmdBindDescriptorSets(
			//	t_CmdBuf.GetHandle(),
			//	VK_PIPELINE_BIND_POINT_GRAPHICS,
			//	m_PipelineLayout,
			//	0,
			//	1,
			//	&t_MeshRend.m_DescriptorSets[0],	// #TODO Descriptor set per cmd buf
			//	0,
			//	nullptr);

			vkCmdBindVertexBuffers(t_CmdBuf.GetHandle(), 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(t_CmdBuf.GetHandle(), Model->GetIndexBuffer()->GetVkBuffer(), 0, Model->GetIndexType());
			vkCmdDrawIndexed(t_CmdBuf.GetHandle(), Model->GetIndexCount(), 1, 0, 0, 0);
		});

		// Cmd to bind frame buffer attachments
		// for each mesh renderer
			// If valid model
			// Update uniform buffer
			// bind descriptor set
			// bind vertex buffers
			// bind index buffer
			// mdDrawIndexed
		// vkCmdDrawIndexed


		// Binds UBO cmd

		// End the render pass
	}

	void GeometrySubpass::CreateDescriptorSets(VkDescriptorPool t_Pool, FrameBuffer& t_FrameBuf, entt::registry& t_reg)
	{
		t_reg.view<MeshRenderer>().each([&](MeshRenderer& t_MeshRend)
		{
			// Textured quad descriptor set
			VkDescriptorSetAllocateInfo allocInfo =
				Initializers::DescriptorSetAllocateInfo(
					t_Pool,
					&m_DescriptorLayout,
					1);

			VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device->GetVkDevice(), &allocInfo, &t_MeshRend.m_DescriptorSets[0]));
		
			// Create the image info's for the write sets to reference
			// that will give us access to the G-Buffer in the shaders
			VkDescriptorImageInfo texDescriptorPosition =
				Initializers::DescriptorImageInfo(
					t_FrameBuf.GetSamplerHandle(),
					t_FrameBuf.GetAttachmentAtIndex(0)->GetViewHandle(),
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			VkDescriptorImageInfo texDescriptorNormal =
				Initializers::DescriptorImageInfo(
					t_FrameBuf.GetSamplerHandle(),
					t_FrameBuf.GetAttachmentAtIndex(1)->GetViewHandle(),
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			VkDescriptorImageInfo texDescriptorAlbedo =
				Initializers::DescriptorImageInfo(
					t_FrameBuf.GetSamplerHandle(),
					t_FrameBuf.GetAttachmentAtIndex(2)->GetViewHandle(),
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			std::vector<VkWriteDescriptorSet> writeDescriptorSets =
			{
				// Uniform buffer for the mesh

				// Image descriptor of the frame buffer sampler, and attachment 0's view
				
				// Attachment 0: (World space) Positions
				/*Initializers::WriteDescriptorSetImage(
					t_MeshRend.m_DescriptorSets[0],
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					1,
					&texDescriptorPosition),*/
				// Attachment 1: (World space) Normals
				// Attachment 2: Albedo (color)
			};

			vkUpdateDescriptorSets(m_Device->GetVkDevice(), static_cast<UINT32>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);	
		});
		
		// For each mesh renderer
		// Bind
	}

	void GeometrySubpass::PrepareAttachments(FrameBuffer& t_FrameBuffer)
	{
		AttachmentCreateInfo attachmentInfo = {};

		// Four attachments (3 color, 1 depth)
		attachmentInfo.Width = FRAME_BUF_DIM;
		attachmentInfo.Height = FRAME_BUF_DIM;
		attachmentInfo.LayerCount = 1;
		attachmentInfo.Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		// Color attachments
		// Attachment 0: (World space) Positions
		attachmentInfo.Format = VK_FORMAT_R16G16B16A16_SFLOAT;
		t_FrameBuffer.AddAttachment(attachmentInfo);

		// Attachment 1: (World space) Normals
		attachmentInfo.Format = VK_FORMAT_R16G16B16A16_SFLOAT;
		t_FrameBuffer.AddAttachment(attachmentInfo);

		// Attachment 2: Albedo (color)
		attachmentInfo.Format = VK_FORMAT_R8G8B8A8_UNORM;
		t_FrameBuffer.AddAttachment(attachmentInfo);

		// Depth attachment
		// Find a suitable depth format
		VkFormat attDepthFormat;
		const PhysicalDevice* PhysDevice = m_Device->GetPhysicalDevice();
		assert(PhysDevice);

		VkBool32 validDepthFormat = PhysDevice->GetSupportedDepthFormat(&attDepthFormat);
		assert(validDepthFormat);

		attachmentInfo.Format = attDepthFormat;
		attachmentInfo.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		t_FrameBuffer.AddAttachment(attachmentInfo);

		// Create sampler to sample from the color attachments
		VK_CHECK_RESULT(t_FrameBuffer.CreateSampler(VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE));
	}

	void GeometrySubpass::CreateGraphicsPipeline(FrameBuffer& t_FrameBuffer)
	{
		VkRenderPass RenderPass = t_FrameBuffer.GetRenderPassHandle();
		assert(RenderPass != VK_NULL_HANDLE);

		// Change anything about the graphics pipeline here
		// Add color blend states
		m_GraphicsPipeline->m_RasterizationState = 
			Initializers::PipelineRasterizationStateCreateInfo(
				VK_POLYGON_MODE_FILL,
				VK_CULL_MODE_BACK_BIT,
				VK_FRONT_FACE_CLOCKWISE,
				0);

		// Color Attachment
		m_GraphicsPipeline->m_ColorBlendAttachmentStates[0] = 
			Initializers::PipelineColorBlendAttachmentState(
				0xf,
				VK_FALSE);

		m_GraphicsPipeline->m_ColorBlendState =
			Initializers::PipelineColorBlendStateCreateInfo(
				1,
				&m_GraphicsPipeline->m_ColorBlendAttachmentStates[0]);

		m_GraphicsPipeline->m_DepthStencilState =
			Initializers::PipelineDepthStencilStateCreateInfo(
				VK_TRUE,
				VK_TRUE,
				VK_COMPARE_OP_LESS_OR_EQUAL);

		m_GraphicsPipeline->m_ViewportState =
			Initializers::PipelineViewportStateCreateInfo(1, 1, 0);

		m_GraphicsPipeline->m_MultisampleState =
			Initializers::PipelineMultiSampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
		
		std::vector<VkDynamicState> dynamicStateEnables = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		m_GraphicsPipeline->m_DynamicState =
			Initializers::PipelineDynamicStateCreateInfo(
				dynamicStateEnables.data(), 
				dynamicStateEnables.size(), 
				0);

		m_GraphicsPipeline->CreateGraphicsPipeline(RenderPass, nullptr);
	}

	void GeometrySubpass::OnMeshRendererAdded(entt::entity t_Ent, entt::registry& t_Reg, MeshRenderer& t_MeshRend)
	{

	}
}   // namespace Fling