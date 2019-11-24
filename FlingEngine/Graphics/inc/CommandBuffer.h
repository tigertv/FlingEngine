#pragma once

#include "FlingVulkan.h"

namespace Fling
{
	class LogicalDevice;

	// #TODO Pipelnie state class defintion
	// #TODO Resource binding state class defintion

    /**
     * @brief Encapsulates functionality of a Vulkan Command buffer
     * 
     */
    class CommandBuffer
    {
	public:
		CommandBuffer(LogicalDevice* t_Device, VkCommandPool t_CmdPool);
		~CommandBuffer();

		inline VkCommandBuffer GetHandle() const { return m_Handle; }
		inline const LogicalDevice* GetDevice() const { return m_Device; }
		inline const VkCommandPool& GetPoolHandle() const { return m_Pool; }

		void BeginRenderPass();

		void NextSubpass();

		void EndRenderPass();

		/** Stop recording commands to the command buffer */
		void End();

	private:
		const LogicalDevice* m_Device;

		const VkCommandPool m_Pool;

		VkCommandBuffer m_Handle = VK_NULL_HANDLE;

    };

}   // namespace Fling