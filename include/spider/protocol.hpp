//
// protocol.hpp for  in /home/gandoulf/epitech/cpp_spider/include/spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 15:44:31 2016 Gandoulf
// Last update Mon Nov  7 14:32:34 2016 Gandoulf
//

#ifndef PROTOCOL_HPP_
# define PROTOCOL_HPP_

# define SEND	0x6D695F55
# define REC	0x611E283D
# define HEADER	12

typedef struct PackageHeader
{
  int	magicNumber;
  int	size;
  int	id;
} PackageHeader;

typedef struct Test
{
  char	cmd[3];
  char	str[100];
} Test;

#endif /* PROTOCOL_HPP_ */
