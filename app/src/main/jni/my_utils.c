//
// Created by Administrator on 2018/2/3/003.
//

#include "My_LOG.h"
#include "my_utils.h"


//此函数在锤子手机m1上测试成功
void utils_nv21ToYv12(jbyte *navtiveYuv ,int y_size){

    if(navtiveYuv){
        int length = y_size * 3 / 2;
        jbyte *result = malloc(sizeof(jbyte) * length);
        memset(result ,0 , sizeof(jbyte) *  length);
        memcpy(result , navtiveYuv , y_size);
        int uCount = (length - y_size) / 2;
        for(int i = y_size , uPosition = (y_size+uCount) , vPosition = y_size ;i < length ; i+= 2 , uPosition++ , vPosition++){
            result[vPosition] =  navtiveYuv[i + 1]; //u
            result[uPosition] = navtiveYuv[i]; //v
        }
        memcpy(navtiveYuv, result  , length);
        free(result);
    }
}


void myThreadStart(){

}

void myThreadStop(){

}