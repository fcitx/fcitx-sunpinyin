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
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#include <sunpinyin.h>
#include <fcitx-utils/utils.h>
#include <fcitx-utils/log.h>
#include "handler.h"
#include "eim.h"

/**
 * @brief handler called while preedit updated
 *
 * @param ppd preedit string
 * @return void
 **/
void FcitxWindowHandler::updatePreedit(const IPreeditString* ppd)
{
    FcitxInstance* instance = owner->owner;
    FcitxInputState* input = FcitxInstanceGetInputState(instance);
    FcitxInputStateSetCursorPos(input, true);
    candidate_flag = true;

    const wstring& codeinput = this->owner->view->getPySegmentor()->getInputBuffer();
    WCSTOMBS(FcitxInputStateGetRawInputBuffer(input), codeinput.c_str(), MAX_USER_INPUT);
    FcitxInputStateSetRawInputBufferSize(input, strlen(FcitxInputStateGetRawInputBuffer(input)));
}

/**
 * @brief sunpinyin called this function while updating candidate words
 *
 * @param pcl candidate list
 * @return void
 **/
void FcitxWindowHandler::updateCandidates(const ICandidateList* pcl)
{
    owner->candNum = pcl->total();

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
    FcitxInputState* input = FcitxInstanceGetInputState(instance);
    char *buf_ = FcitxInputStateGetOutputString(input);
    memset(buf_, 0, MAX_USER_INPUT);
    WCSTOMBS(buf_, str, MAX_USER_INPUT);
    commit_flag = true;
    FcitxInputStateSetCursorPos(input, false);
}

void FcitxWindowHandler::updateStatus(int key, int value)
{
    return;
}
// kate: indent-mode cstyle; space-indent on; indent-width 0;
