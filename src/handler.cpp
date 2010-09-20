#include <sunpinyin.h>
#include "handler.h"

void FcitxWindowHandler::updatePreedit(const IPreeditString* ppd)
{
    TIConvSrcPtr src = (TIConvSrcPtr) (ppd->string());
    memset(front_src, 0, BUF_SIZE * sizeof(TWCHAR));
    memset(end_src, 0, BUF_SIZE * sizeof(TWCHAR));
    
    memcpy(front_src, src, ppd->caret() * sizeof(TWCHAR));
    memcpy(end_src, src + ppd->caret() * sizeof(TWCHAR), 
           (ppd->size() - ppd->caret() + 1) * sizeof(TWCHAR));
    
    memset(buf_, 0, BUF_SIZE);
    
    WCSTOMBS(buf_, front_src, BUF_SIZE - 1);
    buf_[strlen(buf_)] = '|';
    WCSTOMBS(&buf_[strlen(buf_)], end_src, BUF_SIZE - 1);
    
    printf("%s\n", buf_);
}

void FcitxWindowHandler::updateCandidates(const ICandidateList* pcl)
{
    wstring cand_str;
    for (int i = 0, sz = pcl->size(); i < sz; i++) {
        const TWCHAR* pcand = pcl->candiString(i);
        if (pcand == NULL) break;
        cand_str += (i == 9) ? '0' : TWCHAR('1' + i);
        cand_str += TWCHAR('.');
        cand_str += pcand;
        cand_str += TWCHAR(' ');
    }

    TIConvSrcPtr src = (TIConvSrcPtr)(cand_str.c_str());
    WCSTOMBS(buf_, (const TWCHAR*) src, BUF_SIZE - 1);

    printf("%s\n", buf_);
}

void FcitxWindowHandler::commit(const TWCHAR* str)
{
    memset(buf_, 0, BUF_SIZE);
    WCSTOMBS(buf_, str, BUF_SIZE - 1);
    if (eim != NULL) {
        printf("%s\n", buf_);
    }
}
