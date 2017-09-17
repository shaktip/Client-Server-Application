#include"stdio.h"  
#include"stdlib.h"  
#include"sys/types.h"  
#include"sys/socket.h"  
#include"string.h"  
#include"netinet/in.h"  
#include"netdb.h"
#include<signal.h>
  
#define PORT 2224
#define BUF_SIZE 2000 
int sockfd;
/**
    sig_handler1(int signum)
    is used to close the socket on ^C
*/
void sig_handler1(int signum)
{
    printf( "\nAttempt to Close the connection\n");
    close(sockfd);
    //close(newsockfd);
    exit(1);
}

/**
   char *trimWhitespace(char *str)
   is used to remove leading and trailing spaces from str.
*/
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

/**
     int getMessage(char *buffer)
     it accepts the message of the form text;pattern
     and returns 1 if message is exit otherwise 0
*/
int getMessage(char *buffer)
{
   int i;
   memset(buffer, 0, BUF_SIZE);
   printf("Enter your message(type exit to close connection)\n ");
   fflush(stdin);
   fgets(buffer, BUF_SIZE, stdin);
   for(i = 0 ; i < strlen(buffer) && buffer[i] != '\n' ; i++);
       buffer[i] = '\0';
   buffer = trimwhitespace(buffer);
   if(strcmp(buffer , "exit") == 0)
      return 1;
   return 0;       
}
  
  
int main(int argc, char**argv)
{  
    struct sockaddr_in addr, cl_addr;  
    int  ret, i;  
   
    struct hostent * server;
    char * serverAddr;
    signal(SIGINT, sig_handler1); 
    //If user does not give host address in command line.
    if (argc < 2) 
    {
      printf("supply ip address of server(localhost-127.0.0.1) : ");
      serverAddr = (char *) malloc(sizeof(char) * 100);
      fgets(serverAddr, 100, stdin);
      for(i = 0 ; i < strlen(serverAddr) && serverAddr[i] != '\n' ; i++);
      serverAddr[i] = '\0';
      serverAddr = trimwhitespace(serverAddr);  
    }
    else
    {
      serverAddr = argv[1]; 
    }
    
    if(strcmp(serverAddr , "localhost") == 0)
       strcpy(serverAddr , "127.0.0.1");
       
    char *buffer;
    buffer = (char *) malloc(BUF_SIZE * sizeof(char));
 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  
    if (sockfd < 0) 
    {  
     printf("Error creating socket!\n");  
     exit(1);  
    }  
    printf("Socket created...\n");   

    memset(&addr, 0, sizeof(addr));  
    addr.sin_family = AF_INET;  
    addr.sin_port = PORT;     
    /* Mapping Host name to ip address */
    if((addr.sin_addr.s_addr = inet_addr(serverAddr)) == (unsigned long)INADDR_NONE)
    {
      server = gethostbyname(serverAddr);
      if(server == (struct hostent *)NULL)
      {
         printf("HOST NOT FOUND ");
         close(sockfd);
         exit(-1);  
      }
      memcpy(&addr.sin_addr, server->h_addr, sizeof(addr.sin_addr));
    }  
    /*Connecting to server on the port */  
    ret = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));  
    if (ret < 0)
    {  
     printf("Error connecting to the server!\n");  
     exit(1);  
    }  
   printf("Connection Successful...\n");  

   /**
      Following code takes care to send text;pattern and receiving the
      number of occurrences of pattern in the text.
   */

   while (1) 
   {
       fflush(stdin);
       int closeRequest = getMessage(buffer);
           
       ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));  
      if (ret < 0) 
      {  
        printf("Error sending data!\n\t-%s", buffer);  
      }
      if(closeRequest == 1)
      {
         close(sockfd);
         return 0;
      }
      memset(buffer, 0, BUF_SIZE);
      ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
      
      if (ret < 0 || strlen(buffer) == 0)
      {  
         printf("Error receiving data! Connection Interrupted\n");
         return 0;    
      }
      else
      {
         fputs(buffer, stdout);
         printf("\n");
      }  
   }
 
 return 0;    
}  
