#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <functional>
#include <Windows.h>
#include <commdlg.h>
#include <shobjidl_core.h>



//add copy and pasting
//add ctrl f





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
	WSTRING(std::string string) {
		//str = (WCHAR*)std::malloc(string.size() * 2);
		//si = string.size() * 2;
		//cur = string.size();
		str = (WCHAR*)std::malloc(string.size() * 2);
		si = string.size() * 2;
		for (auto& i : string) {
			push_back(i);
		}
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
	std::string toString() {
		std::string c;
		for (int i = 0; i < size(); i++) {
			char cp;
			WideCharToMultiByte(CP_UTF8, 0, getData() + i, 1, &cp, 1, NULL, NULL);
			c.push_back(cp);
		}
		return c;
	}
	void destroy() {
		std::free(str);
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
	void setWidth(int w) {
		width = w;
	}
	void setHeight(int h) {
		height = h;
	}
	HWND getHwnd() {
		return m_hwnd;
	}
	HINSTANCE getInstance() {
		return m_hinstance;
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
		if (size == 0) {
			data = (char*)std::malloc(10);
			size = 10;
		}
		else {
			data = (char*)std::malloc(size);
		}
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
	void del() {
		std::memcpy(data + pos - 1, data + pos, cur - pos);
		cur--;
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
	size_t getline_pos() {
		size_t i = pos;
		size_t c = 0;
		for (i = pos; i > 0 && data[i] != '\n'; i--, c++);
		return c;
	}
	size_t goto_nextline() {
		size_t i;
		size_t lp = getline_pos();
		for (i = pos; i < size && data[i] != '\n'; i++);
		i++;
		size_t c = 0;
		for (i, c; i < size && c <= lp; i++, c++);
		return i;
	}
	size_t goto_prevline() {
		size_t i;
		size_t lp = getline_pos();
		for (i = pos; i > 0 && data[i] != '\n'; i--);
		i--;
		size_t c = 0;
		for (i, c; i > 0 && c <= lp; i--, c++);
		return i;
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


class Button {
private:
	int index;
	int x, y, w, h;
	WSTRING name;
public:
	Button(int xr, int yr, int wr, int hr, int ind, std::string n) {
		x = xr;
		y = yr;
		w = wr;
		h = hr;
		index = ind;
		name = WSTRING(n);
	}
	int getIndex() {
		return index;
	}
	int getX() {
		return x;
	}
	int getY() {
		return y;
	}
	int getWidth() {
		return w;
	}
	int getHeight() {
		return h;
	}
	WSTRING getText() {
		return name;
	}
	~Button() {
		name.destroy();
	}
};


class Editor {
private:
	std::vector<File> files;
	std::vector<Button*> buttons;
	int cur_x = 110;
	size_t cur_file = 0;
	Window* wind = nullptr;
	HWND dropdown;
public:
	Editor(int w, int h, int x, int y, WNDPROC proc) {
		wind = new Window(L"Gore Text", L"GTEXTEDIT", h, w, x, y, proc);
		//dropdown init
		dropdown = CreateWindow(WC_COMBOBOX, TEXT(""), CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 0, 0, 100, 100,
			wind->getHwnd(), NULL, wind->getInstance(), NULL);
		TCHAR drops[4][11] = {
			TEXT("Open File"), TEXT("Close File"), TEXT("New File"), TEXT("Save")
		};
		TCHAR A[16];
		int  k = 0;

		memset(&A, 0, sizeof(A));
		for (k = 0; k <= 3; k += 1)
		{
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)drops[k]);

			// Add string to combobox.
			SendMessage(dropdown, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
		}
		SendMessage(dropdown, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);
		//show everything
		ShowWindow(wind->getHwnd(), true);
	}
	std::string getFileName(std::string f) {
		std::string out;
		size_t i = 0;
		for (i; i < f.size(); i++);
		i--;
		for (i; i > 0 && f[i] != '\\'; i--) {
			out.push_back(f[i]);
		}
		(i == 0) ? out.push_back(f[0]) : void();
		size_t p = out.size() - 1;
		for (i = 0; i < out.size() && i != p; i++, p--) {
			char t = out[i];
			out[i] = out[p];
			out[p] = t;
		}
		return out;
	}
	void nextFile() {
		if (cur_file + 1 >= files.size()) {
			cur_file = 0;
		}
		else {
			std::cout << files.size() << "\n";
			cur_file++;
		}
	}
	void prevFile() {
		if (cur_file - 1 < 0) {
			cur_file = files.size() - 1;
		}
		else {
			cur_file--;
		}
	}
	void openFile(std::string file) {
		File f(file);
		files.push_back(f);
		size_t index = 0;
		if (buttons.size() > 0) {
			index = files.size() - 1;
		}
		std::string name = getFileName(file);
		Button* b = new Button(cur_x, 0, name.size() * 6, 20, index, name);
		buttons.push_back(b);
		cur_x += name.size() * 6;
		cur_x += 5;
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
	void setFile(size_t index) {
		if (index < files.size()) {
			cur_file = index;
		}
	}
	File* getCurFile() {
		return &files[cur_file];
	}
	LPWSTR findFile() {
		OPENFILENAME file;
		GetOpenFileName(&file);
		std::cout << file.lpstrFile << std::endl;
		return file.lpstrFile;
	}
	std::vector<Button*>& getButtons() {
		return buttons;
	}
	bool isColliding(RECT r1, RECT r2) {
		return (r1.left < r2.left + r2.right && r1.left + r1.right > r2.left && r1.top < r2.top + r2.bottom && r1.top + r1.bottom > r2.top);
	}
	Button* findButtonDown(int mx, int my) {
		for (auto& i : buttons) {
			if (isColliding({ mx, my, 3, 3 }, { i->getX(), i->getY(), i->getWidth(), i->getHeight() })) {
				return i;
			}
		}
		return nullptr;
	}


	//main window drawing function
	void drawFile(HWND hwnd, PAINTSTRUCT* ps, File* f) {
		Rectangle(GetDC(hwnd), -1, -1, wind->getWidth(), wind->getHeight());
		WSTRING str;
		HDC hdc;
		hdc = BeginPaint(hwnd, ps);
		//draw the file buttons
		int sx = 120;
		for (auto& i : buttons) {
			SetDCPenColor(hdc, RGB(20, 20, 20));
			Rectangle(hdc, sx, 0, sx + i->getWidth(), 20);
			RECT r = { sx, 0, sx + i->getWidth(), 20};
			DrawText(hdc, i->getText().getData(), i->getText().size(), &r, DT_NOCLIP);
			sx += i->getWidth() + 5;
		}
		//drawing file
		char* t = f->getData();
		int k = 0;
		int y = 22;
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
			else if (t[i] == '\t') {
				str.push_back(' ');
				str.push_back(' ');
				str.push_back(' ');
				str.push_back(' ');
				str.push_back(' ');
			}
			else {
				str.push_back(t[i]);
			}
		}
		str.destroy();
		EndPaint(hwnd, ps);
		DeleteObject(ps);
		ReleaseDC(hwnd, hdc);
	}
	void resize() {
		RECT rect;
		GetWindowRect(wind->getHwnd(), &rect);
		wind->setHeight(rect.bottom);
		wind->setWidth(rect.right);
	}
	void createFile() {
		IFileSaveDialog* dial;
		HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&dial));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = dial->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = dial->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						size_t size = wcslen(pszFilePath);
						char* cp;
						cp = new char[size];
						WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, cp, size, NULL, NULL);
						MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
						std::string c;
						//we get weird chars in odd byte sizes for char array so gotta convert to string here and ignore other garbage
						for (int i = 0; i < size; i++) {
							c.push_back(cp[i]);
						}
						openFile(c);
						std::cout << c << "\n";
						delete[] cp;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			dial->Release();
		}
	}

	void newFile() {
		IFileOpenDialog* dial;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&dial));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = dial->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = dial->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						size_t size = wcslen(pszFilePath);
						char* cp;
						cp = new char[size];
						WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, cp, size, NULL, NULL);
						MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
						std::string c;
						//we get weird chars in odd byte sizes for char array so gotta convert to string here and ignore other garbage
						for (int i = 0; i < size; i++) {
							c.push_back(cp[i]);
						}
						openFile(c);
						std::cout << c << "\n";
						delete[] cp;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			dial->Release();
		}
	}
};
LRESULT	CALLBACK windPrc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
Editor* edit = new Editor(400, 400, 200, 200, windPrc);