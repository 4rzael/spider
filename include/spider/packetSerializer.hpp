//
// packet.hpp for  in /home/gandoulf/epitech/cpp_spider/include/spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sun Nov  6 12:54:32 2016 Gandoulf
// Last update Sat Nov 12 14:33:08 2016 Gandoulf
//

#ifndef PACKETSERILIZER_HPP_
# define PACKETSERILIZER_HPP_

#include <iostream>
#include <cstring>
#include <memory>
#include "spider/protocol.hpp"

namespace spider
{
  typedef std::unique_ptr<char> packedData;

  template<class data>
  class PacketSerializer
  {
  public:
    PacketSerializer(int size, int id, data data_)
    {
      _packaheHeader.magicNumber = SEND;
      _packaheHeader.size = size;
      _packaheHeader.id = id;
      _data = data_;
    }

    //getter
    int	getPacketSize() const
    {
      return (_packaheHeader.size);
    }
    PackageHeader const &getPackageHeader() const
    {
      return (_packaheHeader);
    }
    data const &getData() const
    {
      return (_data);
    }

    // working on it

    char *getPackedData(int useless)
    {
      char *package = new char [sizeof(PackageHeader) + _packaheHeader.size];
      std::memcpy(package, static_cast<void *>(&_packaheHeader),
		   sizeof(PackageHeader));
      std::memcpy(package + sizeof(PackageHeader),
		   static_cast<void *>(&_data),
		  _packaheHeader.size - sizeof(PackageHeader));
      return (package);
    }

    packedData getPackedData()
    {
      packedData package(new char [sizeof(PackageHeader) + _packaheHeader.size]);
      std::memcpy(package.get(), static_cast<void *>(&_packaheHeader),
		   sizeof(PackageHeader));
      std::memcpy(package.get() + sizeof(PackageHeader),
		   static_cast<void *>(&_data),
		  _packaheHeader.size - sizeof(PackageHeader));
      return (package);
      }

  private:
    PackageHeader	_packaheHeader;
    data		_data;
  };
}

#endif /* PACKETSERIALIZER_HPP_ */
