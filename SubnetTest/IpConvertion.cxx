#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
// read an IP address in binary form
// store it into an array of 4 strings of 8 chars each
// return 1 if read successfully or 0 if an error occurred
  int readBinary(char bin[4][8]) {
  char binString[80];
  int i, j;
  int nextChar = 0;
  int len;
 
// read line
  printf("Enter 32-bit IP address in binary form (xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx): ");
  fflush(stdout); //flushes the output, it is needed on some version of linux/unix before scanf
  scanf("%[^\r\n]", binString);
 
// store binary digits into bin array
  len = strlen(binString);
  for (i=0; i<4; i++) {
    if (nextChar >= len) {
      printf("Invalid input string (too short)\n");
      return 0;
    }
    for (j=0; j<8; j++) {
// skip spaces
      while (nextChar < len && binString[nextChar] == ' ')
        nextChar++;
// end of string? break the loop
      if (nextChar >= len) {
        printf("Invalid input string (too short)\n");
        return 0;
      }
      bin[i][j] = binString[nextChar];
      nextChar++;
// only 0 and 1 accepted
      if (bin[i][j] != '0' && bin[i][j] != '1') {
        printf("Invalid input string (not a binary number)\n");
        return 0;
      }
    }
  }
 
// return 1 (OK)
  return 1;
}
 
// read an IP address in decimal dotted form
// return 1 if ok or 0 if an error occurs
  int readIPAddress(int ipaddr[]) {
  char decstring[4][4];
  int i;
 
// read IP address as strings
  printf("Enter 32-bit IP address in dotted decimal notation (xxx.xxx.xxx.xxx): ");
  fflush(stdout);
  if (scanf("%[^.].%[^.].%[^.].%[^\r\n]", decstring[0], decstring[1], decstring[2], decstring[3]) < 4) {
    printf("Invalid input string (wrong IP address format)\n");
    return 0;
  }
 
// convert each string to integer
  for (i=0; i<4; i++) {
    ipaddr[i] = atoi(decstring[i]);  // atoi converts a string to an integer
    if (ipaddr[i] < 0 || ipaddr[i] > 255) {
      printf("Invalid input string (incorrect numbers for IP address)\n");
      return 0;
    }
  }
 
  return 1;
}
 
// read an IP address slash netmask length (CIDR format)
// return 1 if ok or 0 if an error occurs
int readIPSlashAddress(int ipaddr[], int *slash) {
  char decstring[4][4];
  char slashstring[3];
  int i;
 
// read IP address as strings
  printf("Enter IP-address in CIDR notation (xxx.xxx.xxx.xxx/xx): ");
  fflush(stdout);
  if (scanf("%[^.].%[^.].%[^.].%[^/]/%[^\r\n]", decstring[0], decstring[1], decstring[2], decstring[3], slashstring) < 5) {
    printf("Invalid input string (wrong IP address format)\n");
    return 0;
  }
 
// convert each string to integer
  for (i=0; i<4; i++) {
    ipaddr[i] = atoi(decstring[i]);
    if (ipaddr[i] < 0 || ipaddr[i] > 255) {
      printf("Invalid input string (incorrect numbers for IP address)\n");
      return 0;
    }
  }
 
  *slash = atoi(slashstring);  // a variable and it stores the netmask-length
  if (*slash < 0 || *slash > 32) {
    printf("Invalid input string (invalid netmask length)\n");
    return 0;
  }
 
  return 1;
}
 
// read an IP address slash netmask
// return 1 if ok or 0 if an error occurs
  int readIPMaskAddress(int ipaddr[], int mask[]) {
  char decstring[4][4];
  char maskstring[4][4];
  int i;
 
// read IP address as strings
  printf("Enter IP-address/subnet you want converted to CIDR slash form (xxx.xxx.xxx.xxx/xxx.xxx.xxx.xxx.): ");
  fflush(stdout);
  if (scanf("%[^.].%[^.].%[^.].%[^/]/%[^.].%[^.].%[^.].%[^\r\n]", decstring[0], decstring[1], decstring[2], decstring[3],
      maskstring[0], maskstring[1], maskstring[2], maskstring[3]) < 8) {
    printf("Invalid input string (wrong IP address/netmask format)\n");
    return 0;
  }
 
// convert each string to integer
   for (i=0; i<4; i++) {
    ipaddr[i] = atoi(decstring[i]);
    if (ipaddr[i] < 0 || ipaddr[i] > 255) {
      printf("Invalid input string (incorrect numbers for IP address)\n");
      return 0;
    }
  }
 
// convert each string to integer
   for (i=0; i<4; i++) {
    mask[i] = atoi(maskstring[i]);
    if (mask[i] < 0 || mask[i] > 255) {
      printf("Invalid input string (incorrect numbers for netmask)\n");
      return 0;
    }
  }
  return 1;
}
 
void convertBinToDec(char bin[4][8], int dec[]) {
 
  int i, j;
 
// convert each binary group to decimal number
  for (i=0; i<4; i++) {
    dec[i] = 0;
    for (j=0; j<8; j++) {
      dec[i] *= 2;
      if (bin[i][j] == '1')
        dec[i]++;
    }
  }
 
}
 
