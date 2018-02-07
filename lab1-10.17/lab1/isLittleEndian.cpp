#include <iostream>

bool IsLitte_Endian();

int main() {
    if (IsLitte_Endian())
        std::cout << "小端" << std::endl;
    else
        std::cout << "大端" << std::endl;
    return 0;
}

bool IsLitte_Endian() {
    int wTest = 0x12345678;
    short *pTest = (short *) &wTest;
    return !(0x1234 == pTest[0]); //若处理器是Big_endian的，则返回false；若是Little_endian的，则返回true。
}