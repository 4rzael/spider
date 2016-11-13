//
// packetUnserializer.hpp for  in /home/gandoulf/epitech/cpp_spider/include/spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Mon Nov  7 10:00:25 2016 Gandoulf
// Last update Sun Nov 13 14:33:29 2016 debrab_t
//

#ifndef PACKETUNSERIALIZER_HPP_
# define PACKETUNSERIALIZER_HPP_

#include <iostream>
#include <cstring>
#include <memory>
#include <iostream>
#include "spider/protocol.hpp"

namespace spider
{
  class PacketUnserializer
  {
  public:
    PacketUnserializer()
      : _header(NULL), _data(NULL)
    {}
    PackageHeader	getHeader()
    {
      return (*(static_cast<PackageHeader *>(static_cast<void *>(_header))));
    }
    template<class data>
    data		getData()
    {
      return (*(static_cast<data *>(static_cast<void *>(_data))));
    }

    char const *getPacketType()
    {
      return (_data);
    }
    void printPacketType()
    {
      std::cout << _data[0] << _data[1] << _data[2] << std::endl;
    }

    //setter

    void setHeader(char *header, int size)
    {
      if (_header != NULL)
	delete[] _header;
      _header = new char[size];
      std::memcpy(_header, header, size);
    }
    void setData(char *data, int size)
    {
      if (_data != NULL)
	delete[] _data;
      _data = new char[size];
      std::memcpy(_data, data, size);
    }

  private:
    char		*_header;
    char		*_data;
  };
}

#endif /* PACKETUNSERIALIZER_HPP_ */
