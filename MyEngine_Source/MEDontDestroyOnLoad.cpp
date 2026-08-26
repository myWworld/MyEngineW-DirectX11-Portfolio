#include "MEDontDestroyOnLoad.h"

ME::DonDestroyOnLoad::DonDestroyOnLoad()
{
}

ME::DonDestroyOnLoad::~DonDestroyOnLoad()
{
}

void ME::DonDestroyOnLoad::Initialize()
{
	Scene::Initialize();
}

void ME::DonDestroyOnLoad::Update()
{
	Scene::Update();
}

void ME::DonDestroyOnLoad::LateUpdate()
{
	Scene::LateUpdate();
}

void ME::DonDestroyOnLoad::Render()
{
	Scene::Render();
}

void ME::DonDestroyOnLoad::OnEnter()
{
	Scene::OnEnter();
}

void ME::DonDestroyOnLoad::OnExit()
{
	Scene::OnExit();
}
