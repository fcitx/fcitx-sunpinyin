/***************************************************************************
 *   Copyright (C) 2010~2010 by CSSlayer                                   *
 *   wengxt@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <sunpinyin.h>
#include "handler.h"
#include "eim.h"

extern "C"
{
    #include <fcitx-utils/cutils.h>
}

/**
 * @brief handler called while preedit updated
 *
 * @param ppd preedit string
 * @return void
 **/
void FcitxWindowHandler::updatePreedit(const IPreeditString* ppd)
{
    FcitxInstance* instance = owner->owner;
    FcitxInputState* input = &instance->input;
    instance->bShowCursor = true;
    TIConvSrcPtr src = (TIConvSrcPtr) (ppd->string());
    
    memcpy(front_src, src, ppd->caret() * sizeof(TWCHAR));
    memcpy(end_src, src + ppd->caret() * sizeof(TWCHAR), 
           (ppd->size() - ppd->caret() + 1) * sizeof(TWCHAR));
    
    front_src[ppd->caret()] = 0;
    end_src[ppd->size() - ppd->caret() + 1] = 0;
    
    memset(preedit, 0, MAX_USER_INPUT + 1);
    
    WCSTOMBS(preedit, front_src, MAX_USER_INPUT);
    input->iCursorPos = strlen(preedit);
    WCSTOMBS(&preedit[strlen(preedit)], end_src, MAX_USER_INPUT);
    candidate_flag = true;

    SetMessageCount(GetMessageUp(instance), 0);
    AddMessageAtLast(GetMessageUp(instance), MSG_INPUT, preedit);
    
    const wstring& codeinput = this->owner->view->getPySegmentor()->getInputBuffer();
    WCSTOMBS(input->strCodeInput, codeinput.c_str(), MAX_USER_INPUT);
}

/**
 * @brief sunpinyin called this function while updating candidate words
 *
 * @param pcl candidate list
 * @return void
 **/
void FcitxWindowHandler::updateCandidates(const ICandidateList* pcl)
{
    FcitxInstance* instance = owner->owner;
    FcitxInputState* input = &owner->owner->input;
    wstring cand_str;
    char str[3] = { '\0', '\0', '\0' };
    input->iCandWordCount = pcl->size();
    SetMessageCount(GetMessageDown(instance), 0);
    if ( ConfigGetPointAfterNumber(&instance->config)) {
        str[1] = '.';
        str[2] = '\0';
    } else
        str[1] = '\0';
    for (int i = 0, sz = pcl->size(); i < sz; i++) {
        const TWCHAR* pcand = pcl->candiString(i);
        cand_str = pcand;
        TIConvSrcPtr src = (TIConvSrcPtr)(cand_str.c_str());
        WCSTOMBS(owner->CandTable[i], (const TWCHAR*) src, MAX_CAND_LEN);
        
        if (i == 9)
            str[0] = '0';
        else
            str[0] = i + 1 + '0';
        AddMessageAtLast(GetMessageDown(instance), MSG_INDEX, "%s", str);

        MSG_TYPE iType = MSG_OTHER;

        AddMessageAtLast(GetMessageDown(instance), iType, "%s", owner->CandTable[i]);
        
        if (i != (input->iCandWordCount - 1)) {
            MessageConcatLast(GetMessageDown(instance), " ");
        }
    }


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
    FcitxInstance* instance = owner->owner;
    char *buf_ = GetOutputString(&owner->owner->input);
    memset(buf_, 0, MAX_USER_INPUT);
    WCSTOMBS(buf_, str, MAX_USER_INPUT);
    commit_flag = true;
    instance->bShowCursor = false;
}
