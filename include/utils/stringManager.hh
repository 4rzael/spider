//
// stringManager.hh for stringmanager in /home/debrab_t/rendu/TEK3/cpp_spider
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Sat Nov 12 23:17:24 2016 debrab_t
// Last update Sun Nov 13 13:55:25 2016 debrab_t
//

#ifndef STRINGMANAGER_HH_
# define STRINGMANAGER_HH_

# include <iostream>

class	StringManager
{
public:
  StringManager();
  ~StringManager();

  std::string	insertStringAfterChar(std::string, const char&, std::string) const;
  void		toLowerCase(std::string&);
};

#endif /* STRINGMANAGER_HH_ */
