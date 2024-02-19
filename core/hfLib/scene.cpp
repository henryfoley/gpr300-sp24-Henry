#include "scene.h"

namespace hfLib {
	// Scene Asset
	SceneAsset::SceneAsset(ew::Model model, ew::Transform transform, std::vector<GLuint> textures)
	{
		this->model = model;
		this->transform = transform;
		this->textures = textures;
	}

	SceneAsset::~SceneAsset()
	{
	}

	void SceneAsset::addModel(ew::Model model)
	{
		this->model = model;
	}

	void SceneAsset::addTransform(ew::Transform transform)
	{
		this->transform = transform;
	}

	void SceneAsset::addTexture(GLuint texture)
	{
		textures.push_back(texture);
	}
	
	// Scene
	Scene::Scene(std::vector<SceneAsset> assets)
	{
		this->assets = assets;
	}

	void Scene::addAsset(SceneAsset asset)
	{
		assets.push_back(asset);
	}
		
	void Scene::removeAsset(int index)
	{
		assets.erase(assets.begin() + index);
	}

	void Scene::draw(ew::Shader shader, ew::Camera camera)
	{
		//Binding Shader
		shader.use();


		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		shader.setMat4("_Model", glm::mat4(1.0f));

		for (auto asset : assets)
		{
			asset.model.draw();
		}

		//Binding Textures
		//Setting View Projection
		//Setting Model Matrix
		//Mesh.draw() calls
	}
}