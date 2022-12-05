#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <functional>
#include <Windows.h>



class WSTRING {
private:
	WCHAR* str = nullptr;
	size_t si = 0;
	size_t cur = 0;
public:
	WSTRING() { str = (WCHAR*)std::malloc(40); si = 40; cur = 0; };
	WSTRING(LPCWSTR string, size_t length) {
		str = (WCHAR*)std::malloc(length * 2);
		si = length * 2;
		cur = length;
		if (str != 0){
			std::memcpy(str, string, length * 2);
		}
	}
	~WSTRING() {
		std::free(str);
	}
	void push_back(char c) {
		if ((cur*2) + 2 > si) {
			WCHAR* t = (WCHAR*)std::realloc(str, (si * 2));
			if (t == nullptr) {
				return;
			}
			si = (si << 1);
			str = t;
		}
		//str[cur] = c;
		MultiByteToWideChar(CP_UTF8, 0, &c, 1, str + cur, 1);
		cur++;
	} 
	void insert(char c, size_t pos) {
		if (pos < 0 || pos > si) {
			return;
		}
		if ((cur) + 1 > si) {
			WCHAR* d = (WCHAR*)std::realloc(str, si * 2);
			if (d == nullptr) {
				return;
			}
			str = d;
			si = (si << 1);
		}
		std::memcpy(str + pos + 1, str + pos, (cur - pos) * 2);
		MultiByteToWideChar(CP_UTF8, 0, &c, 1, str + pos, 1);
		//str[pos] = c;
		cur++;
	}
	size_t size() {
		return cur;
	}
	LPCWSTR getData() {
		return str;
	}
	void clear() {
		std::memset(str, 0, si);
		cur = 0;
	}
};



class Window {
private:
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	LPCWSTR class_name;
	int height;
	int width;
public:
	Window(LPCWSTR title, LPCWSTR CLASS_NAME, int h, int w, int x, int y, WNDPROC proc)
		: m_hinstance(GetModuleHandle(nullptr))
	{
		class_name = CLASS_NAME;
		WNDCLASS wnd = {};
		wnd.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wnd.lpszClassName = CLASS_NAME;
		wnd.hInstance = m_hinstance;
		wnd.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wnd.hCursor = LoadIcon(NULL, IDC_ARROW);
		wnd.hbrBackground = 0;
		wnd.lpfnWndProc = proc;

		RegisterClass(&wnd);

		DWORD windStyle = WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX | WS_SYSMENU;
		height = h;
		width = w;

		RECT rect;
		rect.left = x;
		rect.top = y;
		rect.right = rect.left + width;
		rect.bottom = rect.top + height;

		AdjustWindowRect(&rect, windStyle, false);

		m_hwnd = CreateWindowEx(0, CLASS_NAME,
			title,
			windStyle,
			rect.left,
			rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			NULL,
			NULL,
			m_hinstance,
			NULL
		);
	}
	Window(const Window&) = delete;
	Window& operator =(const Window&) = delete;
	~Window() {
		//has to be same class name
		UnregisterClass(class_name, m_hinstance);
	}
	bool ProcessMessage() {
		MSG msg = {};

		while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				return false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	}
	int getWidth() {
		return width;
	}
	int getHeight() {
		return height;
	}

	HWND getHwnd() {
		return m_hwnd;
	}
};

class File {
private:
	char* data;
	size_t size;
	size_t cur;
	size_t pos = 0;
	std::string fi;
public:
	File(std::string file) {
		fi = file;
		std::ifstream f;
		f.open(file);
		std::stringstream ss;
		ss << f.rdbuf();
		f.close();
		std::string str = ss.str();
		size = 0;
		for (auto& i : str) {
			size++;
		}
		cur = size;
		data = (char*)std::malloc(size);
		std::memcpy(data, str.c_str(), size);
	}
	void write(char c) {
		if (cur + 1 > size) {
			char* d = (char*)std::realloc(data, size * 2);
			if (d == nullptr) {
				return;
			}
			data = d;
			size = size * 2;
		}
		std::memcpy(data + pos + 1, data + pos, cur - pos);
		data[pos] = c;
		pos++;
		cur++;
	}
	void newLine() {
		if (cur + 1 > size) {
			char* d = (char*)std::realloc(data, size * 2);
			if (d == nullptr) {
				return;
			}
			data = d;
			size = size * 2;
		}
		size_t i = pos + 1;
		for (i; i < size && data[i] != '\n'; i++);
		std::memcpy(data + pos + 1, data + pos, i);
		data[pos] = '\n';
		pos++;
		cur++;
	}
	void remove() {
		std::memcpy(data + pos, data + pos + 1, cur - pos);
		cur--;
		pos--;
	}
	void save() {
		std::ofstream f;
		f.open(fi);
		for (int i = 0; i < cur; i++) {
			f << data[i];
		}
		f.close();
	}
	void setPos(size_t p) {
		if (p > size || p < 0) {
			return;
		}
		pos = p;
	}
	size_t getPos() {
		return pos;
	}
	size_t getSize() {
		return cur;
	}
	char* getData() {
		return data;
	}
};



class Editor {
private:
	std::vector<File> files;
	Window* wind = nullptr;
public:
	Editor(int w, int h, int x, int y, WNDPROC proc) {
		wind = new Window(L"Gore Text", L"GTEXTEDIT", h, w, x, y, proc);
		ShowWindow(wind->getHwnd(), true);
	}
	void openFile(std::string file) {
		File f(file);
		files.push_back(f);
	}
	void closeFile() {
		
	}
	bool loop() {
		UpdateWindow(wind->getHwnd());
		if (!wind->ProcessMessage()) {
			std::cout << "Closing window\n";
			delete wind;
			return false;
		}
		return true;
	}
	File* getFile(size_t index) {
		if (index >= files.size()) {
			return nullptr;
		}
		return &files[index];
	}

	LPWSTR findFile() {
		OPENFILENAME file;
		GetOpenFileName(&file);
		std::cout << file.lpstrFile << std::endl;
		return file.lpstrFile;
	}
	//it's not actually clearing screen
	void drawFile(HWND hwnd, PAINTSTRUCT* ps, File* f) {
		WSTRING str;
		HDC hdc;
		hdc = BeginPaint(hwnd, ps);
		//SetBkMode(hdc, TRANSPARENT);
		char* t = f->getData();
		int k = 0;
		int y = 0;
		for (int i = 0; i <= f->getSize(); i++, k++) {
			if (i == f->getPos()) {
				str.push_back('|');
			}
			if (t[i] == '\n' || i == f->getSize()) {
				RECT r = { 0, y, 0, 16};
				DrawText(hdc, str.getData(), str.size(), &r, DT_NOCLIP);
				str.clear();
				k = 0;
				y += 16;
			}
			else {
				str.push_back(t[i]);
			}
		}
		EndPaint(hwnd, ps);
		DeleteObject(ps);
		ReleaseDC(hwnd, hdc);
	}

};
LRESULT	CALLBACK windPrc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
Editor* edit = new Editor(400, 400, 200, 200, windPrc);