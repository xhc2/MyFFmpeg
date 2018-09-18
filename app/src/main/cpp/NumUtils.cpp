//
// Created by Administrator on 2018/9/6/006.
//

#include "NumUtils.h"
#include "my_log.h"

//初始化
NumUtils *NumUtils::numUtils = NULL;

int NumUtils::array2Int(char *array, const int start, const int size) {

    char temp[size];
    int tempCount = 0;
    int result = 0;
    int copySize = size;
    int removeSize = 0;

    for (int i = start; i < size + start; ++i) {
        if (array[i] == 0) {
            removeSize++;
        } else {
            break;
        }
    }
    copySize -= removeSize;
    char newArray[copySize];
    memcpy(newArray, array + removeSize + start, copySize);

    //高低翻转下
    for (int i = size - removeSize - 1; i >= 0; --i) {
        temp[tempCount] = newArray[i];
        tempCount++;

    }
    memcpy(&result, temp, copySize);
    return result;
}


//这里不加static
NumUtils *NumUtils::getInstance() {
    if (numUtils == NULL) {
        numUtils = new NumUtils();
    }
    return numUtils;
}

//读取字节
void NumUtils::bitRead(int result) {
    int temp = 1;
    string str;
    for (int i = 0; i < 32; ++i) {
        int a = temp & result;
        result = result >> 1;
        str.append(int2String(a));
    }
    LOGE(" %s ", str.c_str());
}


string NumUtils::int2String(int num) {
    stringstream stream;
    stream << num;
    string str = stream.str();
    stream.clear();
    return str;
}

string NumUtils::double2String(double num) {
    stringstream stream;
    stream << num;
    string str = stream.str();
    stream.clear();
    return str;
}

double NumUtils::array2Double(char *array, int start) {

    double result = 0;
    int doubleSize = 8;
    char *resultArray = (char *) malloc(doubleSize);
    int j = start + doubleSize - 1;
    for (int i = 0; i < doubleSize; ++i) {
        resultArray[i] = array[j];
        --j;

    }
    memcpy(&result, resultArray, doubleSize);

    return result;

}

//是从高位开始判断的
u_int NumUtils::u(char *bs, char bitCount) {
    u_int val = 0;
    int pos = 0;
    for (int i = 0; i < bitCount; i++) {
        val <<= 1;
        if (bs[pos / 8] & (0x80 >> (pos % 8))) {     //计算index所在的位是否为1
            val |= 1;
        }
        pos++;
    }
    return val;
}


/**
 *
 * @param param
 * @return
 *  1 对应值 0
 *  0 1 x  对应 1,2
 *  0 0 1 x x 对应3-6
 *  前面0的个数就是后面信息位（x）的个数
 *  值的计算是 = 2 ^ 0 位数 - 1+ x（xxx的十进制表示方式）
 *  还没写完。先不管
 */
u_int NumUtils::columbusCoding(char *param) {
    u_int zeroNum = 0;

    while (u(param, 1) == 0 && zeroNum < 32) {

        zeroNum++;
    }
    LOGE(" 0 count %d" , zeroNum);
    return (u_int) ((1 << zeroNum) - 1 + u(param, zeroNum));
}

NumUtils::NumUtils() {

}

