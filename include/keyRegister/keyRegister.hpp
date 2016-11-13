#ifndef KEYREGISTER_HPP_
# define KEYREGISTER_HPP_

#include <list>
#include <iostream>
#include <fstream>


namespace spider
{
	class KeyRegister
	{
	public:
		KeyRegister(size_t fileMaxSize = 1000000);
		template<class packet>
		void write(spider::PacketSerializer<packet> data)
		{

		}
		char *read();

	private:
		void swapMode();
		void openReadableFile();
		void openWriteableFile();
		void getRandomAscii();

	private:
		std::ifstream			_currentFile;
		bool					_writeMode;
		std::list<std::string>	_file;
		size_t					_currentFileSize
		size_t 					_fileMaxSize;
	};
}

#endif /* KEYREGISTER_HPP_ */