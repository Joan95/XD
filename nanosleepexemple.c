#include <stdio.h>
#include <time.h>  //llibreria per poder cridar al nanosleep

int main()
{
   struct timespec tim, tim2;
   tim.tv_sec = 1;
   tim.tv_nsec = 500000000;  //valor en nanosegons 0.5 segons


   printf("Abans del nanosleep \n");
   nanosleep(&tim , &tim2);
   printf("Despres del nanosleep \n");

   return 0;
}
