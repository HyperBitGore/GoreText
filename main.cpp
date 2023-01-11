#include "gore_text.h"


LRESULT	CALLBACK windPrc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	PAINTSTRUCT ps;
	File* f = nullptr;
	int y = 0;
	if (edit != nullptr) {
		f = edit->getCurFile();
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
	case WM_KEYUP:
		switch (wparam) {
		case VK_CONTROL:
			edit->setCtrl(false);
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wparam) {
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case VK_CAPITAL:
			
			break;
		case VK_CONTROL:
			edit->setCtrl(true);
			break;
		case VK_SHIFT:

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
			if (GetAsyncKeyState(VK_CONTROL)) {
				if (c == 'v') {
					//get the clipboard data
					std::string clip = edit->getClipboardText();
					//write to current file
					edit->copyStringToFile(clip, f);
				}
				else if (c == 'c') {
					//get selection
					std::string sel = edit->getSelection();
					//add to clipboard
					edit->setClipboardText(sel);
					edit->clearSelection();
				}
			}
			else {
				f->write(c);
			}
			InvalidateRect(hwnd, NULL, true);
		}
			break;
		}
		break;
	case WM_CLOSE:
		if (f != nullptr) {
			f->save();
		}
		DestroyWindow(hwnd);
		break;
	case WM_LBUTTONUP:
		edit->endSelection();
		break;
	case WM_LBUTTONDOWN:
		{
			LPPOINT p = new tagPOINT;
			GetCursorPos(p);
			//converts cursos pos to window pos
			ScreenToClient(hwnd, p);
			if (p->y < 25) {
				Button* b = edit->findButtonDown(p->x, p->y);
				if (b != nullptr) {
					edit->setFile(b->getIndex());
					InvalidateRect(hwnd, NULL, true);
				}
			}
			else {
				if (edit->getSelect()) {
					edit->startSelection();
				}
				else {
					edit->changeSelect(p->x, p->y);
				}
				InvalidateRect(hwnd, NULL, true);
			}
			delete p;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		if (HIWORD(wparam) == CBN_SELCHANGE)
			// If the user makes a selection from the list:
			//   Send CB_GETCURSEL message to get the index of the selected list item.
			//   Send CB_GETLBTEXT message to get the item.
			//   Display the item in a messagebox.
		{
			int ItemIndex = SendMessage((HWND)lparam, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);

			switch (ItemIndex) {
			case 0:
				edit->newFile();
				InvalidateRect(hwnd, NULL, true);
				break;
			case 1:
				edit->closeFile();
				InvalidateRect(hwnd, NULL, true);
				break;
			case 2:
				edit->createFile();
				InvalidateRect(hwnd, NULL, true);
				break;
			case 3:
				f->save();
				InvalidateRect(hwnd, NULL, true);
				break;
			}
		}
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


int main() {
	while (edit->loop());
	return 0;
}