#include "MEApplication.h"
#include "MEInput.h"
#include "METime.h"
#include "MESceneManager.h"
#include "MEResources.h"
#include "MECollisionManager.h"
#include "MEUIManager.h"
#include "MENetworkManager.h"
#include "MEFmod.h"
#include "MERenderer.h"
#include "Protocol.h"


namespace ME 
{

	Application::Application()
		:mHwnd(nullptr)
		, mHdc(nullptr)
		, mWidth(0)
		, mHeight(0)
		,mBackBuffer(nullptr)
		,mBackHdc(nullptr)
		,mSpeed(0)
		,mbLoaded(false)
	{

	}
	
	Application::~Application()
	{
		CoUninitialize();
	}




	void Application::Initialize(HWND hwnd, UINT width, UINT height)
	{
		AdjustWindowRect(hwnd, width, height);
		InitializeEtc();

		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr)) {
			MessageBox(nullptr, L"COM 초기화 실패!", L"Error", MB_OK);
			return;
		}

	

		mGraphicDevice = std::make_unique<graphics::GraphicDevice_DX11>(); //dx11 그래픽 디바이스 생성
		 
		 renderer::Initialize();
		 mGraphicDevice->Initialize();
		 Fmod::Initialize();
		 CollisionManager::Iniatialize();
		 UIManager::Initailize();
		 SceneManager::Initialize();
		 NetworkManager::Initialize();//클라이언트 소켓 서버에 연결
		

	}

	void Application::AdjustWindowRect(HWND hwnd, UINT width, UINT height)//윈도우 크기 조정
	{
		mHwnd = hwnd;
		mHdc = GetDC(hwnd);
		RECT rect = { 0, 0, (LONG)width, (LONG)height };
		::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false); //타이틀 바랑 테두리포함 윈도우 크기 조정
		mWidth = rect.right - rect.left;
		mHeight = rect.bottom - rect.top;
		SetWindowPos(hwnd, nullptr, 0, 0, mWidth, mHeight, 0);
		ShowWindow(hwnd, true);
	}

	void Application::InitializeEtc()
	{
		Input::Initialize();
		Time::Intialize();
	}

	void Application::Run()//게임 로직 실행
	{
		if (mbLoaded.load() == false)
		{
			mbLoaded.store(true);//게임 로직이 처음 실행 될 때부터 리소스 로딩 시작
		}

		Update();
		LateUpdate();
		Render();

		Destroy();
	}

	void Application::Update()
	{
		Input::Update();
		Time::Update();

   		CollisionManager::Update();
		UIManager::Update();
		SceneManager::Update();
		NetworkManager::Update();

		
	}

	void Application::LateUpdate()
	{	
		CollisionManager::LateUpdate();
		UIManager::LateUpdate();
		SceneManager::LateUpdate();
	}

	void Application::Render()
	{
	
		graphics::GetDevice()->ClearRenderTargetView();
		graphics::GetDevice()->ClearDepthStencilView();
		graphics::GetDevice()->BindViewPort();
		graphics::GetDevice()->BindDefaultRenderTarget();


		Time::Render();
		
		CollisionManager::Render();
		SceneManager::Render();
		UIManager::Render();

		graphics::GetDevice()->Present();

		
	
	}

	void Application::Destroy()
	{
		SceneManager::Destroy();
	}

	void Application::Release()
	{
		SceneManager::Release();
		UIManager::Release();
		Resources::Release();


		renderer::Release();

		ME::NetworkManager::Release();

	}



}
