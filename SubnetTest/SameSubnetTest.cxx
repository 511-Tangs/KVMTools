#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
 
int main(int argc, char* argv[])
{
  char Ip1Octet[4][4], Ip2Octet[4][4], MaskOctet[4][4], JunkOctet[5][4];
  int  Ip1Addr[4], Ip2Addr[4], NetMask[4];
  int  i, NoOfOctet;

//  fprintf(stderr, "%d arguments seen.\n", argc);
 
  if (argc != 4)
    { 
      fprintf(stderr, "SameSubnetP IP1 IP2 Mask, where IP1, IP2, Mask are IP\n");
      fprintf(stderr, "    addresses and netmask in dotted decimal notation.\n");
      fprintf(stderr, "SameSubnetP, based on given netmask Mask, returns 0,\n"); 
      fprintf(stderr, "   if IP1, IP2 are in the same subnet. If IP1, IP2 are not\n");
      fprintf(stderr, "   in the same subnet, it returns 1.\n");
      fprintf(stderr, "If wrong syntaxes occur, exit with value 2 upto 12.\n");
      exit(2);
    }

  if (sscanf(argv[1], "%[^.].%[^.].%[^.].%[^.].%[^\r\n]", JunkOctet[0], JunkOctet[1], 
      JunkOctet[2], JunkOctet[3], JunkOctet[4]) > 4) 
    {
       fprintf(stderr, "Input String %s has too many components.\n", argv[1]);
       exit(10);
    }

  if (sscanf(argv[2], "%[^.].%[^.].%[^.].%[^.].%[^\r\n]", JunkOctet[0], JunkOctet[1], 
      JunkOctet[2], JunkOctet[3], JunkOctet[4]) > 4) 
    {
       fprintf(stderr, "Input String %s has too many components.\n", argv[2]);
       exit(11);
    }

  if (sscanf(argv[3], "%[^.].%[^.].%[^.].%[^.].%[^\r\n]", JunkOctet[0], JunkOctet[1], 
      JunkOctet[2], JunkOctet[3], JunkOctet[4]) > 4) 
    {
       fprintf(stderr, "Input String %s has too many components.\n", argv[3]);
       exit(12);
    }


//  Scan arg[1] (Ip1), arg[2] (Ip2), and arg[3] (NetMask), put them in respective 
//  Octets.
  if (sscanf(argv[1], "%[0-9].%[0-9].%[0-9].%[0-9]", Ip1Octet[0], Ip1Octet[1], 
      Ip1Octet[2], Ip1Octet[3]) < 4) 
    {
       fprintf(stderr, "Input String: %s, wrong IP1 address format.\n", argv[1]);
       exit(3);
    }

  if (sscanf(argv[2], "%[0-9].%[0-9].%[0-9].%[0-9]", Ip2Octet[0], Ip2Octet[1], 
      Ip2Octet[2], Ip2Octet[3]) < 4) 
    {
       fprintf(stderr, "Input String: %s, wrong IP2 address format.\n", argv[2]);
       exit(4);
    }

  if (sscanf(argv[3], "%[0-9].%[0-9].%[0-9].%[0-9]", MaskOctet[0], MaskOctet[1], 
      MaskOctet[2], MaskOctet[3]) < 4) 
    {
       fprintf(stderr, "Input String: %s, wrong NetMask format.\n", argv[3]);
       exit (5);
    }

// convert each string in each Octet to integer
  for (i=0; i<4; i++) 
    {
     Ip1Addr[i] = atoi(Ip1Octet[i]);  // atoi converts a string to an integer
     if (Ip1Addr[i] < 0 || Ip1Addr[i] > 255) 
       {
         fprintf(stderr, "Invalid input string (incorrect numbers for IP1 address)\n");
         exit(6);
       }
     Ip2Addr[i] = atoi(Ip2Octet[i]);  // atoi converts a string to an integer
     if (Ip2Addr[i] < 0 || Ip2Addr[i] > 255) 
       {
         fprintf(stderr, "Invalid input string (incorrect numbers for IP2 address)\n");
         exit(7);
       }
     NetMask[i] = atoi(MaskOctet[i]);  // atoi converts a string to an integer
     if (NetMask[i] < 0 || NetMask[i] > 255) 
       {
         fprintf(stderr, "Invalid input string (incorrect numbers for NetMask)\n");
         exit(8);
       }
    }

  fprintf(stderr, " IP1 %d %d %d %d, \n IP2 %d %d %d %d,\n NetMask %d %d %d %d \n", 
	  Ip1Addr[0], Ip1Addr[1], Ip1Addr[2], Ip1Addr[3],
	  Ip2Addr[0], Ip2Addr[1], Ip2Addr[2], Ip2Addr[3],
          NetMask[0], NetMask[1], NetMask[2], NetMask[3]);

  for (i=0; i<4; i++)
    { if ((Ip1Addr[i] & NetMask[i]) != (Ip2Addr[i] & NetMask[i]))
       {
         fprintf(stderr, "Octet %d differs: Ip1Mask %d, Ip2Mask %d\n ", 
                         i, Ip1Addr[i] & NetMask[i], Ip2Addr[i] & NetMask[i]);
         std::cout << "1" << std::endl;
         exit(1);
       }
    }

  fprintf(stderr, "IpMask %d %d %d %d \n", 
                 Ip1Addr[0] & NetMask[0], Ip1Addr[1] & NetMask[1], 
                 Ip1Addr[2] & NetMask[2], Ip1Addr[3] & NetMask[3]);

  std::cout << "0" << std::endl;
  return(0);
}
