#include "fvm/MasterFVM/masterFVM.h"
#include<gtest/gtest.h>
int add(int a,int b){
    return a+b;
}
TEST(masterFVMtest,testInit){
    trip[0] = 255;
    trip[1] = 255;
    MasterFVM_Init();
    EXPECT_EQ(trip[0],0);
    EXPECT_EQ(trip[1],1);
    EXPECT_EQ(TripCntLength,1);

    trip[1] = 255;
    trip[0] = 133;
    MasterFVM_Init();
    EXPECT_EQ(trip[0],134);
    EXPECT_EQ(trip[1],0);
    EXPECT_EQ(TripCntLength,8+8);

    trip[1] = 2;
    trip[0] = 3;
    MasterFVM_Init();
    EXPECT_EQ(trip[0],3);
    EXPECT_EQ(trip[1],3);
    EXPECT_EQ(TripCntLength,8+2);

}






