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
#include <fcitx/context.h>
#include <fcitx/module/punc/fcitx-punc.h>
#include <string>
#include <libintl.h>

#include "config.h"
#include "handler.h"
#include "eim.h"

#define FCITX_SUNPINYIN_MAX(x, y) ((x) > (y)? (x) : (y))

#ifdef __cplusplus
extern "C" {
#endif
    FCITX_DEFINE_PLUGIN(fcitx_sunpinyin, ime, FcitxIMClass) = {
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
static void* SunpinyinAddWord(void* arg, FcitxModuleFunctionArg args);
static INPUT_RETURN_VALUE FcitxSunpinyinDeleteCandidate (FcitxSunpinyin* sunpinyin, FcitxCandidateWord* candWord);
static void UpdatePunc(FcitxSunpinyin* sunpinyin);


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

struct CorrectionPair {
    const char* first;
    const char* second;
};

static const CorrectionPair uenunPairs[] = {
    {"uen", "un"},
};

static const CorrectionPair imgingPairs[] = {
    {"img", "ing"},
};

static const CorrectionPair iouiuPairs[] = {
    {"iou", "iu"},
};

static const CorrectionPair ueiuiPairs[] = {
    {"uei", "ui"},
};

static const CorrectionPair gnngPairs[] = {
    {"ign", "ing"},
    {"ogn", "ong"},
    {"agn", "ang"},
    {"egn", "eng"},
};

struct CorrectionPairs {
    const CorrectionPair* correctionPair;
    size_t size;
};

#define _DEF_PAIR(NAME) {NAME, sizeof(NAME) / sizeof(NAME[0]) }

static const CorrectionPairs correctionPairs[] = {
    _DEF_PAIR(uenunPairs),
    _DEF_PAIR(imgingPairs),
    _DEF_PAIR(iouiuPairs),
    _DEF_PAIR(ueiuiPairs),
    _DEF_PAIR(gnngPairs),
};

/**
 * @brief Reset the status.
 *
 **/
void FcitxSunpinyinReset (void* arg)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) arg;
    sunpinyin->view->clearIC();
    FcitxUIStatus* puncStatus = FcitxUIGetStatusByName(sunpinyin->owner, "punc");
    if (puncStatus)
        sunpinyin->view->setStatusAttrValue(CIMIWinHandler::STATUS_ID_FULLPUNC, puncStatus->getCurrentStatus(puncStatus->arg));
    else
        sunpinyin->view->setStatusAttrValue(CIMIWinHandler::STATUS_ID_FULLPUNC, false);
}

/**
 * @brief Process Key Input and return the status
 *
 * @param keycode keycode from XKeyEvent
 * @param state state from XKeyEvent
 * @param count count from XKeyEvent
 * @return INPUT_RETURN_VALUE
 **/
