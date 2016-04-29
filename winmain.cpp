#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// 関数プロトタイプ
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool CreateMainWindow(HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);
// グローバル変数
HINSTANCE hinst;
// 定数
const char CLASS_NAME[] = "WinMain";
const char APP_TITLE[] = "Hello World"; // タイトルバーのテキスト
const int WINDOW_WIDTH = 400; // ウィンドウ幅
const int WINDOW_HEIGHT = 400; // ウィンドウの高さ

// ====================================================================================
// Windowsアプリケーションの開始点
// パラメータ
//		hInstance : アプリケーションの現在のインスタンスへのハンドル
//		hPrevInstance : 常にNULL(廃止されたパラメータ)
//		lpCmdLine : コマンドライン引数のNULL終端文字列へのポインタ
//		nCmdShow : ウィンドウをどのように表示するかを指定
// ====================================================================================
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	MSG msg;

	// ウィンドウを作成
	if (!CreateMainWindow(hInstance, nCmdShow))
		return true;
	// メインのメッセージループ
	int done = 0;
	while(!done)
	{
		// PeekMessageはWindowsメッセージがあるかをテストする非ブロックメソッド
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// 終了メッセージを検知
			if (msg.message == WM_QUIT)
				done = 1;
			// メッセージをデコードしてWinProcに渡す
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

// ====================================================================================
// ウィンドウイベントコールバック関数
// ====================================================================================
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		// Windowsにこのプログラムを終了するように伝える
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// ====================================================================================
// ウィンドウを作成
// 戻り値 : エラーの場合、false
// ====================================================================================
bool CreateMainWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcx;
	HWND hwnd;

	// ウィンドウクラスの構造体をメインウィンドウを記述するパラメータで設定します。
	wcx.cbSize = sizeof(wcx); // 構造体のサイズ
	wcx.style = CS_HREDRAW | CS_VREDRAW; // ウィンドウサイズ変更時に再描画
	wcx.lpfnWndProc = WinProc; // ウィンドウプロシージャを指す
	wcx.cbClsExtra = 0; // 拡張クラスメモリなし
	wcx.cbWndExtra = 0; // 拡張ウィンドウメモリなし
	wcx.hInstance = hInstance; // インスタンスへのハンドル
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW); // 事前定義されている矢印カーソル
	wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // 背景ブラシ
	wcx.lpszMenuName = NULL; // メニューリソースの名前
	wcx.lpszClassName = CLASS_NAME; // ウィンドウクラスの名前
	wcx.hIconSm = NULL; // 小さいアイコン
	
	// ウィンドウクラスを登録
	// エラー時、RegisterClassExは0を戻す
	if (RegisterClassEx(&wcx) == 0)	// エラーの場合
		return false;
	// ウィンドウを作成
	hwnd = CreateWindow(
		CLASS_NAME,	// ウィンドウクラスの名前
		APP_TITLE, // タイトルバーのテキスト
		WS_OVERLAPPEDWINDOW, // ウィンドウのスタイル
		CW_USEDEFAULT, // ウィンドウの水平位置のデフォルト
		CW_USEDEFAULT, // ウィンドウの垂直位置のデフォルト
		WINDOW_WIDTH,	// ウィンドウの幅
		WINDOW_HEIGHT,	// ウィンドウの高さ
		(HWND)NULL,		// 親ウィンドウ なし
		(HMENU)NULL,	// メニュー	なし
		hInstance,		// アプリケーションインスタンスへのハンドル
		(LPVOID)NULL	// ウィンドウパラメータ なし
		);
	// ウィンドウの作成でエラーが発生した場合
	if (!hwnd)
		return false;
	// ウィンドウを表示
	ShowWindow(hwnd, nCmdShow);
	// ウィンドウプロシージャにWM_PAINT メッセージを送る
	UpdateWindow(hwnd);
	return true;
}