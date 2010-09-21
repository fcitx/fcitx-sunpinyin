#include <sunpinyin.h>
#include "handler.h"

void FcitxWindowHandler::updatePreedit(const IPreeditString* ppd)
{
    char *buf_ = eim->CodeInput;
    TIConvSrcPtr src = (TIConvSrcPtr) (ppd->string());
    memset(front_src, 0, BUF_SIZE * sizeof(TWCHAR));
    memset(end_src, 0, BUF_SIZE * sizeof(TWCHAR));
    
    memcpy(front_src, src, ppd->caret() * sizeof(TWCHAR));
    memcpy(end_src, src + ppd->caret() * sizeof(TWCHAR), 
           (ppd->size() - ppd->caret() + 1) * sizeof(TWCHAR));
    
    memset(buf_, 0, MAX_USER_INPUT + 1);
    
    WCSTOMBS(buf_, front_src, MAX_USER_INPUT);
    buf_[strlen(buf_)] = '|';
    WCSTOMBS(&buf_[strlen(buf_)], end_src, MAX_USER_INPUT);
}

void FcitxWindowHandler::updateCandidates(const ICandidateList* pcl)
{
    wstring cand_str;
    for (int i = 0, sz = pcl->size(); i < sz; i++) {
        const TWCHAR* pcand = pcl->candiString(i);
        cand_str = pcand;
        TIConvSrcPtr src = (TIConvSrcPtr)(cand_str.c_str());
        WCSTOMBS(eim->CandTable[i], (const TWCHAR*) src, MAX_CAND_LEN);
    }

    eim->CandWordCount = pcl->size();
}

void FcitxWindowHandler::commit(const TWCHAR* str)
{
    char *buf_ = eim->StringGet;
    memset(buf_, 0, MAX_USER_INPUT);
    WCSTOMBS(buf_, str, MAX_USER_INPUT);
    commit_flag = true;
}
