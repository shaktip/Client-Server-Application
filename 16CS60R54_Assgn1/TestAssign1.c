#include<stdio.h>
#include<string.h>
#include"stdlib.h"
#include "SearchPattern.h"
/**
  SearchPattern.h contains
  following methods which returns no of occurrences of pattern in text
  0. char *trimwhitespace(char *str)
  1. int Naive(char *text , char *pattern)
  2. int DFASearch(char *text , char *pattern)
  3. int countMatchWords(char *text, char *pattern)
  
*/
/**
   buff is seperated on the basis of first ;
   
*/
#define BUF_SIZE 1034

/**
   int split(char buff[] , char ch , char text[], char pattern[])
      Returns number of ch found
      splits buff on the basis of first ch
      left portion of first ch is into text
      right portion of first ch is into pattern.
*/

int split(char *buff , char ch , char *text , char *pattern)
{
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
   return counter;
}
/** 
  Error codes received by client 
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


int main()
{
    char *buffer , *text, *pattern;
    buffer = (char *) malloc(BUF_SIZE * sizeof(char));
    text = (char *) malloc(BUF_SIZE * sizeof(char));
    pattern = (char *) malloc(10 * sizeof(char));
    
    printf("Enter string in following form \n text;pattern\n");
    fflush(stdin);
    fgets(buffer, BUF_SIZE, stdin);
    int i;
    for(i = 0 ; i < strlen(buffer) && buffer[i] != '\n' ; i++);
    buffer[i] = '\0';
    //buffer = trimwhitespace(buffer);
    printf("Buffer is %s and its length is %d\n",buffer, (int)strlen(buffer));
    
    int no = split(buffer , ';', text , pattern);
    if(no == 0 || no > 1)
    {
       printf("One semicolon(only)\n");
       return 0;
    }    
    //text = trimwhitespace(text);
    //pattern = trimwhitespace(pattern);
    printf("Text is : %s \n Pattern is : %s\n" , text , pattern);
    int errorNo = checkSpecification(text , pattern);
    if(errorNo != 0)
    {
       if(errorNo == -1)
         printf("Error! Pattern contains other than a-zA-Z\n");
       else if(errorNo == -3)
         printf("Error! Text contains other than a-zA-Z and space \n");
       else if(errorNo == -4)
         printf("Error! Pattern is missing");
       else if(errorNo == -5)
         printf("Error! Text is missing");
       else if(errorNo == -6)
         printf("Error! Text length exceeding 30 charachters");
       else if(errorNo == -7)
          printf("Error! Pattern length exceeding 5 characters");
          
    }
    else
    {
      int NoOfOccur;
      NoOfOccur = Naive(text , pattern);
      printf("No of occurrences (Naive) = %d \n", NoOfOccur);
      NoOfOccur = DFASearch(text , pattern);
      printf("No of occurrences (DFASearch) = %d \n", NoOfOccur);
      NoOfOccur = KMP(text , pattern);
      printf("No of occurrences (KMP) = %d \n", NoOfOccur);
      NoOfOccur = RabinKarp(text , pattern);
      printf("No of occurrences (RabinKarp) = %d \n", NoOfOccur);
      NoOfOccur = countMatchWords(text , pattern);
      printf("word count is = %d \n" , NoOfOccur);
    }
    return 0;
}



