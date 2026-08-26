#pragma once
#define WIN32_LEAN_AND_MEAN

#include "CommonInclude.h"
#include "MEGameObject.h"
#include "MEScenes.h"
#include "MEGraphicDevice_DX11.h"


namespace ME
{
	

	class Application
	{

	public:

		Application();
		~Application();

		void Initialize(HWND hwnd, UINT width, UINT height);

		void AdjustWindowRect(HWND hwnd, UINT width, UINT height);
		void InitializeEtc();

		void Run();
		void Update();
		void LateUpdate();
		void Render();
		void Destroy();

		void Release();


		HDC GetHdc() const{ return mHdc; }
		HWND GetHwnd() const { return mHwnd; }
		UINT GetWidth() const { return mWidth; }
		UINT GetHeight() const { return mHeight; }

		
		bool IsLoaded() const { return mbLoaded.load(); }
		void IsLoaded(bool load) { mbLoaded.store(load); }
	private:

		HWND mHwnd;
		HDC mHdc;

		HDC mBackHdc;
		HBITMAP mBackBuffer;

		UINT mWidth;
		UINT mHeight;

		float mSpeed;

		std::atomic<bool> mbLoaded;


		std::vector <Scene*> mScenes;

		std::unique_ptr<graphics::GraphicDevice_DX11> mGraphicDevice;
	};

}

