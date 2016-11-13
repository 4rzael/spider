//
// HandleFileServer.cpp for handlefileserver in /home/debrab_t/rendu/TEK3/cpp_spider
//
// Made by debrab_t
// Login   <debrab_t@epitech.net>
//
// Started on  Fri Nov 11 15:17:59 2016 debrab_t
// Last update Sat Nov 12 23:52:43 2016 debrab_t
//

#include "bdd_connect/HandleFileServer.hh"

HandleFileServer::HandleFileServer()
{
  _model.insert(std::pair<HandleData::column,std::string>(HandleData::column::tmeStampCLC, "timestamp CLC"));
  _model.insert(std::pair<HandleData::column,std::string>(HandleData::column::xCLC, "x CLC"));
  _model.insert(std::pair<HandleData::column,std::string>(HandleData::column::yCLC, "y CLC"));
  _model.insert(std::pair<HandleData::column,std::string>(HandleData::column::idCLC, "id CLC"));
  _model.insert(std::pair<HandleData::column,std::string>(HandleData::column::tmeStampMVT, "timestamp MVT"));
  _model.insert(std::pair<HandleData::column,std::string>(HandleData::column::xMVT, "x MVT"));
  _model.insert(std::pair<HandleData::column,std::string>(HandleData::column::yMVT, "y MVT"));
  _model.insert(std::pair<HandleData::column,std::string>(HandleData::column::tmeStampTOC, "timestamp TOC"));
  _model.insert(std::pair<HandleData::column,std::string>(HandleData::column::idTOC, "id TOC"));
}

HandleFileServer::~HandleFileServer()
{

}

void		HandleFileServer::insertDataClient(int idClient, const std::map<HandleData::column, std::string> &data)
{
  std::ofstream	ofClientFile;
  std::map<HandleData::column, std::string>::iterator it;

  ofClientFile.open("saveClient/" + std::to_string(idClient) + ".csv", std::ios::in | std::ios::out | std::ios::app);
  if (!ofClientFile.fail())
    {
      std::ifstream ifClientFile("saveClient/" + std::to_string(idClient) + ".csv", std::ios::in);
      ifClientFile.seekg(0, std::ios::end);
      if (ifClientFile.tellg() == 0)
	{
	  for (it = _model.begin(); it != _model.end(); it++ )
	    {
	      ofClientFile << "\"" << it->second << "\"" << ";";
	    }
	  ofClientFile << std::endl;
	}
      verifFirstLine(idClient, ifClientFile);
      for (it = _model.begin(); it != _model.end(); it++ )
	{
	  if (data.find(it->first) != data.end())
	    ofClientFile << "\"" << strMan.insertStringAfterChar(std::string(data.find(it->first)->second), '"', std::string("\"")) << "\"" << ";";
	  else
	    ofClientFile << ";";
	}
      ofClientFile << std::endl;
      ofClientFile.close();
      ifClientFile.close();
    }
}

void		HandleFileServer::verifFirstLine(int idClient, std::ifstream &file)
{
  std::string	firstL;
  std::string	firstCL;
  std::map<HandleData::column, std::string>::iterator it;

  file.seekg(0, std::ios::beg);
  getline(file, firstL);
  for (it = _model.begin(); it != _model.end(); it++ )
    {
      firstCL += "\"" + it->second + "\"" + ";";
    }
  if (firstL.compare(firstCL) != 0)
    std::cerr << "Format of saved file is incorrectx " << std::to_string(idClient) << ".csv" << " - first line should be :\n" << firstCL << std::endl;
}