INPUT_RETURN_VALUE FcitxSunpinyinDoInput(void* arg, FcitxKeySym sym, unsigned int state)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) arg;
    FcitxInputState* input = FcitxInstanceGetInputState(sunpinyin->owner);
    CIMIView* view = sunpinyin->view;
    FcitxWindowHandler* windowHandler = sunpinyin->windowHandler;
    FcitxSunpinyinConfig* fs = &sunpinyin->fs;
    FcitxGlobalConfig* config = FcitxInstanceGetGlobalConfig(sunpinyin->owner);
    FcitxCandidateWordList* candList = FcitxInputStateGetCandidateList(input);
    FcitxCandidateWordSetChoose(candList, DIGIT_STR_CHOOSE);

    int chooseKey = FcitxHotkeyCheckChooseKey(sym, FcitxKeyState_None, DIGIT_STR_CHOOSE);
    if (state == FcitxKeyState_Ctrl_Alt && chooseKey >= 0)
    {
        FcitxCandidateWord* candidateWord = FcitxCandidateWordGetByIndex(candList, chooseKey);
        return FcitxSunpinyinDeleteCandidate(sunpinyin, candidateWord);
    }

    if ( (!FcitxHotkeyIsHotKeySimple(sym, state) || FcitxHotkeyIsHotKey(sym, state, FCITX_SPACE)) && view->getIC()->isEmpty())
        return IRV_TO_PROCESS;

    /* there is some special case that ';' is used */
    if (FcitxHotkeyIsHotKey(sym, state, FCITX_SEMICOLON) &&
        !(!view->getIC()->isEmpty() && fs->bUseShuangpin && (fs->SPScheme == MS2003 || fs->SPScheme == ZIGUANG)))
        return IRV_TO_PROCESS;

    if (FcitxHotkeyIsHotKey(sym, state, FCITX_SEPARATOR) &&
        view->getIC()->isEmpty())
        return IRV_TO_PROCESS;

    if (sym == FcitxKey_KP_Enter)
        sym = FcitxKey_Return;

    if (FcitxHotkeyIsHotKeyDigit(sym, state))
        return IRV_TO_PROCESS;

    if (FcitxHotkeyIsHotKey(sym, state, FCITX_SPACE))
        return FcitxCandidateWordChooseByIndex(candList, 0);

    if (FcitxHotkeyIsHotKey(sym, state, FCITX_ENTER)) {
        if (FcitxInputStateGetRawInputBufferSize(input)) {
            /* remove space from preedit */
            char* p = sunpinyin->preedit;
            char* pp = sunpinyin->preedit;

            while (*p) {
                uint32_t chr;

                char* next = fcitx_utf8_get_char(p, &chr);
                // space
                if (chr != 0x20) {
                    if (p != pp) {
                        memmove(pp, p, next - p);
                    }
                    pp += next - p;
                }
                p = next;
            }
            *pp = 0;
            FcitxInstanceCommitString(sunpinyin->owner, FcitxInstanceGetCurrentIC(sunpinyin->owner), sunpinyin->preedit);
            return IRV_CLEAN;
        } else {
            return IRV_TO_PROCESS;
        }
    }

    if ((view->getIC()->isEmpty() || !sunpinyin->fs.bProcessPunc)
        && !FcitxHotkeyIsHotKeyLAZ(sym, state)
        && !FcitxHotkeyIsHotKey(sym, state, FCITX_SEMICOLON)
        && !FcitxHotkeyIsHotKey(sym, state, FCITX_BACKSPACE)
        && !FcitxHotkeyIsHotKey(sym, state, FCITX_DELETE)
        && !FcitxHotkeyIsHotKey(sym, state, FCITX_ENTER)
        && !FcitxHotkeyIsHotKey(sym, state, FCITX_LEFT)
        && !FcitxHotkeyIsHotKey(sym, state, FCITX_RIGHT)
        && !FcitxHotkeyIsHotKey(sym, state, FCITX_HOME)
        && !FcitxHotkeyIsHotKey(sym, state, FCITX_END)
        && !FcitxHotkeyIsHotKey(sym, state, FCITX_SEPARATOR)
        )
        return IRV_TO_PROCESS;

    if (FcitxHotkeyIsHotKey(sym, state, config->hkPrevPage) || FcitxHotkeyIsHotKey(sym, state, config->hkNextPage))
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
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin* )arg;
    FcitxInstanceSetContext(sunpinyin->owner, CONTEXT_IM_KEYBOARD_LAYOUT, "us");
    UpdatePunc(sunpinyin);
    return true;
}


/**
 * @brief function DoInput has done everything for us.
 *
 * @param searchMode
 * @return INPUT_RETURN_VALUE
 **/
