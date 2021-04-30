#include "fvm/SlaveFVM//slaveFVM.h"
#include<gtest/gtest.h>
int add(int a,int b){
    return a+b;
}
TEST(testCase,test0){
    EXPECT_EQ(add(2,3),5);
}

