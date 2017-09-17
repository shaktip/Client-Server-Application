/*Pattern Search in a given Text*/
//1. Naive Approach
#include<malloc.h>
#define NoOfChars 256

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(*str == ' ') str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && *end == ' ') end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

int Naive(char *text , char *pattern)
{
   int N = strlen(text);
   int M = strlen(pattern);
   int count = 0;
   int i;
   for(i = 0 ; i <= N - M ; i++)
   {
        int j;
        for(j = 0 ; j < M ;j++)
           if(text[i+j] != pattern[j])
              break;
        if(j == M)
          count++;      
   }
   return count;
}
void fillTransitionTable(int  **TT , char *pattern , int NoOfStates)
{
   int i , j,k,ns;
   int N = strlen(pattern);
   for(i = 0 ; i < NoOfStates ; i++)
   {
      for(j = 0 ; j < NoOfChars ; j++)
      {
         if( i < N && j == pattern[i])
            TT[i][j] = i+1;
         else if(i == 0)
            TT[i][j] = 0;
         else
         {
            for(ns = i - 1; ns >= 0 ; ns--)
            {
               if(pattern[ns] == j)
               {
                  int len = ns ; int start = i - ns;
                  for(k = 0 ; k < len; k++)
                  {
                       if(pattern[k] != pattern[start++])
                          break;
                  }
                  //printf("\n\t i = %d j = %d ns = %d k = %d len = %d",i,j,ns,k,len);
                  if(k == len)
                  {
                    TT[i][j] = len + 1;
                    break;
                  }
               }
            }
            if(ns < 0)
                TT[i][j] = 0;
         }      
            
      }
   }
}
int DFASearch(char *text , char *pattern)
{
   int M = strlen(text);
   int N = strlen(pattern);
   int NoOfStates = N + 1;
   int i,j;
   int **TransitionTable;
   TransitionTable = (int **)malloc((NoOfStates) * sizeof(int *));
   for(i = 0 ; i < N+1 ; i++)
      TransitionTable[i] = (int *) malloc (NoOfChars * sizeof(int));
      
   fillTransitionTable(TransitionTable, pattern , NoOfStates);
   int count = 0 ;
   int state = 0; 
   /*
   for(i = 0 ; i < NoOfStates ; i++)
   {
     printf("\nState%d ", i);
     for(j = 0 ; j < NoOfChars ;j++)
       printf(" %d",TransitionTable[i][j]);
   }
   */
   for(i = 0 ; i < M ;i++)
   {
     state = TransitionTable[state][text[i]];
     //printf("\n\t state is : %d and N is %d",state, N);
     if(state == N )
       count++;
   }
   for(i = 0 ; i < N + 1; i++)
     free(TransitionTable[i]);
   free(TransitionTable);
   return count;
}
int countMatchWords(char *text, char *pattern)
{
   char *t = text;
   char *tok = strtok(t , " \t");
   int count = 0;
   while(tok)
   {
       if(strcmp(tok , pattern) == 0)
          count++;
       tok = strtok(NULL , " \t");   
   }
   return count;
}

void constructLPS(char *pat , int LPS[])
{
   int N = strlen(pat) ; LPS[0] = 0;
   int i,j,k,p;
   for(i = 1; i < N ; i++)
   {
      for(j = i - 1; j >= 0 ; j--)
      {
         if(pat[i] == pat[j])
         {
            int len = j + 1;
            int start = i - j;
            for(p = 0 ; p < len ; p++)
               if(pat[p] != pat[start++])
                  break;
            if(p == len)
            {LPS[i] = len ; break;}      
         }
      }
      if(j < 0)
         LPS[i] = 0;
   }
}

int KMP(char *text , char *pattern)
{
   int M = strlen(text);
   int N = strlen(pattern);
   int *LPS;
   LPS = (int *) malloc ( N * sizeof(int));
   constructLPS(pattern , LPS);
   
   int i, j = 0, count = 0;
   /*
   printf("\n\t");
   for( i = 0 ; i < N ;i++)
     printf(" %d\n" , LPS[i]);
   */  
   for(i = 0 ; i < M ; )
   {
       
      if(text[i] == pattern[j])
      {
        i++; j++; 
        if(j == N) { count++; j = LPS[j-1];}
        continue;
      }
      
      
      if(j > 0) { j = LPS[j-1]; continue;}
      i++;
   }
   return count;
}

#define d 256
#define q 257
int RabinKarp(char *text , char *pattern)
{
   int M = strlen(text);
   int N = strlen(pattern);
   int h = 1;
   int i, j;
   for(i = 0 ; i < N-1; i++)
      h = (h * d) % q;
   int t = 0, p = 0;   
   for(i = 0 ; i < N ;i++)
   {
      p = (p * d + pattern[i]) % q;
      t = (t * d + text[i]) % q;
   }   
   //printf("pattern = %s text = %s p = %d t = %d \n",pattern ,text , p , t);
   int count = 0;
   for(i = 0 ; i <= M - N ; i++)
   {
      //printf("pattern = %s text = %s p = %d t = %d \n",pattern ,text , p , t);
      if(p == t)
      {
         printf("p=%d  t=%d",p,t);
         for(j = 0 ; j < N ; j++)
         {
             if(pattern[j] != text[i + j])
               break;
         }
         if(j == N)
           count++;
      }
      if(i < M-N)
      {
         t =  (d * (t - h * text[i]) + text[i+N]) % q;
         
         if(t < 0)
           t = t + q;
      }
   }
   return count;   
}



