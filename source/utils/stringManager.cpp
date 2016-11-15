//
// stringManager.cpp for stringManager in /home/debrab_t/rendu/TEK3/cpp_spider
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Sat Nov 12 23:16:29 2016 debrab_t
// Last update Tue Nov 15 09:26:07 2016 debrab_t
//

#include "utils/stringManager.hh"

StringManager::StringManager()
{

}

StringManager::~StringManager()
{

}

std::string		StringManager::insertStringAfterChar(std::string str, const char &toFind, std::string toSet) const
{
  for (int i = 0; i != str.size(); i++)
    {
      if (str[i] == toFind)
	{
	  str.insert(i, toSet);
	  i++;
	}
    }
  return (str);
}

void		StringManager::toLowerCase(std::string &str)
{
  for (int i = 0; i != str.length(); i++)
    {
      if (str[i] >= 'A' && str[i] <= 'Z')
	str[i] = str[i] + 32;
    }
}

bool		StringManager::checkNumber(const std::string &str) const
{
  for (int i = 0; i != str.length(); i++)
    {
      if (str[i] < '0' || str[i] > '9')
	return (false);
    }
  return (true);
}
