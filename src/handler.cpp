/*  Copyright (C) 2010~2010 by CSSlayer
    wengxt@gmail.com 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <sunpinyin.h>
#include "handler.h"

/**
 * @brief handler called while preedit updated
 *
 * @param ppd preedit string
 * @return void
 **/
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
    eim->CaretPos = strlen(buf_);
    WCSTOMBS(&buf_[strlen(buf_)], end_src, MAX_USER_INPUT);
    candidate_flag = true;
}

/**
 * @brief sunpinyin called this function while updating candidate words
 *
 * @param pcl candidate list
 * @return void
 **/
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

    candidate_flag = true;
}

/**
 * @brief sunpinyin called this function while commit the string
 *
 * @param str committed string
 * @return void
 **/
void FcitxWindowHandler::commit(const TWCHAR* str)
{
    char *buf_ = eim->StringGet;
    memset(buf_, 0, MAX_USER_INPUT);
    WCSTOMBS(buf_, str, MAX_USER_INPUT);
    commit_flag = true;
}