INPUT_RETURN_VALUE FcitxSunpinyinGetCandWords(void* arg)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin* )arg;
    FcitxInstance* instance = sunpinyin->owner;
    FcitxInputState* input = FcitxInstanceGetInputState(instance);
    FcitxGlobalConfig* config = FcitxInstanceGetGlobalConfig(sunpinyin->owner);
    FcitxCandidateWordList* candList = FcitxInputStateGetCandidateList(input);
    FcitxMessages* clientPreedit = FcitxInputStateGetClientPreedit(input);
    FcitxCandidateWordSetPageSize(candList, config->iMaxCandWord);

    CPreEditString ppd;
    sunpinyin->view->getPreeditString(ppd);
    TIConvSrcPtr src = (TIConvSrcPtr) (ppd.string());

    int hzlen = 0;
    while (hzlen < ppd.charTypeSize())
    {
        if ((ppd.charTypeAt(hzlen) & IPreeditString::HANZI_CHAR) != IPreeditString::HANZI_CHAR)
            break;
        hzlen ++ ;
    }

    FcitxInstanceCleanInputWindowUp(instance);

    memcpy(sunpinyin->front_src, src, ppd.caret() * sizeof(TWCHAR));
    memcpy(sunpinyin->end_src, src + ppd.caret() * sizeof(TWCHAR),
           (ppd.size() - ppd.caret() + 1) * sizeof(TWCHAR));
    memcpy(sunpinyin->input_src, src, hzlen * sizeof(TWCHAR));

    sunpinyin->front_src[ppd.caret()] = 0;
    sunpinyin->end_src[ppd.size() - ppd.caret() + 1] = 0;
    sunpinyin->input_src[hzlen] = 0;

    memset(sunpinyin->clientpreedit, 0, FCITX_SUNPINYIN_MAX(hzlen * UTF8_MAX_LENGTH + 1, MAX_USER_INPUT + 1));
    WCSTOMBS(sunpinyin->clientpreedit, sunpinyin->input_src, MAX_USER_INPUT);
    FcitxMessagesAddMessageAtLast(clientPreedit, MSG_INPUT, "%s", sunpinyin->clientpreedit);
    FcitxInputStateSetClientCursorPos(input, 0);

    memset(sunpinyin->preedit, 0, FCITX_SUNPINYIN_MAX(ppd.size() * UTF8_MAX_LENGTH + 1, MAX_USER_INPUT + 1));
    WCSTOMBS(sunpinyin->preedit, sunpinyin->front_src, MAX_USER_INPUT);
    FcitxInputStateSetCursorPos(input, strlen(sunpinyin->preedit));
    WCSTOMBS(&sunpinyin->preedit[strlen(sunpinyin->preedit)], sunpinyin->end_src, MAX_USER_INPUT);

    FcitxInputStateSetShowCursor(input, true);

    FcitxMessagesAddMessageAtLast(FcitxInputStateGetPreedit(input), MSG_INPUT, "%s", sunpinyin->preedit);

    CCandidateList pcl;
    sunpinyin->view->getCandidateList(pcl, 0, sunpinyin->candNum);
    for (int i = 0; i < pcl.size(); i ++ )
    {
        const TWCHAR* pcand = pcl.candiString(i);
        if (pcand == NULL)
            continue;

        int *index = (int*) fcitx_utils_malloc0(sizeof(int));
        *index = i;
        FcitxCandidateWord candWord;
        candWord.callback = FcitxSunpinyinGetCandWord;
        candWord.owner = sunpinyin;
        candWord.priv = index;
        candWord.strExtra = NULL;

        wstring cand_str = pcand;
        TIConvSrcPtr src = (TIConvSrcPtr)(cand_str.c_str());
        WCSTOMBS(sunpinyin->ubuf, (const TWCHAR*) src, MAX_CAND_LEN);

        candWord.strWord = strdup(sunpinyin->ubuf);
        candWord.wordType = MSG_OTHER;

        FcitxCandidateWordAppend(candList, &candWord);

        if (i == 0)
            FcitxMessagesAddMessageAtLast(FcitxInputStateGetClientPreedit(input), MSG_INPUT, "%s", candWord.strWord);
    }
    return IRV_DISPLAY_CANDWORDS;
}

/**
 * @brief get the candidate word by index
 *
 * @param iIndex index of candidate word
 * @return the string of canidate word
 **/
INPUT_RETURN_VALUE FcitxSunpinyinGetCandWord (void* arg, FcitxCandidateWord* candWord)
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
void* FcitxSunpinyinCreate (FcitxInstance* instance)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) fcitx_utils_malloc0(sizeof(FcitxSunpinyin));
    FcitxAddon* addon = FcitxAddonsGetAddonByName(FcitxInstanceGetAddons(instance), "fcitx-sunpinyin");
    bindtextdomain("fcitx-sunpinyin", LOCALEDIR);
    bind_textdomain_codeset("fcitx-sunpinyin", "UTF-8");
    sunpinyin->owner = instance;
    FcitxSunpinyinConfig* fs = &sunpinyin->fs;

    if (!LoadSunpinyinConfig(&sunpinyin->fs))
    {
        free(sunpinyin);
        return NULL;
    }

    /* portable detect here */
    if (getenv("FCITXDIR")) {
        char* path = fcitx_utils_get_fcitx_path_with_filename("libdir", "sunpinyin/data");
        std::string spath(path);
        ASimplifiedChinesePolicy::instance().setDataDir(spath);
        free(path);
    }

    CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();

    if (fs->bUseShuangpin)
        fac.setPinyinScheme(CSunpinyinSessionFactory::SHUANGPIN);
    else
        fac.setPinyinScheme(CSunpinyinSessionFactory::QUANPIN);

    ConfigSunpinyin(sunpinyin);
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
    sunpinyin->view->setStatusAttrValue(CIMIWinHandler::STATUS_ID_FULLSYMBOL, false);
    sunpinyin->view->setStatusAttrValue(CIMIWinHandler::STATUS_ID_FULLPUNC, false);
    sunpinyin->puncOp = new CGetFullPunctOp;
    sunpinyin->view->getIC()->setGetFullPunctOp(sunpinyin->puncOp);
    ConfigSunpinyin(sunpinyin);

    FcitxInstanceRegisterIM(instance,
                    sunpinyin,
                    "sunpinyin",
                    _("Sunpinyin"),
                    "sunpinyin",
                    FcitxSunpinyinInit,
                    FcitxSunpinyinReset,
                    FcitxSunpinyinDoInput,
                    FcitxSunpinyinGetCandWords,
                    NULL,
                    FcitxSunpinyinSave,
                    ReloadConfigFcitxSunpinyin,
                    NULL,
                    1,
                    "zh_CN");

    FcitxModuleAddFunction(addon, SunpinyinGetFullPinyin);
    FcitxModuleAddFunction(addon, SunpinyinAddWord);

    return sunpinyin;
}

