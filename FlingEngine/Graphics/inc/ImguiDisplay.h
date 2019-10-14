#pragma once

#include <imgui.h>
#include <array>

namespace Fling
{
	class ImguiDisplay
	{
	public:
		ImguiDisplay();
		~ImguiDisplay() {};

		/**
		 * @brief User defined callback to make or change imgui UI
		 * 
		 */
		void NewFrame();

	private:
		std::array<float, 50> fpsGraph{};
		float frameTimeMin = 9999.0f, frameTimeMax = 0.0f;
	};
}