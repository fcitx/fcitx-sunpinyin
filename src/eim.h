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

#ifndef EIM_H
#define EIM_H

#include <sunpinyin.h>
#include <fcitx/ime.h>
#include <fcitx-config/fcitx-config.h>
#include <fcitx/instance.h>
#include <fcitx/candidate.h>

#define _(x) dgettext("fcitx-sunpinyin", (x))


#define FUZZY_INDEX_ShiSi 0
#define FUZZY_INDEX_ZhiZi 1
#define FUZZY_INDEX_ChiCi 2
#define FUZZY_INDEX_AnAng 3
#define FUZZY_INDEX_OnOng 4
#define FUZZY_INDEX_EnEng 5
#define FUZZY_INDEX_InIng 6
#define FUZZY_INDEX_EngOng 7
#define FUZZY_INDEX_IanIang 8
#define FUZZY_INDEX_UanUang 9
#define FUZZY_INDEX_NeLe 10
#define FUZZY_INDEX_FoHe 11
#define FUZZY_INDEX_LeRi 12
#define FUZZY_INDEX_KeGe 13
#define FUZZY_SIZE 14

#define CORRECT_INDEX_UenUn 0
#define CORRECT_INDEX_ImgIng 1
#define CORRECT_INDEX_IouIu 2
#define CORRECT_INDEX_UeiUi 3
#define CORRECT_INDEX_gnng 4
#define CORRECT_SIZE 5

class FcitxWindowHandler;
struct FcitxSunpinyinConfig
{
    FcitxGenericConfig gconfig;
    boolean bUseShuangpin;
    EShuangpinType SPScheme;
    boolean bFuzzySegmentation;
    boolean bFuzzyInnerSegmentation;
    boolean bProcessPunc;
    int iMemoryStrength;
    int maxBest;
    int maxTail;

    boolean bFuzzy[FUZZY_SIZE];
    boolean bAutoCorrecting[CORRECT_SIZE];
};

#define BUF_SIZE 4096

CONFIG_BINDING_DECLARE(FcitxSunpinyinConfig);
void* FcitxSunpinyinCreate(FcitxInstance* instance);
void FcitxSunpinyinDestroy(void* arg);
INPUT_RETURN_VALUE FcitxSunpinyinDoInput(void* arg, FcitxKeySym sym, unsigned int state);
INPUT_RETURN_VALUE FcitxSunpinyinGetCandWords (void *arg);
INPUT_RETURN_VALUE FcitxSunpinyinGetCandWord (void *arg, FcitxCandidateWord* candWord);
boolean FcitxSunpinyinInit(void*);
void ReloadConfigFcitxSunpinyin(void*);
void FcitxSunpinyinSave(void*);

typedef struct FcitxSunpinyin
{
    FcitxSunpinyinConfig fs;
    FcitxWindowHandler* windowHandler;
    CIMIView* view ;
    FcitxInstance* owner;
    char ubuf[BUF_SIZE];
    TWCHAR front_src[BUF_SIZE];
    TWCHAR end_src[BUF_SIZE];
    TWCHAR input_src[BUF_SIZE];
    char preedit[BUF_SIZE];
    char clientpreedit[BUF_SIZE];
    int candNum;
    CShuangpinData* shuangpin_data;
    boolean bShuangpin;
    CGetFullPunctOp* puncOp;
} FcitxSunpinyin;

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 0;
