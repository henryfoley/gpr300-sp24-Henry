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
#include "../hfLib/node.h"

namespace hfLib {
	class SceneAsset
	{
	public:
		SceneAsset(ew::Model model, ew::Transform transform);
		SceneAsset(ew::Model model, ew::Transform transform, std::vector<GLuint> textures);
		SceneAsset(ew::Model model, Node* transform, std::vector<GLuint> textures);
		~SceneAsset();
		void addModel(ew::Model model);
		void addTransform(ew::Transform transform);
		void addTexture(GLuint texture);
		void setPos(glm::vec3 pos);
		void setRot(glm::quat rot);
		void setScale(glm::vec3 scale);
		ew::Model getModel();
		glm::mat4 getModelMatrix();
		std::vector<GLuint> getTextures();

	private:
		bool nodeTransform = false;
		ew::Model model;
		ew::Transform transform;
		std::vector<GLuint> textures;
		Node* node;
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
			void setAssetPos(int index, glm::vec3 pos);
			void setAssetRot(int index, glm::quat rot);
			void setAssetScale(int index, glm::vec3 scale);
			void draw(ew::Shader shader, ew::Camera camera);
			SceneAsset getAsset(int index);
	};

}

