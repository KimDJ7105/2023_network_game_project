#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"

CSceneManager::CSceneManager()
{
	if (Define::SceneManager == nullptr) Define::SceneManager = this;
}

void CSceneManager::LoadScene(int index)
{
	currentScene = SceneList[index];
	currentScene->LoadScene();
}

void CSceneManager::AddScene(CScene* scene, int index)
{
	SceneList[index] = scene;
}