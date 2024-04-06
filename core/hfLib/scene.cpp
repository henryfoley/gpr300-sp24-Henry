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

	SceneAsset::SceneAsset(ew::Model model, Node* transform, std::vector<GLuint> textures)
	{
		this->model = model;
		this->nodeTransform = true;
		this->node = transform;
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

	void SceneAsset::setPos(glm::vec3 pos)
	{
		transform.position = pos;
	}

	void SceneAsset::setRot(glm::quat rot)
	{
		transform.rotation = rot;
	}

	void SceneAsset::setScale(glm::vec3 scale)
	{
		transform.scale = scale;
	}

	ew::Model SceneAsset::getModel()
	{
		return model;
	}

	glm::mat4 SceneAsset::getModelMatrix()
	{
		if (nodeTransform){
			return node->globalTransform;
		}
		else {
			return transform.modelMatrix();
		}

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

	void Scene::setAssetPos(int index, glm::vec3 pos)
	{
		assets[index].setPos(pos);
	}

	void Scene::setAssetRot(int index, glm::quat rot)
	{
		assets[index].setRot(rot);
	}

	void Scene::setAssetScale(int index, glm::vec3 scale)
	{
		assets[index].setScale(scale);
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

			shader.setMat4("_Model", asset.getModelMatrix());

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