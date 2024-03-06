#include "scene.h"

namespace hfLib {
	// Scene Asset
	SceneAsset::SceneAsset(ew::Model model, ew::Transform transform)
	{
		this->model = model;
		this->transform = transform;
	}

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

	ew::Model SceneAsset::getModel()
	{
		return model;
	}

	ew::Transform SceneAsset::getTransform()
	{
		return transform;
	}

	std::vector<GLuint> SceneAsset::getTextures()
	{
		return textures;
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
		//shader.use();

		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		shader.setMat4("_Model", glm::mat4(1.0f));

		for (auto asset : assets)
		{
			glBindTextureUnit(0, asset.getTextures()[0]);
			shader.setInt("_MainTex", 0);
			glBindTextureUnit(1, asset.getTextures()[1]);
			shader.setInt("_NormalTex", 1);

			shader.setMat4("_Model", asset.getTransform().modelMatrix());

			asset.getModel().draw();
		}

		//Binding Textures
		//Setting View Projection
		//Setting Model Matrix
		//Mesh.draw() calls
	}

	SceneAsset Scene::getAsset(int index)
	{
		return assets[index];
	}
}