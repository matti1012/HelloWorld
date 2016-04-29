#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// �֐��v���g�^�C�v
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool AnotherInstance();
bool CreateMainWindow(HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);
// �O���[�o���ϐ�
HINSTANCE hinst;
HDC hdc; // �f�o�C�X�R���e�L�X�g�ւ̃n���h��
TCHAR ch = ' '; // ���͂��ꂽ����
RECT rect;	// Rectangle�\����
PAINTSTRUCT ps; // WM_PAINT�Ŏg�p�����
bool vkKeys[256];	// ���z�L�[�̏��

// �萔
const char CLASS_NAME[] = "Keyboard";
const char APP_TITLE[] = "Prevent Multiple"; // �^�C�g���o�[�̃e�L�X�g
const int WINDOW_WIDTH = 400; // �E�B���h�E��
const int WINDOW_HEIGHT = 400; // �E�B���h�E�̍���

// ====================================================================================
// Windows�A�v���P�[�V�����̊J�n�_
// �p�����[�^
//		hInstance : �A�v���P�[�V�����̌��݂̃C���X�^���X�ւ̃n���h��
//		hPrevInstance : ���NULL(�p�~���ꂽ�p�����[�^)
//		lpCmdLine : �R�}���h���C��������NULL�I�[������ւ̃|�C���^
//		nCmdShow : �E�B���h�E���ǂ̂悤�ɕ\�����邩���w��
// ====================================================================================
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	MSG msg;

	// �����N����h�~
	if (AnotherInstance())
		return false;

	// �E�B���h�E���쐬
	if (!CreateMainWindow(hInstance, nCmdShow))
		return true;

	// ���C���̃��b�Z�[�W���[�v
	int done = 0;
	while(!done)
	{
		// PeekMessage��Windows���b�Z�[�W�����邩���e�X�g�����u���b�N���\�b�h
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// �I�����b�Z�[�W�����m
			if (msg.message == WM_QUIT)
				done = 1;
			// ���b�Z�[�W���f�R�[�h����WinProc�ɓn��
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

// ====================================================================================
// �E�B���h�E�C�x���g�R�[���o�b�N�֐�
// ====================================================================================
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	short nVirtKey;	// ���z�L�[�̃R�[�h
	const short SHIFTED = (short)0x8000;
	TEXTMETRIC tm;			// �e�L�X�g���g���b�N�̍\��
	DWORD chWidth = 20;		// �L�����N�^�[�̕�
	DWORD chHeight = 20;	//

	switch (msg)
	{
	case WM_CREATE:
		// ���g���b�N�𓾂�
		hdc = GetDC(hWnd);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hWnd, hdc);
		chWidth = tm.tmAveCharWidth;	// �������̕���
		chHeight = tm.tmHeight;			// ����
		return 0;

	case WM_DESTROY:
		// Windows�ɂ��̃v���O�������I������悤�ɓ`����
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN: // �L�[����
		vkKeys[wParam] = true;
		switch (wParam)
		{
			// VK �� WinUser.h�Œ�`����Ă���
		case VK_SHIFT:	// �V�t�g�L�[
			nVirtKey = GetKeyState(VK_LSHIFT);	// ���V�t�g�̏�Ԃ𓾂�
			if (nVirtKey & SHIFTED)	// �������V�t�g����������Ă���Ȃ�
				vkKeys[VK_LSHIFT] = true;
			nVirtKey = GetKeyState(VK_RSHIFT);
			if (nVirtKey & SHIFTED)
				vkKeys[VK_RSHIFT] = true;
			break;
		case VK_CONTROL:
			nVirtKey = GetKeyState(VK_LCONTROL);	// ���R���g���[���L�[�̏�Ԃ𓾂�
			if (nVirtKey & SHIFTED)	// �������V�t�g����������Ă���Ȃ�
				vkKeys[VK_LCONTROL] = true;
			nVirtKey = GetKeyState(VK_RCONTROL);
			if (nVirtKey & SHIFTED)
				vkKeys[VK_RCONTROL] = true;
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);	// �����I��WM_PAINT
		return 0;
		break;

	case WM_KEYUP: // �L�[����Ă���
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
		InvalidateRect(hWnd, NULL, TRUE);	// �����I��WM_PAINT
		return 0;
		break;
	case WM_CHAR:	// �L�[�{�[�h���當�������͂��ꂽ�ꍇ
		switch (wParam)	// ������wParam�Ɋi�[����Ă���
		{
		case 0x08:	// �o�b�N�X�y�[�X
		case 0x09:	// �^�u
		case 0x0A:	// ���C���t�B�[�h
		case 0x0D:	// �L�����b�W���^�[��
		case 0x1B:	// �G�X�P�[�v
			MessageBeep((UINT)-1); // �r�[�v����炷�A�\���͂��Ȃ�
			return 0;
		default:	// �\���\�ȕ���
			ch = (TCHAR)wParam; // �������擾
								// WM_PAINT�������I�ɔ���������
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}
	case WM_PAINT:	// �E�B���h�E���ĕ`�悷��K�v������ꍇ
					// �f�o�C�X�R���e�L�X�g�ւ̃n���h�����擾
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 0, 0, &ch, 1); // �����̕\��

		// �L�[�z��̏�Ԃ�\��
		// ��������Ă����'T' �����[�X����'F'��\��
		for (int r = 0; r < 16; r++)
		{
			for (int c = 0; c < 16; c++)
			{
				if (vkKeys[r * 16 + c])
				{
					SetBkMode(hdc, OPAQUE);	// �e�L�X�g�w�i���s����
					TextOut(hdc, c * chWidth + chWidth * 2, r * chHeight + chHeight * 2, "T ", 2);
				}
				else {
					SetBkMode(hdc, TRANSPARENT);	// �e�L�X�g�w�i������
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
// ���݂̃A�v���P�[�V�����̕ʂ̃C���X�^���X���`�F�b�N
// �߂�l : true �ʂ̃C���X�^���X�����o���ꂽ
//			false ���̃C���X�^���X�͗B���
// ====================================================================================
bool AnotherInstance()
{
	HANDLE ourMutex;

	// ���j�[�N�ȕ�������g�p���ă~���[�e�b�N�X�����
	ourMutex = CreateMutex(NULL, true, "Use_a_different_string_here_for_each_program_48161-XYZZY");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return true; // �ʂ̃C���X�^���X�����o���ꂽ

	return false; // �C���X�^���X�͈����
}

// ====================================================================================
// �E�B���h�E���쐬
// �߂�l : �G���[�̏ꍇ�Afalse
// ====================================================================================
bool CreateMainWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcx;
	HWND hwnd;

	// �E�B���h�E�N���X�̍\���̂����C���E�B���h�E���L�q����p�����[�^�Őݒ肵�܂��B
	wcx.cbSize = sizeof(wcx); // �\���̂̃T�C�Y
	wcx.style = CS_HREDRAW | CS_VREDRAW; // �E�B���h�E�T�C�Y�ύX���ɍĕ`��
	wcx.lpfnWndProc = WinProc; // �E�B���h�E�v���V�[�W�����w��
	wcx.cbClsExtra = 0; // �g���N���X�������Ȃ�
	wcx.cbWndExtra = 0; // �g���E�B���h�E�������Ȃ�
	wcx.hInstance = hInstance; // �C���X�^���X�ւ̃n���h��
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW); // ���O��`����Ă�����J�[�\��
	wcx.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); // �w�i�F
	wcx.lpszMenuName = NULL; // ���j���[���\�[�X�̖��O
	wcx.lpszClassName = CLASS_NAME; // �E�B���h�E�N���X�̖��O
	wcx.hIconSm = NULL; // �������A�C�R��
	
	// �E�B���h�E�N���X��o�^
	// �G���[���ARegisterClassEx��0��߂�
	if (RegisterClassEx(&wcx) == 0)	// �G���[�̏ꍇ
		return false;
	// �E�B���h�E���쐬
	hwnd = CreateWindow(
		CLASS_NAME,	// �E�B���h�E�N���X�̖��O
		APP_TITLE, // �^�C�g���o�[�̃e�L�X�g
		WS_OVERLAPPEDWINDOW, // �E�B���h�E�̃X�^�C��
		CW_USEDEFAULT, // �E�B���h�E�̐����ʒu�̃f�t�H���g
		CW_USEDEFAULT, // �E�B���h�E�̐����ʒu�̃f�t�H���g
		WINDOW_WIDTH,	// �E�B���h�E�̕�
		WINDOW_HEIGHT,	// �E�B���h�E�̍���
		(HWND)NULL,		// �e�E�B���h�E �Ȃ�
		(HMENU)NULL,	// ���j���[	�Ȃ�
		hInstance,		// �A�v���P�[�V�����C���X�^���X�ւ̃n���h��
		(LPVOID)NULL	// �E�B���h�E�p�����[�^ �Ȃ�
		);
	// �E�B���h�E�̍쐬�ŃG���[�����������ꍇ
	if (!hwnd)
		return false;
	// �E�B���h�E��\��
	ShowWindow(hwnd, nCmdShow);
	// �E�B���h�E�v���V�[�W����WM_PAINT ���b�Z�[�W�𑗂�
	UpdateWindow(hwnd);
	return true;
}