/**
 * @brief Destroy the input method while unload it.
 *
 * @return int
 **/
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

INPUT_RETURN_VALUE FcitxSunpinyinDeleteCandidate (FcitxSunpinyin* sunpinyin, FcitxCandidateWord* candWord)
{
    if (candWord->owner == sunpinyin)
    {
        CCandidateList pcl;
        sunpinyin->view->getCandidateList(pcl, 0, sunpinyin->candNum);
        int* index = (int*) candWord->priv;
        CIMIClassicView* classicView = (CIMIClassicView*) sunpinyin->view;
        unsigned int mask;
        classicView->deleteCandidate(*index, mask);
        classicView->updateWindows(mask);
        return IRV_DISPLAY_CANDWORDS;
    }
    return IRV_TO_PROCESS;
}

void FcitxSunpinyinSave(void* arg)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) arg;
    sunpinyin->view->getIC()->memorize();
}


/**
 * @brief Load the config file for fcitx-sunpinyin
 *
 * @param Bool is reload or not
 **/
boolean LoadSunpinyinConfig(FcitxSunpinyinConfig* fs)
{
    FcitxConfigFileDesc *configDesc = GetSunpinyinConfigDesc();
    if (!configDesc)
        return false;

    FILE *fp = FcitxXDGGetFileUserWithPrefix("conf", "fcitx-sunpinyin.config", "r", NULL);

    if (!fp)
    {
        if (errno == ENOENT)
            SaveSunpinyinConfig(fs);
    }
    FcitxConfigFile *cfile = FcitxConfigParseConfigFileFp(fp, configDesc);

    FcitxSunpinyinConfigConfigBind(fs, cfile, configDesc);
    FcitxConfigBindSync(&fs->gconfig);

    if (fp)
        fclose(fp);
    return true;
}

void ConfigSunpinyin(FcitxSunpinyin* sunpinyin)
{
    FcitxInstance* instance = sunpinyin->owner;
    FcitxGlobalConfig* config = FcitxInstanceGetGlobalConfig(instance);
    FcitxSunpinyinConfig *fs = &sunpinyin->fs;
    int i = 0;

    if (sunpinyin->view)
    {
        sunpinyin->view->setCandiWindowSize(2048);
        // page up/down key
        CHotkeyProfile* prof = sunpinyin->view->getHotkeyProfile();
        prof->clear();

        for (i = 0 ; i < 2; i++)
        {
            if (config->hkPrevPage[i].sym)
                prof->addPageUpKey(CKeyEvent(config->hkPrevPage[i].sym, 0, config->hkPrevPage[i].state));
            if (config->hkNextPage[i].sym)
                prof->addPageDownKey(CKeyEvent(config->hkNextPage[i].sym, 0, config->hkNextPage[i].state));
        }
        sunpinyin->view->setCancelOnBackspace(1);
        sunpinyin->view->getIC()->setHistoryPower(fs->iMemoryStrength);
        sunpinyin->view->getIC()->setMaxBest(fs->maxBest + 1);
        sunpinyin->view->getIC()->setMaxTailCandidateNum(fs->maxTail);
    }

    string_pairs fuzzy, correction;
    for (i = 0; i < FUZZY_SIZE; i++)
        if (fs->bFuzzy[i])
            fuzzy.push_back(std::make_pair<std::string, std::string>(fuzzyPairs[i][0], fuzzyPairs[i][1]));

    for (i = 0; i < CORRECT_SIZE; i++)
        if (fs->bAutoCorrecting[i]) {
            for (int j = 0; j < correctionPairs[i].size; j ++ ) {
                correction.push_back(std::make_pair<std::string, std::string>(correctionPairs[i].correctionPair[j].first,
                                                                              correctionPairs[i].correctionPair[j].second));
            }
        }

    if (fuzzy.size() != 0)
    {
        AQuanpinSchemePolicy::instance().setFuzzyForwarding(true);
        AQuanpinSchemePolicy::instance().setFuzzyPinyinPairs(fuzzy);
        AShuangpinSchemePolicy::instance().setFuzzyForwarding(true);
        AShuangpinSchemePolicy::instance().setFuzzyPinyinPairs(fuzzy);
    }
    else
    {
        AQuanpinSchemePolicy::instance().setFuzzyForwarding(false);
        AQuanpinSchemePolicy::instance().clearFuzzyPinyinPairs();
        AShuangpinSchemePolicy::instance().setFuzzyForwarding(false);
        AShuangpinSchemePolicy::instance().clearFuzzyPinyinPairs();
    }

    if (correction.size() != 0)
    {
        AQuanpinSchemePolicy::instance().setAutoCorrecting(true);
        AQuanpinSchemePolicy::instance().setAutoCorrectionPairs(correction);
    }
    else
        AQuanpinSchemePolicy::instance().setAutoCorrecting(false);

    if (sunpinyin->shuangpin_data == NULL)
        sunpinyin->shuangpin_data = new CShuangpinData(fs->SPScheme);
    AShuangpinSchemePolicy::instance().setShuangpinType(fs->SPScheme);
    AQuanpinSchemePolicy::instance().setFuzzySegmentation(fs->bFuzzySegmentation);
    AQuanpinSchemePolicy::instance().setInnerFuzzySegmentation(fs->bFuzzyInnerSegmentation);
    UpdatePunc(sunpinyin);
}

