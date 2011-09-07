#include <pinyin/pinyin_data.h>
#include <pinyin/shuangpin_data.h>
#include <iostream>

int main()
{
    const char* pinyin = "ig";
    CShuangpinData shuangpin(MS2003);
    CMappedYin syls;
    shuangpin.getMapString(pinyin, syls);
    if (syls.size() == 1 && syls[0] == "cheng")
        return 0;
    else
        return 1;
}
