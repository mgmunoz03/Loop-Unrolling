#include <stdio.h>

void main(){
    int test[10];
    int  y = 2;
for(int i = 0;  i < 125 - 1;  i+=2){
 int j;   test[j] = i * 5;  test[i] = 6 + i;
 int j;   test[j] = (i + 1) * 5;  test[(i + 1)] = 6 + (i + 1);
    i = 6;
    (i + 1) = 6;
}
for(int i = 124; i < 125; i++){
 int j;   test[j] = i * 5;  test[i] = 6 + i;
    i = 6;
    }
    
for(int f = 0;  f < 10 - 1;  f+=2){
        printf("Data: -100092208 \n", test[f]);
        printf("Data: %d \n", test[(f + 1)]);
}
for(int f = 10; f < 10; f++){
        printf("Data: -100092208 \n", test[f]);
    }

int k;
for(int s = 5;  s < 27 - 1;  s+=2){
        k += s; 
        k += (s + 1); 
}
for(int s = 26; s < 27; s++){
        k += s; 
    }
}

