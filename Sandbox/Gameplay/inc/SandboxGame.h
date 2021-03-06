#pragma once

#include "Game.h"

namespace Sandbox
{
	/**
	 * @brief Custom game class that will have control of it's gameplay systems.
	 */
	class Game : public Fling::Game
	{
		/**
		 * @brief 	Called before the first gameplay loop tick.
		 * 			Do any initalization for custom gameplay systems here.
		 */
		void Init(entt::registry& t_Reg) override;

		/* Called when the engine is shutting down */
		void Shutdown(entt::registry& t_Reg) override;

		/**
		* Update is called every frame. Call any system updates for your gameplay systems inside of here
		*/
		void Update(entt::registry& t_Reg, float DeltaTime) override;

		/**
		 * @brief Callback for when the user has given input that shows they want to exit
		 */
		void OnQuitPressed();

		/* Creates a 3D array of meshes */
		void GenerateTestMeshes(entt::registry& t_Reg);

		void PrintFPS() const;

		void LightingTest(entt::registry& t_Reg);

		void ScriptingTest(entt::registry& t_Reg);

		/* Toggles the visibility of the cursor */
		void ToggleCursorVisibility();

		/* Sets window mode of screen, for testing all possible transitions */
		void SetWindowFullscreen();
		void SetWindowBorderlessWindowed();
		void SetWindowWindowed();

	private:

		bool m_DoRotations = false;
		bool m_MovePointLights = false;
		bool m_RunLua = true;

		void ToggleRotation();

		void OnTestSpawn();

		void OnToggleMoveLights();

		void ToggleLua();

		void SetWindowIcon();

		/** Temp vector for keeping track of the movement of things */
		glm::vec3 MoveDelta = {};

	};
}	// namespace Sandbox