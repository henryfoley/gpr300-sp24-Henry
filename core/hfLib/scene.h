#pragma once

/*
*	Author: Henry Foley
*/

#include "../ew/external/glad.h"
#include <cstdlib>
#include <vector>
#include <GLFW/glfw3.h>
#include "../ew/model.h"
#include "../ew/shader.h"
#include "../ew/camera.h"
#include "../ew/transform.h"

namespace hfLib {
	class SceneAsset
	{
	public:
		SceneAsset(ew::Model model, ew::Transform transform, std::vector<GLuint> textures);
		~SceneAsset();
		void addModel(ew::Model model);
		void addTransform(ew::Transform transform);
		void addTexture(GLuint texture);
		ew::Model getModel();
		ew::Transform getTransform();
		std::vector<GLuint> getTextures();

	private:
		ew::Model model;
		ew::Transform transform;
		std::vector<GLuint> textures;
	};
	
	class Scene {
		private:
			std::vector<SceneAsset> assets;

		public:
			Scene() {}
			~Scene() {}
			Scene(std::vector<SceneAsset> assets);
			void addAsset(SceneAsset asset);
			void removeAsset(int index);
			void draw(ew::Shader shader, ew::Camera camera);
			SceneAsset getAsset(int index);
	};

}

