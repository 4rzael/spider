#define _WIN32_WINNT 0x0501 // Asio et Windows XP
#include <boost/asio.hpp>
#include <iostream>

int resolver()
{
  // Création du service principal et du résolveur.
  boost::asio::io_service ios;
  boost::asio::ip::tcp::resolver resolver(ios);// (1)

  // Paramètrage du resolver sur Developpez.com
  boost::asio::ip::tcp::resolver::query query("www.developpez.com", "80"); // (2)

  // On récupère une "liste" d'itérateur
  boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query); // (3)
  boost::asio::ip::tcp::resolver::iterator end; //Marqueur de fin
  while (iter != end) // On itère le long des endpoints
    {
      boost::asio::ip::tcp::endpoint endpoint = *iter++;
      std::cout << endpoint << std::endl; // on affiche (4)
    }

  return 0;
}
