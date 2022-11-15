#include "gore_text.h"


LRESULT	CALLBACK windPrc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	HDC hdc;
	PAINTSTRUCT ps;
	File* f = nullptr;
	char* t = nullptr;
	int y = 0;
	if (edit != nullptr) {
		f = edit->getFile(0);
		t = f->getData();
	}
	switch (msg) {
	case WM_PAINT:
		if (edit != nullptr) {
			WSTRING str;
			hdc = BeginPaint(hwnd, &ps);
			int x = 0;
			int o = 0;
			bool line = false;
			int j = 0;
			int k = 0;
			for (int i = 0; i <= f->getSize(); i++, x += 8, k++) {
				if (i == f->getPos()) {
					line = true;
					o = x;
					j = k;
				}
				if (t[i] == '\n' || i == f->getSize()) {
					if (line) {
						line = false;
						TextOut(hdc, 0, y, str.getData(), j);
						SetDCPenColor(hdc, RGB(255, 80, 20));
						Rectangle(hdc, o, y, o + 8, y + 8);
						TextOut(hdc, o + 8, y, str.getData() + j, str.size() - j);
						//SetDCPenColor(hdc, BLACK_PEN);
					}
					else {
						TextOut(hdc, 0, y, str.getData(), str.size());
					}
					str.clear();
					k = 0;
					x = 0;
					y += 16;
				}
				else {
					str.push_back(t[i]);
				}
			}
			EndPaint(hwnd, &ps);
		}
		break;
	case WM_KEYDOWN:
		switch (wparam) {
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case VK_RETURN:
			
			break;
		case VK_BACK:
			f->remove();
			InvalidateRect(hwnd, NULL, true);
			break;
		case VK_TAB:
			
			break;
		case VK_SPACE:
			f->write(' ');
			InvalidateRect(hwnd, NULL, true);
			break;
		case VK_RIGHT:
			f->setPos((f->getPos()) + 1);
			InvalidateRect(hwnd, NULL, true);
			break;
		case VK_LEFT:
			f->setPos((f->getPos()) - 1);
			InvalidateRect(hwnd, NULL, true);
			break;
		case VK_UP:
			
			break;
		case VK_DOWN:
			
			break;
		default:
		{
			char c = MapVirtualKey(wparam, 2);
			if (!GetAsyncKeyState(VK_SHIFT)) { c += 32; }
			//std::cout << c << "\n";
			f->write(c);
			InvalidateRect(hwnd, NULL, true);
		}
			break;
		}
		break;
	case WM_CLOSE:
		f->save();
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


//https://learn.microsoft.com/en-us/windows/win32/dlgbox/open-and-save-as-dialog-boxes
//https://learn.microsoft.com/en-us/windows/win32/controls/create-a-button
int main() {
	//edit = new Editor(400, 400, 200, 200, windPrc);
	edit->openFile("file1.txt");
	while (edit->loop());
	return 0;
}