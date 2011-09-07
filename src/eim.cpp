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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ime-core/imi_view.h>
#include <ime-core/imi_options.h>
#include <ime-core/utils.h>
#include <fcitx/ime.h>
#include <fcitx-config/hotkey.h>
#include <fcitx-config/xdg.h>
#include <fcitx-utils/log.h>
#include <fcitx-config/fcitx-config.h>
#include <fcitx-utils/utils.h>
#include <fcitx/instance.h>
#include <fcitx/keys.h>
#include <fcitx/module.h>
#include <string>
#include <libintl.h>

#include "handler.h"
#include "eim.h"

#ifdef __cplusplus
extern "C" {
#endif
    FCITX_EXPORT_API
    FcitxIMClass ime = {
        FcitxSunpinyinCreate,
        FcitxSunpinyinDestroy
    };
#ifdef __cplusplus
}
#endif

CONFIG_DESC_DEFINE(GetSunpinyinConfigDesc, "fcitx-sunpinyin.desc")

boolean LoadSunpinyinConfig(FcitxSunpinyinConfig* fs);
static void SaveSunpinyinConfig(FcitxSunpinyinConfig* fs);
static void ConfigSunpinyin(FcitxSunpinyin* sunpinyin);
static void* SunpinyinGetFullPinyin(void* arg, FcitxModuleFunctionArg args);


static const char* fuzzyPairs[][2] = {
    {"sh", "s"},
    {"zh", "z"},
    {"ch", "c"},
    {"an", "ang"},
    {"on", "ong"},
    {"en", "eng"},
    {"in", "ing"},
    {"eng", "ong"},
    {"ian", "iang"},
    {"uan", "uang"},
    {"n", "l"},
    {"f", "h"},
    {"l", "r"},
    {"k", "g"}
};

static const char *correctionPairs[][2] = {
    {"ign", "ing"},
    {"ogn", "ong"},
    {"uen", "un"},
    {"img", "ing"},
    {"iou", "iu"},
    {"uei", "ui"}
};

/**
 * @brief Reset the status.
 *
 **/
__EXPORT_API
void FcitxSunpinyinReset (void* arg)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) arg;
    FcitxUIStatus* puncStatus = GetUIStatus(sunpinyin->owner, "punc");
    FcitxUIStatus* fullwidthStatus = GetUIStatus(sunpinyin->owner, "fullwidth");
    sunpinyin->view->setStatusAttrValue(CIMIWinHandler::STATUS_ID_FULLSYMBOL, fullwidthStatus->getCurrentStatus(fullwidthStatus->arg));
    sunpinyin->view->setStatusAttrValue(CIMIWinHandler::STATUS_ID_FULLPUNC, puncStatus->getCurrentStatus(puncStatus->arg));
    sunpinyin->view->clearIC();
}

/**
 * @brief Process Key Input and return the status
 *
 * @param keycode keycode from XKeyEvent
 * @param state state from XKeyEvent
 * @param count count from XKeyEvent
 * @return INPUT_RETURN_VALUE
 **/
