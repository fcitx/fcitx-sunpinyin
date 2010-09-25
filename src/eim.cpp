#include <stdlib.h>
#include <fcitx/im.h>
#include <stdio.h>
#include <string.h>
#include <fcitx-config/hotkey.h>
#include <ime-core/imi_view.h>
#include <ime-core/imi_options.h>
#include <fcitx-config/configfile.h>
#include <fcitx-config/profile.h>

#include "handler.h"
#include "eim.h"

#ifdef _
#undef _
#endif

#define _(x) x

#ifdef __cplusplus
extern "C" {
#endif
    EXTRA_IM EIM = {
        _("sunpinyin"), /* Name */
        "fcitx-sunpinyin", /* IconName */
        Reset, /* Reset */
        DoInput, /* DoInput */
        GetCandWords, /* GetCandWords */
        GetCandWord,
        Init,
        Destroy,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        NULL,
        NULL
    };
#ifdef __cplusplus
}
#endif

static FcitxWindowHandler* instance = NULL;
static CIMIView* view = NULL;

__EXPORT_API
void Reset (void)
{
    FcitxProfile *profile = (FcitxProfile*) EIM.profile;
    view->setStatusAttrValue(CIMIWinHandler::STATUS_ID_FULLSYMBOL, profile->bCorner);
    view->setStatusAttrValue(CIMIWinHandler::STATUS_ID_FULLPUNC, profile->bChnPunc);
    view->clearIC();
}

__EXPORT_API
INPUT_RETURN_VALUE DoInput (unsigned int keycode, unsigned int state, int count)
{
    if ((keycode <= 0x20 || keycode > 0x7E) && view->getIC()->isEmpty())
        return IRV_TO_PROCESS;

    instance->commit_flag = false;
    instance->candidate_flag = false;
    unsigned int changeMasks = view->onKeyEvent(CKeyEvent(keycode, keycode, state));

    if (instance->commit_flag)
        return IRV_GET_CANDWORDS;
    if (!(changeMasks & CIMIView::KEYEVENT_USED))
        return IRV_TO_PROCESS;

    if (instance->candidate_flag)
    {
        return IRV_DISPLAY_CANDWORDS;
    }

    return IRV_TO_PROCESS;
}

__EXPORT_API
INPUT_RETURN_VALUE GetCandWords(SEARCH_MODE searchMode)
{
    return IRV_DO_NOTHING;
}

__EXPORT_API
char *GetCandWord (int iIndex)
{
    EIM.CandWordCount = 0;
    instance->commit_flag = false;
    instance->candidate_flag = false;
    if (iIndex <= 8)
    {
        unsigned int keycode = '1' + iIndex;
        unsigned int state = 0;
        unsigned int changeMasks = view->onKeyEvent(CKeyEvent(keycode, keycode, state));

        if (instance->commit_flag)
            return EIM.StringGet;
    }
    
    return NULL;
}

__EXPORT_API
int Init (char *arg)
{
    FcitxConfig *fc = (FcitxConfig*)EIM.fc;
    CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();
    fac.setPinyinScheme(CSunpinyinSessionFactory::QUANPIN);
    view = fac.createSession();

    instance = new FcitxWindowHandler();
    view->getIC()->setCharsetLevel(1);// GBK

    view->setCandiWindowSize(fc->iMaxCandWord);
    view->attachWinHandler(instance);
    // page up/down key
    CHotkeyProfile* prof = view->getHotkeyProfile();
    prof->clear();

    int i = 0;
    for (i = 0 ; i < 2; i++)
    {
        if (fc->hkPrevPage[i].iKeyCode)
            prof->addPageUpKey(CKeyEvent(fc->hkPrevPage[i].iKeyCode));
        if (fc->hkNextPage[i].iKeyCode)
            prof->addPageDownKey(CKeyEvent(fc->hkNextPage[i].iKeyCode));
    }
    view->setCancelOnBackspace(1);
    instance->set_eim(&EIM);

    return 0;
}

__EXPORT_API
int Destroy (void)
{
    CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();
    fac.destroySession(view);

    if (instance)
        delete instance;

    return 0;
}

