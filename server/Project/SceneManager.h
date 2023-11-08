#pragma once

class CScene;

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager() {}

public:
	CScene* GetCurrentScene() { return currentScene; }
	void LoadScene(int index);
	void AddScene(CScene* scene, int index);

private:
	CScene* currentScene;
	map<int, CScene*> SceneList;
}; 