__EXPORT_API
INPUT_RETURN_VALUE FcitxSunpinyinDoInput(void* arg, FcitxKeySym sym, unsigned int state)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) arg;
    FcitxInputState* input = &sunpinyin->owner->input;
    CIMIView* view = sunpinyin->view;
    FcitxWindowHandler* windowHandler = sunpinyin->windowHandler;
    FcitxSunpinyinConfig* fs = &sunpinyin->fs;
    CandidateWordSetChoose(input->candList, DIGIT_STR_CHOOSE);
    if ( (!IsHotKeySimple(sym, state) || IsHotKey(sym, state, FCITX_SPACE)) && view->getIC()->isEmpty())
        return IRV_TO_PROCESS;

    /* there is some special case that ';' is used */
    if (IsHotKey(sym, state, FCITX_SEMICOLON) &&
        !(!view->getIC()->isEmpty() && fs->bUseShuangpin && (fs->SPScheme == MS2003 || fs->SPScheme == ZIGUANG)))
        return IRV_TO_PROCESS;

    if (IsHotKey(sym, state, FCITX_SEPARATOR) &&
        view->getIC()->isEmpty())
        return IRV_TO_PROCESS;

    if (sym == Key_KP_Enter)
        sym = Key_Return;

    if (IsHotKeyDigit(sym, state))
        return IRV_TO_PROCESS;

    if (IsHotKey(sym, state, FCITX_SPACE))
        return CandidateWordChooseByIndex(input->candList, 0);

    if (!IsHotKeyUAZ(sym, state)
        && !IsHotKeyLAZ(sym, state)
        && !IsHotKey(sym, state, FCITX_SEMICOLON)
        && !IsHotKey(sym, state, FCITX_BACKSPACE)
        && !IsHotKey(sym, state, FCITX_DELETE)
        && !IsHotKey(sym, state, FCITX_ENTER)
        && !IsHotKey(sym, state, FCITX_LEFT)
        && !IsHotKey(sym, state, FCITX_RIGHT)
        && !IsHotKey(sym, state, FCITX_HOME)
        && !IsHotKey(sym, state, FCITX_END)
        && !IsHotKey(sym, state, FCITX_SEPARATOR)
        )
        return IRV_TO_PROCESS;

    if (IsHotKey(sym, state, sunpinyin->owner->config->hkPrevPage) || IsHotKey(sym, state, sunpinyin->owner->config->hkNextPage))
        return IRV_TO_PROCESS;

    windowHandler->commit_flag = false;
    windowHandler->candidate_flag = false;
    unsigned int changeMasks = view->onKeyEvent(CKeyEvent(sym, sym, state));

    if (windowHandler->commit_flag)
        return IRV_COMMIT_STRING;
    if (!(changeMasks & CIMIView::KEYEVENT_USED))
        return IRV_TO_PROCESS;

    if (view->getIC()->isEmpty())
        return IRV_CLEAN;

    if (windowHandler->candidate_flag)
    {
        return IRV_DISPLAY_CANDWORDS;
    }

    return IRV_DO_NOTHING;
}

boolean FcitxSunpinyinInit(void* arg)
{
    return true;
}


/**
 * @brief function DoInput has done everything for us.
 *
 * @param searchMode
 * @return INPUT_RETURN_VALUE
 **/
__EXPORT_API
INPUT_RETURN_VALUE FcitxSunpinyinGetCandWords(void* arg)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin* )arg;
    FcitxInstance* instance = sunpinyin->owner;
    FcitxInputState* input = &sunpinyin->owner->input;

    CPreEditString ppd;
    sunpinyin->view->getPreeditString(ppd);
    TIConvSrcPtr src = (TIConvSrcPtr) (ppd.string());

    memcpy(sunpinyin->front_src, src, ppd.caret() * sizeof(TWCHAR));
    memcpy(sunpinyin->end_src, src + ppd.caret() * sizeof(TWCHAR),
           (ppd.size() - ppd.caret() + 1) * sizeof(TWCHAR));

    sunpinyin->front_src[ppd.caret()] = 0;
    sunpinyin->end_src[ppd.size() - ppd.caret() + 1] = 0;

    memset(sunpinyin->preedit, 0, MAX_USER_INPUT + 1);

    WCSTOMBS(sunpinyin->preedit, sunpinyin->front_src, MAX_USER_INPUT);
    input->iCursorPos = strlen(sunpinyin->preedit);
    WCSTOMBS(&sunpinyin->preedit[strlen(sunpinyin->preedit)], sunpinyin->end_src, MAX_USER_INPUT);

    CleanInputWindowUp(instance);
    AddMessageAtLast(input->msgPreedit, MSG_INPUT, sunpinyin->preedit);

    CCandidateList pcl;
    sunpinyin->view->getCandidateList(pcl, 0, sunpinyin->candNum);
    for (int i = 0; i < pcl.size(); i ++ )
    {
        const TWCHAR* pcand = pcl.candiString(i);
        if (pcand == NULL)
            continue;

        int *index = (int*) fcitx_malloc0(sizeof(int));
        *index = i;
        CandidateWord candWord;
        candWord.callback = FcitxSunpinyinGetCandWord;
        candWord.owner = sunpinyin;
        candWord.priv = index;
        candWord.strExtra = NULL;

        wstring cand_str = pcand;
        TIConvSrcPtr src = (TIConvSrcPtr)(cand_str.c_str());
        WCSTOMBS(sunpinyin->ubuf, (const TWCHAR*) src, MAX_CAND_LEN);

        candWord.strWord = strdup(sunpinyin->ubuf);

        CandidateWordAppend(sunpinyin->owner->input.candList, &candWord);
    }
    return IRV_DISPLAY_CANDWORDS;
}

