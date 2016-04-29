#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// 関数プロトタイプ
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool AnotherInstance();
bool CreateMainWindow(HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);
// グローバル変数
HINSTANCE hinst;
HDC hdc; // デバイスコンテキストへのハンドル
TCHAR ch = ' '; // 入力された文字
RECT rect;	// Rectangle構造体
PAINTSTRUCT ps; // WM_PAINTで使用される
bool vkKeys[256];	// 仮想キーの状態

// 定数
const char CLASS_NAME[] = "Keyboard";
const char APP_TITLE[] = "Prevent Multiple"; // タイトルバーのテキスト
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

	// 複数起動を防止
	if (AnotherInstance())
		return false;

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
	short nVirtKey;	// 仮想キーのコード
	const short SHIFTED = (short)0x8000;
	TEXTMETRIC tm;			// テキストメトリックの構造
	DWORD chWidth = 20;		// キャラクターの幅
	DWORD chHeight = 20;	//

	switch (msg)
	{
	case WM_CREATE:
		// メトリックを得る
		hdc = GetDC(hWnd);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hWnd, hdc);
		chWidth = tm.tmAveCharWidth;	// 文字幅の平均
		chHeight = tm.tmHeight;			// 高さ
		return 0;

	case WM_DESTROY:
		// Windowsにこのプログラムを終了するように伝える
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN: // キー押下
		vkKeys[wParam] = true;
		switch (wParam)
		{
			// VK は WinUser.hで定義されている
		case VK_SHIFT:	// シフトキー
			nVirtKey = GetKeyState(VK_LSHIFT);	// 左シフトの状態を得る
			if (nVirtKey & SHIFTED)	// もし左シフトが押下されているなら
				vkKeys[VK_LSHIFT] = true;
			nVirtKey = GetKeyState(VK_RSHIFT);
			if (nVirtKey & SHIFTED)
				vkKeys[VK_RSHIFT] = true;
			break;
		case VK_CONTROL:
			nVirtKey = GetKeyState(VK_LCONTROL);	// 左コントロールキーの状態を得る
			if (nVirtKey & SHIFTED)	// もし左シフトが押下されているなら
				vkKeys[VK_LCONTROL] = true;
			nVirtKey = GetKeyState(VK_RCONTROL);
			if (nVirtKey & SHIFTED)
				vkKeys[VK_RCONTROL] = true;
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);	// 強制的にWM_PAINT
		return 0;
		break;

	case WM_KEYUP: // キー離れている
		vkKeys[wParam] = false;
		switch (wParam)
		{
		case VK_SHIFT:
			nVirtKey = GetKeyState(VK_LSHIFT);
			if ((nVirtKey & SHIFTED) == 0)
				vkKeys[VK_LSHIFT] = false;
			nVirtKey = GetKeyState(VK_RSHIFT);
			if ((nVirtKey & SHIFTED) == 0)
				vkKeys[VK_RSHIFT] = false;
			break;
		case VK_CONTROL:
			nVirtKey = GetKeyState(VK_LCONTROL);
			if ((nVirtKey & SHIFTED) == 0)
				vkKeys[VK_LCONTROL] = false;
			nVirtKey = GetKeyState(VK_RCONTROL);
			if ((nVirtKey & SHIFTED) == 0)
				vkKeys[VK_RCONTROL] = false;
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);	// 強制的にWM_PAINT
		return 0;
		break;
	case WM_CHAR:	// キーボードから文字が入力された場合
		switch (wParam)	// 文字はwParamに格納されている
		{
		case 0x08:	// バックスペース
		case 0x09:	// タブ
		case 0x0A:	// ラインフィード
		case 0x0D:	// キャリッジリターン
		case 0x1B:	// エスケープ
			MessageBeep((UINT)-1); // ビープ音を鳴らす、表示はしない
			return 0;
		default:	// 表示可能な文字
			ch = (TCHAR)wParam; // 文字を取得
								// WM_PAINTを強制的に発生させる
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}
	case WM_PAINT:	// ウィンドウを再描画する必要がある場合
					// デバイスコンテキストへのハンドルを取得
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 0, 0, &ch, 1); // 文字の表示

		// キー配列の状態を表示
		// 押下されていれば'T' リリース時に'F'を表示
		for (int r = 0; r < 16; r++)
		{
			for (int c = 0; c < 16; c++)
			{
				if (vkKeys[r * 16 + c])
				{
					SetBkMode(hdc, OPAQUE);	// テキスト背景が不透明
					TextOut(hdc, c * chWidth + chWidth * 2, r * chHeight + chHeight * 2, "T ", 2);
				}
				else {
					SetBkMode(hdc, TRANSPARENT);	// テキスト背景が透明
					TextOut(hdc, c * chWidth + chWidth * 2, r * chHeight + chHeight * 2, "F ", 2);
				}
			}
		}

		EndPaint(hWnd, &ps);
		return 0;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

// ====================================================================================
// 現在のアプリケーションの別のインスタンスをチェック
// 戻り値 : true 別のインスタンスが検出された
//			false このインスタンスは唯一つ
// ====================================================================================
bool AnotherInstance()
{
	HANDLE ourMutex;

	// ユニークな文字列を使用してミューテックスを作る
	ourMutex = CreateMutex(NULL, true, "Use_a_different_string_here_for_each_program_48161-XYZZY");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return true; // 別のインスタンスが検出された

	return false; // インスタンスは一つだけ
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
	wcx.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); // 背景色
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