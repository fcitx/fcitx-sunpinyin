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

extern "C" {
#include <fcitx/ime.h>
#include <fcitx-config/hotkey.h>
#include <fcitx-config/xdg.h>
#include <fcitx-utils/cutils.h>
#include <fcitx-config/fcitx-config.h>
#include <fcitx/instance.h>
}
#include <string>
#include <libintl.h>

#include "handler.h"
#include "eim.h"

#ifdef __cplusplus
extern "C" {
#endif
FcitxIMClass ime = {
    FcitxSunpinyinCreate,
    FcitxSunpinyinDestroy
};
#ifdef __cplusplus
}
#endif

CONFIG_DESC_DEFINE(GetSunpinyinConfigDesc, "addon/fcitx-sunpinyin.desc")

static void LoadSunpinyinConfig(FcitxSunpinyinConfig* fs, boolean reload = (0));
static void SaveSunpinyinConfig(FcitxSunpinyinConfig* fs);
static void ConfigSunpinyin(FcitxSunpinyin* sunpinyin);


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
    CIMIView* view = sunpinyin->view;
    FcitxWindowHandler* windowHandler = sunpinyin->windowHandler;
    if ((sym <= 0x20 || sym > 0x7E) && view->getIC()->isEmpty())
        return IRV_TO_PROCESS;
    
    if (sym == 0x003b && view->getIC()->isEmpty())
        return IRV_TO_PROCESS;         

    if (sym == 0xFF8D)
        sym = 0xFF0D;

    windowHandler->commit_flag = false;
    windowHandler->candidate_flag = false;
    unsigned int changeMasks = view->onKeyEvent(CKeyEvent(sym, sym, state));

    if (windowHandler->commit_flag)
        return IRV_GET_CANDWORDS;
    if (!(changeMasks & CIMIView::KEYEVENT_USED))
        return IRV_TO_PROCESS;
    
    if (view->getIC()->isEmpty())
        return IRV_CLEAN;

    if (windowHandler->candidate_flag)
    {
        return IRV_DISPLAY_CANDWORDS;
    }

    return IRV_TO_PROCESS;
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
INPUT_RETURN_VALUE FcitxSunpinyinGetCandWords(void* arg, SEARCH_MODE searchMode)
{
    return IRV_DO_NOTHING;
}

/**
 * @brief get the candidate word by index
 *
 * @param iIndex index of candidate word
 * @return the string of canidate word
 **/
__EXPORT_API
char *FcitxSunpinyinGetCandWord (void* arg, int iIndex)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin* )arg;
    sunpinyin->owner->input.iCandWordCount = 0;
    sunpinyin->windowHandler->commit_flag = false;
    sunpinyin->windowHandler->candidate_flag = false;
    if (iIndex <= 8)
    {
        unsigned int keycode = '1' + iIndex;
        unsigned int state = 0;
        unsigned int changeMasks = sunpinyin->view->onKeyEvent(CKeyEvent(keycode, keycode, state));

        if (sunpinyin->windowHandler->commit_flag)
            return sunpinyin->owner->input.strStringGet;
    }
    
    return NULL;
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
    bindtextdomain("fcitx-sunpinyin", LOCALEDIR);
    sunpinyin->owner = instance;
    GenericConfig *fc = &instance->config.gconfig;
    FcitxSunpinyinConfig* fs = &sunpinyin->fs;

    LoadSunpinyinConfig(&sunpinyin->fs);
    CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();

    if (fs->bUseShuangpin)
        fac.setPinyinScheme(CSunpinyinSessionFactory::SHUANGPIN);
    else
        fac.setPinyinScheme(CSunpinyinSessionFactory::QUANPIN);

    sunpinyin->view = fac.createSession();

    FcitxWindowHandler* windowHandler = new FcitxWindowHandler();
    sunpinyin->windowHandler = windowHandler;
    sunpinyin->view->getIC()->setCharsetLevel(1);// GBK

    sunpinyin->view->attachWinHandler(windowHandler);
    sunpinyin->windowHandler->SetOwner(sunpinyin);
    
    ConfigSunpinyin(sunpinyin);
    
    FcitxRegisterIM(instance,
                    sunpinyin,
                    _("Sunpinyin"),
                    "fcitx-sunpinyin",
                    FcitxSunpinyinInit,
                    FcitxSunpinyinReset,
                    FcitxSunpinyinDoInput,
                    FcitxSunpinyinGetCandWords,
                    FcitxSunpinyinGetCandWord,
                    NULL,
                    NULL,
                    ReloadConfigFcitxSunpinyin,
                    NULL,
                    fs->iSunpinyinPriority
                   );
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

    if (sunpinyin->windowHandler)
        delete sunpinyin->windowHandler;
    
    free(arg);
}

/**
 * @brief Load the config file for fcitx-sunpinyin
 *
 * @param Bool is reload or not
 **/
void LoadSunpinyinConfig(FcitxSunpinyinConfig* fs, boolean reload)
{
    ConfigFileDesc *configDesc = GetSunpinyinConfigDesc();

    FILE *fp = GetXDGFileUser( "addon/fcitx-sunpinyin.config", "rt", NULL);

    if (!fp)
    {
        if (!reload && errno == ENOENT)
        {
            char *lastdomain = strdup(textdomain(NULL));
            textdomain("fcitx-sunpinyin");
            SaveSunpinyinConfig(fs);
            textdomain(lastdomain);
            free(lastdomain);
            LoadSunpinyinConfig(fs, true);
        }
        return;
    }
    ConfigFile *cfile = ParseConfigFileFp(fp, configDesc);

    if (cfile)
    {
        FcitxSunpinyinConfigConfigBind(fs, cfile, configDesc);
        ConfigBindSync(&fs->gconfig);
    }
    else
    {
        fs->bUseShuangpin = False;
        fs->SPScheme = MS2003;
        fs->bFuzzySegmentation = False;
        fs->bFuzzyInnerSegmentation = False;
        int i = 0;
        for (i = 0; i < FUZZY_SIZE; i ++)
            fs->bFuzzy[i] = False;
        
        for (i = 0; i < CORRECT_SIZE; i ++)
            fs->bAutoCorrecting[i] = False;
    }
}

void ConfigSunpinyin(FcitxSunpinyin* sunpinyin)
{
    ConfigValueType candword;
    ConfigValueType prevpage;
    ConfigValueType nextpage;
    FcitxInstance* instance = sunpinyin->owner;
    GenericConfig *fc = &instance->config.gconfig;
    FcitxSunpinyinConfig *fs = &sunpinyin->fs;
    candword = ConfigGetBindValue(fc, "Output", "CandidateWordNumber");
    prevpage = ConfigGetBindValue(fc, "Hotkey", "PrevPageKey");
    nextpage = ConfigGetBindValue(fc, "Hotkey", "NextPageKey");    
    sunpinyin->view->setCandiWindowSize(*candword.integer);
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
    FILE *fp = GetXDGFileUser( "addon/fcitx-sunpinyin.config", "wt", NULL);
    SaveConfigFileFp(fp, &fs->gconfig, configDesc);
    fclose(fp);
}