/**
 * @brief get the candidate word by index
 *
 * @param iIndex index of candidate word
 * @return the string of canidate word
 **/
__EXPORT_API
INPUT_RETURN_VALUE FcitxSunpinyinGetCandWord (void* arg, CandidateWord* candWord)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin* )arg;
    sunpinyin->windowHandler->commit_flag = false;
    sunpinyin->windowHandler->candidate_flag = false;
    int* index = (int*) candWord->priv;
    sunpinyin->view->onCandidateSelectRequest(*index);

    if (sunpinyin->windowHandler->commit_flag)
        return IRV_COMMIT_STRING;

    if (sunpinyin->windowHandler->candidate_flag)
        return IRV_DISPLAY_CANDWORDS;

    return IRV_DO_NOTHING;
}

/**
 * @brief initialize the extra input method
 *
 * @param arg
 * @return successful or not
 **/
__EXPORT_API
void* FcitxSunpinyinCreate (FcitxInstance* instance)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) fcitx_malloc0(sizeof(FcitxSunpinyin));
    FcitxAddon* addon = GetAddonByName(&instance->addons, "fcitx-sunpinyin");
    bindtextdomain("fcitx-sunpinyin", LOCALEDIR);
    sunpinyin->owner = instance;
    FcitxSunpinyinConfig* fs = &sunpinyin->fs;

    if (!LoadSunpinyinConfig(&sunpinyin->fs))
    {
        free(sunpinyin);
        return NULL;
    }
    CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();

    if (fs->bUseShuangpin)
        fac.setPinyinScheme(CSunpinyinSessionFactory::SHUANGPIN);
    else
        fac.setPinyinScheme(CSunpinyinSessionFactory::QUANPIN);
    sunpinyin->bShuangpin = fs->bUseShuangpin;

    sunpinyin->view = fac.createSession();

    if (sunpinyin->view == NULL)
    {
        free(sunpinyin);
        return NULL;
    }

    FcitxWindowHandler* windowHandler = new FcitxWindowHandler();
    sunpinyin->windowHandler = windowHandler;
    sunpinyin->view->getIC()->setCharsetLevel(3);

    sunpinyin->view->attachWinHandler(windowHandler);
    sunpinyin->windowHandler->SetOwner(sunpinyin);

    ConfigSunpinyin(sunpinyin);

    FcitxRegisterIM(instance,
                    sunpinyin,
                    _("Sunpinyin"),
                    "sunpinyin",
                    FcitxSunpinyinInit,
                    FcitxSunpinyinReset,
                    FcitxSunpinyinDoInput,
                    FcitxSunpinyinGetCandWords,
                    NULL,
                    NULL,
                    ReloadConfigFcitxSunpinyin,
                    NULL,
                    fs->iSunpinyinPriority
                   );

    AddFunction(addon, (void*) SunpinyinGetFullPinyin);

    return sunpinyin;
}

/**
 * @brief Destroy the input method while unload it.
 *
 * @return int
 **/
__EXPORT_API
void FcitxSunpinyinDestroy (void* arg)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) arg;
    CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();
    fac.destroySession(sunpinyin->view);
    if (sunpinyin->shuangpin_data)
        delete sunpinyin->shuangpin_data;

    if (sunpinyin->windowHandler)
        delete sunpinyin->windowHandler;

    free(arg);
}

/**
 * @brief Load the config file for fcitx-sunpinyin
 *
 * @param Bool is reload or not
 **/
boolean LoadSunpinyinConfig(FcitxSunpinyinConfig* fs)
{
    ConfigFileDesc *configDesc = GetSunpinyinConfigDesc();
    if (!configDesc)
        return false;

    FILE *fp = GetXDGFileUserWithPrefix("conf", "fcitx-sunpinyin.config", "rt", NULL);

    if (!fp)
    {
        if (errno == ENOENT)
            SaveSunpinyinConfig(fs);
    }
    ConfigFile *cfile = ParseConfigFileFp(fp, configDesc);

    FcitxSunpinyinConfigConfigBind(fs, cfile, configDesc);
    ConfigBindSync(&fs->gconfig);

    if (fp)
        fclose(fp);
    return true;
}

