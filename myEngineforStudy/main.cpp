// myEngineforStudy.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#define WIN32_LEAN_AND_MEAN 
#include "framework.h"
#include "myEngineforStudy.h"
#include "..//MyEngine_Source/MEApplication.h"
#include "..//MyEngine_W/MELoadScene.h"
#include "..//MyEngine_Source//MEResources.h"
#include "windows.h"
#include "../MyEngine_Source/METexture.h"
#include "../MyEngine_W/METoolScene.h"
#include "../MyEngine_Source/MESceneManager.h"



ME::Application application;

ULONG_PTR gpToken;
Gdiplus::GdiplusStartupInput gpsi;


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass (HINSTANCE hInstance, const wchar_t* name, WNDPROC proc);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//LRESULT CALLBACK    WndTileProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //디버그, 메모리 누수 체크
    //_CrtSetBreakAlloc(251);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYENGINEFORSTUDY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance , szWindowClass, WndProc); //윈도우 속성 등록


    ME::LoadScenes();//내부에서 로딩씬을 만들고 실행해서 -> 메모리에 사용할 리소들을 작업 쓰레드로 로딩 -> 로딩씬 끝나면 -> 플레이씬으로 넘어간다.

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYENGINEFORSTUDY));

    MSG msg;

    //AllocConsole();

    while (true)
    {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) //윈도우 메시지 처리, 논블로킹 메시지 처리
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }
            

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {

            //메시지가 없을 경우
              //게임 로직이 들어가면 된다.
          
            application.Run(); //게임 로직 돌아가는 곳
        }

    }

    // 기본 메시지 루프입니다:
   // while (GetMessage(&msg, nullptr, 0, 0)) //블락킹
   // {
      //  if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
     //   {
  //          TranslateMessage(&msg);
///DispatchMessage(&msg);
  //      }
   // }
    RemoveFontResourceEx(L"..\\Resources\\jejufont.tff", FR_PRIVATE, 0);

   
    application.Release(); //리소스 해제

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance,const wchar_t * name,  WNDPROC proc)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = proc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYENGINEFORSTUDY)); //아이콘
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW); //커서
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYENGINEFORSTUDY);
    wcex.lpszClassName  = name ;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
   const UINT width = 1300;
   const UINT height = 900;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, width,height, nullptr, nullptr, hInstance, nullptr); //윈도우창 만들기



 

   if (!hWnd)
   {
      return FALSE;
   }


   ShowWindow(hWnd, nCmdShow); //설정한 윈도우 띄우기
   UpdateWindow(hWnd);



   application.Initialize(hWnd, width, height); //게임 로직 초기화, 해상도 넘김

   //Tile 윈도우 크기 조정

   // ME::Scene* activeScene = ME::SceneManager::GetActiveScene();

   // std::wstring name = activeScene->GetName();


   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            //DC란 화면에 출력에 필요한 모든 정보를 가지는 데이터 구조체
            //GDI모듈에 의해서 관리된다.
            //어떤 폰트를 사용할 건가, 어떤 선의 굵기를 정해줄건가 어떤 색상으로 그려줄껀가
            //화면 출력에 필요한 모든 경우는 WINAPI에서는 DC를 통해서 작업을 진행할 수 있다,
            
            //여기서 사용안함
         

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
