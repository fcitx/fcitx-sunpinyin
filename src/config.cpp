/***************************************************************************
 *   Copyright (C) 2010~2012 by CSSlayer                                   *
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

#include "eim.h"

/* USE fcitx provided macro to bind config and variable */
CONFIG_BINDING_BEGIN(FcitxSunpinyinConfig);
CONFIG_BINDING_REGISTER("Sunpinyin", "UseShuangpin", bUseShuangpin);
CONFIG_BINDING_REGISTER("Sunpinyin", "ShuangpinScheme", SPScheme);
CONFIG_BINDING_REGISTER("Sunpinyin", "FuzzySegmentation", bFuzzySegmentation);
CONFIG_BINDING_REGISTER("Sunpinyin", "FuzzyInnerSegmentation", bFuzzyInnerSegmentation);
CONFIG_BINDING_REGISTER("Sunpinyin", "MemoryStrength", iMemoryStrength);
CONFIG_BINDING_REGISTER("Sunpinyin", "MaxBest", maxBest);
CONFIG_BINDING_REGISTER("Sunpinyin", "MaxTail", maxTail);
CONFIG_BINDING_REGISTER("Sunpinyin", "ProcessPunc", bProcessPunc);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyZhiZi", bFuzzy[FUZZY_INDEX_ZhiZi]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyChiCi", bFuzzy[FUZZY_INDEX_ChiCi]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyShiSi", bFuzzy[FUZZY_INDEX_ShiSi]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyAnAng", bFuzzy[FUZZY_INDEX_AnAng]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyOnOng", bFuzzy[FUZZY_INDEX_OnOng]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyEnEng", bFuzzy[FUZZY_INDEX_EnEng]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyInIng", bFuzzy[FUZZY_INDEX_InIng]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyEngOng", bFuzzy[FUZZY_INDEX_EngOng]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyIanIang", bFuzzy[FUZZY_INDEX_IanIang]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyUanUang", bFuzzy[FUZZY_INDEX_UanUang]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyNeLe", bFuzzy[FUZZY_INDEX_NeLe]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyFoHe", bFuzzy[FUZZY_INDEX_FoHe]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyLeRi", bFuzzy[FUZZY_INDEX_LeRi]);
CONFIG_BINDING_REGISTER("QuanPin", "FuzzyKeGe", bFuzzy[FUZZY_INDEX_KeGe]);
CONFIG_BINDING_REGISTER("QuanPin", "AutoCorrectinggnng", bAutoCorrecting[CORRECT_INDEX_gnng]);
CONFIG_BINDING_REGISTER("QuanPin", "AutoCorrectingUenUn", bAutoCorrecting[CORRECT_INDEX_UenUn]);
CONFIG_BINDING_REGISTER("QuanPin", "AutoCorrectingImgIng", bAutoCorrecting[CORRECT_INDEX_ImgIng]);
CONFIG_BINDING_REGISTER("QuanPin", "AutoCorrectingIouIu", bAutoCorrecting[CORRECT_INDEX_IouIu]);
CONFIG_BINDING_REGISTER("QuanPin", "AutoCorrectingUeiUi", bAutoCorrecting[CORRECT_INDEX_UeiUi]);
CONFIG_BINDING_END();
// kate: indent-mode cstyle; space-indent on; indent-width 0;