void ReloadConfigFcitxSunpinyin(void* arg)
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
    FcitxConfigFileDesc *configDesc = GetSunpinyinConfigDesc();
    FILE *fp = FcitxXDGGetFileUserWithPrefix("conf", "fcitx-sunpinyin.config", "w", NULL);
    FcitxConfigSaveConfigFileFp(fp, &fs->gconfig, configDesc);
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

void UpdatePunc(FcitxSunpinyin* sunpinyin)
{
    if (!sunpinyin->puncOp)
        return;
    const char symbol[] = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
    string_pairs puncPairs;
    for (unsigned int i = 0;i < sizeof(symbol) / sizeof(char) - 1;i++) {
        int c = symbol[i];
        char s[2] = {symbol[i], '\0'};
        char *p1 = NULL, *p2 = NULL;
        FcitxPuncGetPunc2(sunpinyin->owner, &c, &p1, &p2);
        string_pair p;
        p.first = s;
        if (p1) {
            p.second = p1;
            puncPairs.push_back(p);
        }
        if (p2) {
            p.second = p2;
            puncPairs.push_back(p);
        }
    }
    sunpinyin->puncOp->initPunctMap(puncPairs);
}

void* SunpinyinAddWord(void* arg, FcitxModuleFunctionArg args)
{
    FcitxSunpinyin* sunpinyin = (FcitxSunpinyin*) arg;

    char* word = (char*) args.args[0];

    CUserDict* dict = sunpinyin->view->getIC()->getUserDict();

    if (!dict)
        return NULL;

    IPySegmentor::TSegmentVec& segments = sunpinyin->view->getPySegmentor()->getSegments(false);
    if (segments.size() == 0)
        return NULL;
    CSyllables syls;
    size_t len = fcitx_utf8_strlen(word);

    /* sunpinyin have assert inside add word, this is must be checked first */
    if (len > MAX_USRDEF_WORD_LEN || len < 2)
        return NULL;

    /* no way to check real single character pronouce, but let it be here */
    for (unsigned int i = 0;i < segments.size();i++) {
        const IPySegmentor::TSegment& segment = segments[i];
        for (unsigned int j = 0;j < segment.m_syllables.size();j++) {
            TSyllable syl = segment.m_syllables[j];
            if (!syl.isFullSyllable())
                return NULL;
            syls.push_back(syl);
        }
    }

    /* check the size */
    if (syls.size() != fcitx_utf8_strlen(word))
        return NULL;

    TWCHAR* wword = (TWCHAR*) fcitx_utils_malloc0(sizeof(TWCHAR) * (len + 1));
    MBSTOWCS(wword, word, len);
    dict->addWord(syls, wword);
    free(wword);

    return NULL;
}

// kate: indent-mode cstyle; space-indent on; indent-width 0;
