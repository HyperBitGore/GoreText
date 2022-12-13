#include "gore_text.h"


LRESULT	CALLBACK windPrc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	PAINTSTRUCT ps;
	File* f = nullptr;
	int y = 0;
	if (edit != nullptr) {
		f = edit->getCurFile();
		//t = f->getData();
	}
	switch (msg) {
	case WM_PAINT:
		if (edit != nullptr) {
			edit->drawFile(hwnd, &ps, f);
		}
		break;
	case WM_SIZE:
		if (edit != nullptr) {
			edit->resize();
		}
		break;
	case WM_KEYDOWN:
		switch (wparam) {
		case VK_ESCAPE:
			//DestroyWindow(hwnd);
			if (edit != nullptr) {
				edit->newFile();
			}
			break;
		case VK_CAPITAL:
			if (edit != nullptr) {
				edit->nextFile();
				InvalidateRect(hwnd, NULL, true);
			}
			break;
		case VK_RETURN:
			f->newLine();
			InvalidateRect(hwnd, NULL, true);
			break;
		case VK_BACK:
			f->remove();
			InvalidateRect(hwnd, NULL, true);
			break;
		case VK_TAB:
			f->write('\t');
			InvalidateRect(hwnd, NULL, true);
			break;
		case VK_DELETE:
			f->del();
			InvalidateRect(hwnd, NULL, true);
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
			f->setPos(f->goto_prevline());
			InvalidateRect(hwnd, NULL, true);
			break;
		case VK_DOWN:
			f->setPos(f->goto_nextline());
			InvalidateRect(hwnd, NULL, true);
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