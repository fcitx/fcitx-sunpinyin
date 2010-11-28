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

#ifndef EIM_H
#define EIM_H

#include <sunpinyin.h>
#include <fcitx/im.h>
#include <fcitx-config/fcitx-config.h>

#ifdef __cplusplus
#define __EXPORT_API extern "C"
#else
#define __EXPORT_API
#endif

struct FcitxSunpinyinConfig
{
    GenericConfig gconfig;
    Bool bUseShuangpin;
    EShuangpinType SPScheme;
    Bool bFuzzySegmentation;
    Bool bFuzzyInnerSegmentation;
        
    Bool bFuzzy[14];
    Bool bAutoCorrecting[6];
};

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

#define CORRECT_INDEX_IgnIng 0
#define CORRECT_INDEX_OgnOng 1
#define CORRECT_INDEX_UenUn 2
#define CORRECT_INDEX_ImgIng 3
#define CORRECT_INDEX_IouIu 4
#define CORRECT_INDEX_UeiUi 5
#define CORRECT_SIZE 6

CONFIG_BINDING_DECLARE(FcitxSunpinyinConfig);

__EXPORT_API void Reset (void);
__EXPORT_API INPUT_RETURN_VALUE DoInput (unsigned int keycode, unsigned int state, int count);
__EXPORT_API INPUT_RETURN_VALUE GetCandWords(SEARCH_MODE);
__EXPORT_API char *GetCandWord (int);
__EXPORT_API int Init (char *arg);
__EXPORT_API int Destroy (void);

#endif
