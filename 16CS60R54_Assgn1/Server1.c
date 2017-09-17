#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"
#include<sys/ipc.h>
#include<sys/shm.h>
#include<signal.h>
#include "SearchPattern.h"


#define PORT 2224
#define BUF_SIZE 2000
#define CLADDR_LEN 100

/**
   int split(char *buff, char ch,  char *text, char *pattern)
   it splits buffer into first occurrence of ch and left side is transferred into text
   and right into pattern.
   it returns number of occurrences of ch into buffer.
*/
int split(char *buff , char ch , char *text , char *pattern)
{
   memset(text, 0, BUF_SIZE);
   memset(pattern, 0, 10);
   char *ptr = strchr(buff , ch);
   
   int i;
   
   if(ptr == NULL)
     return 0;
   for(i = 0 ; (buff+i) != ptr ; i++)
      text[i] = buff[i];
   text[i] = '\0';   
   int k = 0;
   int counter = 1;
   for(++i; i < strlen(buff) ; i++)
   {
      if(buff[i] == ch)
        counter++;
      pattern[k++] = buff[i];
   }
   pattern[k] = '\0';  
   char *s = trimwhitespace(pattern);     
   strcpy(pattern , s);  
   return counter;
}

/** 
  Error codes sent to client 
  -1 : pattern containing other than a-z A-Z
  -3 : text contains other than a-z A-Z and space
  -4 : pattern missing
  -5 : text missing
  -6 : text length exceeding
  -7 : pattern word length exceeding
  0  : on success
*/

int checkSpecification(char *text, char *pattern)
{
    int i ;
    int m = strlen(text);
    int n = strlen(pattern);
    if(m == 0) return -5;
    if(n == 0) return -4;
    if(m > 30) return -6;
    if(n > 5) return -7;
    
    for(i=0; i < m ; i++)
    {
       char ch = text[i];
       if(!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == ' '))
         return -3;
    }
    for(i = 0 ; i < n ; i++)
    {
       char ch = pattern[i];
       if(!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')))
         return -1;
    }
    return 0;
}

int sockfd, newsockfd;
/** 
     void sig_handler1(int signum)
     is close the socket on ^C hit.
*/
void sig_handler1(int signum)
{
    printf( "Attempt to Close the connection\n");
    close(sockfd);
    close(newsockfd);
    exit(1);
}


int main() 
{
  struct sockaddr_in addr, cl_addr;
  
  int ret, len;
  char buffer[BUF_SIZE];
  pid_t childpid;
  char clientAddr[CLADDR_LEN];
  int ShmID;
  int *NoOfClients;
  ShmID = shmget(IPC_PRIVATE , sizeof(int) , IPC_CREAT | 0666);
  NoOfClients = (int *)shmat(ShmID , NULL , 0);
  signal(SIGINT, sig_handler1);
  if(NoOfClients == NULL)
  {
     printf("Error! in creating shared variable");
     exit(0);
  }
  
  /**
    NoOfClients is a shared variable to handle number of active clients connected to 
    server
  */
  char *text, *pattern;
  text = (char *) malloc(BUF_SIZE * sizeof(char));
  pattern = (char *) malloc(10 * sizeof(char));
  
  /**
      to create socket.
  */ 
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    printf("Error creating socket!\n");
    exit(1);
  }
  printf("Socket created...\n");
 
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = PORT;
  /**
    To bind the server to client on given port
  */
  ret = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
  if (ret < 0) 
  {
    printf("Error binding!\n");
    exit(1);
  }
  printf("Binding done\n");

  printf("Waiting for a connection\n");
  listen(sockfd, 5);
  *NoOfClients = 0;
  
  /**
     Server runs infinite to serve to client.
  */
  for (;;)
  { //infinite loop
    
    len = sizeof(cl_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cl_addr, &len);
    if (newsockfd < 0)
    {
       printf("Error accepting connection!\n");
       exit(1);
    }
    
    (*NoOfClients)++;
    printf("Connection Accepted,  No of Active Clients = %d\n", (*NoOfClients));
    
    inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, CLADDR_LEN);
    /**
       For every connected client a new thread is created(child process) for
       communication using fork system call
    */
    if ((childpid = fork()) == 0)
    { //creating a child process
        // signal(SIGINT, sig_handler1);
         close(sockfd); 
      //stop listening for new connections by the main process. 
      //the child will continue to listen. 
      //the main process now handles the connected client.

      for (;;)
      {
        memset(buffer, 0, BUF_SIZE);
        ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);
         if(ret < 0)
         { 
           printf("Error receiving data!\n");  
           exit(1);
         }
         printf("Received data from %s: %s\n", clientAddr, buffer); 
         if(strcmp(buffer , "exit") == 0)
         {
            
            close(newsockfd);
            printf("Connection is closed,  No of Active Clients = %d\n", --(*NoOfClients));
            return 0;
         }
         
         int no = split(buffer , ';', text , pattern);
         if(no == 0)
            strcpy(buffer , "Error! Semicolon missing");
         else if(no > 1)  
            strcpy(buffer , "Error! Multiple semicolons");
          else
          {   
            printf("Text is : %s \n Pattern is : %s\n" , text , pattern);
            int errorNo = checkSpecification(text , pattern);
             printf("Text is : %s \n Pattern is : %s\n" , text , pattern);
            if(errorNo != 0)
            {
             if(errorNo == -1)
                strcpy(buffer , "Error! Pattern contains other than a-zA-Z");
             else if(errorNo == -3)
                strcpy(buffer , "Error! Text contains other than a-zA-Z and space");
             else if(errorNo == -4)
                strcpy(buffer , "Error! Pattern is missing");
             else if(errorNo == -5)
                strcpy(buffer, "Error! Text is missing");
             else if(errorNo == -6)
                 strcpy(buffer, "Error! Text length exceeding 30 charachters");
             else if(errorNo == -7)
                strcpy(buffer, "Error! Pattern length exceeding 5 characters");  
            }
            else
            {
              int noOfOccurrences = Naive(text , pattern);
              char *t = (char *) malloc(strlen(text) * sizeof(char));
              strcpy(t, text);
              int noOfMatchWords = countMatchWords(t , pattern);
              printf("Text is : %s \nPattern is : %s\n" , text , pattern);
              sprintf(buffer , "Text : %s \n Pattern : %s \n NO of Occurrences as Substring = %d\n No of Occurrences as Words = %d \n",text, pattern, noOfOccurrences , noOfMatchWords);  
             }
          } 
           ret = sendto(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);   
           if (ret < 0)
           {  
              printf("Error sending data!\n");  
              exit(1);  
           }  
           printf("Sent data to %s: %s\n", clientAddr, buffer);
        }
      
      exit(0);
    }
   close(newsockfd);
 }
 return 0;
}

