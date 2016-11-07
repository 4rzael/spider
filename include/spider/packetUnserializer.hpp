//
// packetUnserializer.hpp for  in /home/gandoulf/epitech/cpp_spider/include/spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Mon Nov  7 10:00:25 2016 Gandoulf
// Last update Mon Nov  7 14:40:48 2016 Gandoulf
//

#ifndef PACKETUNSERIALIZER_HPP_
# define PACKETUNSERIALIZER_HPP_

#include <cstring>
#include <memory>
#include "spider/protocol.hpp"

namespace spider
{
  class PacketUnserializer
  {
  public:
    PacketUnserializer() {}
    PackageHeader	getHeader()
    {
      return (*(static_cast<PackageHeader *>(static_cast<void *>(_header))));
    }
    template<class data>
    data		getData()
    {
      return (*(static_cast<data *>(static_cast<void *>(_data))));
    }

    //setter

    void setHeader(char *header, int size)
    {
      _header = new char[size];
      std::memcpy(_header, header, size);
    }
    void setData(char *data, int size)
    {
      _data = new char[size];
      std::memcpy(_data, data, size);
    }

  private:
    char		*_header;
    char		*_data;
  };
}

#endif /* PACKETUNSERIALIZER_HPP_ */