void convertDecToBin(int dec[], char bin[4][9]) {
 
  int i, j;
 
// convert each decimal group to binary
  for (i=0; i<4; i++) {
    for (j=7; j>=0; j--) {
      bin[i][j] = (dec[i] & 1) + '0';
      dec[i] /= 2;
    }
    bin[i][8] = 0;
  }
 
}
 
// show network and host parts
void showNetworkHost(int ipaddr[], int numNetwork) {
  int i;
 
  printf("Network portion is: ");
  for (i=0; i<=numNetwork; i++)
    printf("%d.", ipaddr[i]);
  printf("\n");
 
  printf("Host portion is: ");
  for (i=numNetwork+1; i<4; i++)
    printf(".%d", ipaddr[i]);
  printf("\n");
}
 
void convertCIDRToNetmask(int slash, int mask[]) {
 
// create binary mask first, then convert to decimal
  char bin[4][8];
  int i, j;
 
// write 1's while slash is positive, then write 0's
  for (i=0; i<4; i++)
    for (j=0; j<8; j++) {
      if (slash > 0) {
        bin[i][j] = '1';
        slash--;
      }
      else
        bin[i][j] = '0';
    }
 
// convert to decimal
  convertBinToDec(bin, mask);
}
 
  int convertNetmaskToCIDR(int mask[], int *slash) {
// create binary mask first, then convert to slash form
  char bin[4][9];
  int i, j;
  char lastChar;
 
  convertDecToBin(mask, bin);
 
// add to slash while 1's are found
  lastChar = '1';
  *slash = 0;
  for (i=0; i<4; i++)
    for (j=0; j<8; j++) {
      if (bin[i][j] == '1') {
// once a zero is found - no more 1's should appear
        if (lastChar == '0') {
          printf("Cannot convert netmask to CIDR notation\n");
          return 0;
        }
        (*slash)++;
      }
      lastChar = bin[i][j];
    }
 
  return 1;
}
 
int main()
{
  int choice;
  char bin[4][8];
  char bin2[4][9];
  int ipaddr[4];
  int mask[4];
  int slash;
 
// temp string, used when reading - used to skip any remaining input after reading
// a menu option in the main menu.  The main menu scanf reads an integer.  If a temp
// string wasn't used, the next scanf trying to read a string would read an empty string
  char temp[80];
 
  do
  {
    printf("\nSelect from one of the choices:\n\n");
    printf("1) Convert from 32 bit binary ip form to dotted decimal ip form\n");
    printf("2) Convert from dotted decimal ip form to 32 bit binary ip form\n");
    printf("3) Convert a dotted decimal ip form to its class and display the network and host portions separately\n");
    printf("4) Convert from IP/CIDR slash notation to dotted decimal ip form\n");
    printf("5) Convert from IP/subnet dotted ip form to CIDR slash form\n");
    printf("6) Quit the program\n");
    printf("\nEnter your choice: ");
    fflush(stdout);
    scanf("%d", &choice);
    scanf("%[^\n]", temp);
    scanf("%c", temp);
 
    switch (choice)
    {
      case (1) : // Convert from 32 bit binary ip form to dotted decimal ip form
 
                 if (!readBinary(bin))
                   break;
 
                 convertBinToDec(bin, ipaddr);
 
                 // print the resulting IP
                 printf("The address converted to decimal is: %d.%d.%d.%d\n", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
 
                 break;
 
      case (2) : // Convert from dotted decimal ip form to 32 bit binary ip form
                 if (!readIPAddress(ipaddr))
                   break;
 
                 convertDecToBin(ipaddr, bin2);
 
                 // print the results
                 printf("The address converted to binary is: %8s %8s %8s %8s\n", bin2[0], bin2[1], bin2[2], bin2[3]);
                 break;
 
      case (3) : // Convert a dotted decimal ip form to its class
                 if (!readIPAddress(ipaddr))
                   break;
 
                 if (ipaddr[0] < 128) {
                   printf("This address belongs to class A\n");
                   showNetworkHost(ipaddr, 0);
                 }
                 else if (ipaddr[0] < 192) {
                   printf("This address belongs to class B\n");
                   showNetworkHost(ipaddr, 1);
                 }
                 else if (ipaddr[0] < 224) {
                   printf("This address belongs to class C\n");
                   showNetworkHost(ipaddr, 2);
                 }
                 else if (ipaddr[0] < 240)
                   printf("This address belongs to class D\n");
                 else
                   printf("This address belongs to class E\n");
 
                 break;
      case (4) : // Convert from IP CIDR slash notation of to dotted decimal ip form
                 if (!readIPSlashAddress(ipaddr, &slash))
                   break;
 
                 convertCIDRToNetmask(slash, mask);
 
                 printf("IP/Netmask in dotted decimal IP form: %d.%d.%d.%d/%d.%d.%d.%d\n",
                     ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3], mask[0], mask[1], mask[2], mask[3]);
 
                 break;
 
      case (5) : // Convert from IP/subnet form to CIDR slash form
                 if (!readIPMaskAddress(ipaddr, mask))
                   break;
 
                 if (!convertNetmaskToCIDR(mask, &slash))
                   break;
 
                 printf("IP/Netmask-length in CIDR notation: %d.%d.%d.%d/%d",
                     ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3], slash);
 
                 break;
 
      case (6) : system("clear"); //clear program
                 exit(0); // close program
 
      default  : printf("I don't know the option %d.\n", choice);
                 printf("Try again.\n");
                 break;
    }
  } while(1);
}
