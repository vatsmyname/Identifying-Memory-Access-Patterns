#include <stdio.h>

struct MyStruct {
    int field0;
    int field1;
};
int a;
int myArray[3];

void function1() {
    struct MyStruct myStruct;
    a=10;
    myArray[0] = 5;
    myArray[0] = myStruct.field0;
    myStruct.field1 = a;
}

void function2() {
    int b;
    b=20;
    myArray[0] = b;
    myArray[0] = a;
}

int main() {
    function1();
    function2();
    return 0;
}

