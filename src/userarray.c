#include <stdio.h>
#include <unistd.h>
void array_set(int index, int value){
  printf("Setting array[%d] to %d\n", index, value);
}

void array_read(int index){
  printf("Reading array[%d]\n", index);
}

int main(){
  while(1){
    sleep(3);
    array_set(10,100);
    sleep(3);
    array_read(10);
    }
  return 0;
}