void ConfigSunpinyin(FcitxSunpinyin* sunpinyin)
{
    ConfigValueType prevpage;
    ConfigValueType nextpage;
    FcitxInstance* instance = sunpinyin->owner;
    GenericConfig *fc = &instance->config->gconfig;
    FcitxSunpinyinConfig *fs = &sunpinyin->fs;
    prevpage = ConfigGetBindValue(fc, "Hotkey", "PrevPageKey");
    nextpage = ConfigGetBindValue(fc, "Hotkey", "NextPageKey");
    sunpinyin->view->setCandiWindowSize(2048);
    // page up/down key
    CHotkeyProfile* prof = sunpinyin->view->getHotkeyProfile();
    prof->clear();

    int i = 0;
    for (i = 0 ; i < 2; i++)
    {
        if (prevpage.hotkey[i].sym)
            prof->addPageUpKey(CKeyEvent(prevpage.hotkey[i].sym, 0, prevpage.hotkey[i].state));
        if (nextpage.hotkey[i].sym)
            prof->addPageDownKey(CKeyEvent(nextpage.hotkey[i].sym, 0, nextpage.hotkey[i].state));
    }

    string_pairs fuzzy, correction;
    for (i = 0; i < FUZZY_SIZE; i++)
        if (fs->bFuzzy[i])
            fuzzy.push_back(std::make_pair<std::string, std::string>(fuzzyPairs[i][0], fuzzyPairs[i][1]));

    for (i = 0; i < CORRECT_SIZE; i++)
        if (fs->bAutoCorrecting[i])
            correction.push_back(std::make_pair<std::string, std::string>(correctionPairs[i][0], correctionPairs[i][1]));

    if (fuzzy.size() != 0)
    {
        AQuanpinSchemePolicy::instance().setFuzzyForwarding(true);
        AQuanpinSchemePolicy::instance().setFuzzyPinyinPairs(fuzzy);
    }
    else
    {
        AQuanpinSchemePolicy::instance().setFuzzyForwarding(false);
        AQuanpinSchemePolicy::instance().clearFuzzyPinyinPairs();
    }

    if (correction.size() != 0)
    {
        AQuanpinSchemePolicy::instance().setAutoCorrecting(true);
        AQuanpinSchemePolicy::instance().setAutoCorrectionPairs(correction);
    }
    else
        AQuanpinSchemePolicy::instance().setAutoCorrecting(false);

    sunpinyin->view->setCancelOnBackspace(1);
    if (sunpinyin->shuangpin_data)
        delete sunpinyin->shuangpin_data;
    sunpinyin->shuangpin_data = new CShuangpinData(fs->SPScheme);
    AShuangpinSchemePolicy::instance().setShuangpinType(fs->SPScheme);
    AQuanpinSchemePolicy::instance().setFuzzySegmentation(fs->bFuzzySegmentation);
    AQuanpinSchemePolicy::instance().setInnerFuzzySegmentation(fs->bFuzzyInnerSegmentation);
}

__EXPORT_API void ReloadConfigFcitxSunpinyin(void* arg)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) arg;
    LoadSunpinyinConfig(&sunpinyin->fs);
    ConfigSunpinyin(sunpinyin);
}

/**
 * @brief Save the config
 *
 * @return void
 **/
void SaveSunpinyinConfig(FcitxSunpinyinConfig* fs)
{
    ConfigFileDesc *configDesc = GetSunpinyinConfigDesc();
    FILE *fp = GetXDGFileUserWithPrefix("conf", "fcitx-sunpinyin.config", "wt", NULL);
    SaveConfigFileFp(fp, &fs->gconfig, configDesc);
    if (fp)
        fclose(fp);
}

void* SunpinyinGetFullPinyin(void* arg, FcitxModuleFunctionArg args)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) arg;
    char* pinyin = (char*) args.args[0];
    boolean *issp = (boolean*) args.args[1];
    *issp = sunpinyin->bShuangpin;
    CMappedYin syls;
    if (sunpinyin->bShuangpin)
    {
        sunpinyin->shuangpin_data->getMapString(pinyin, syls);
        if (syls.size() == 0)
            return NULL;
        else
            return strdup(syls[0].c_str());
    }
    else
        return NULL;
}

// kate: indent-mode cstyle; space-indent on; indent-width 0;
