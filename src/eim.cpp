#include <stdlib.h>
#include <fcitx/im.h>
#include <stdio.h>
#include <string.h>
#include <sunpinyin.h>
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
    printf("keycode:%u %d\n", keycode, view->getIC()->isEmpty());

    if ((keycode <= 0x20 || keycode > 0x7E) && view->getIC()->isEmpty())
        return IRV_TO_PROCESS;

    instance->commit_flag = false;
    instance->candidate_flag = false;
    unsigned int changeMasks = view->onKeyEvent(CKeyEvent(keycode, keycode, state));
    
    printf("%u\n", changeMasks);

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
    return NULL;
}

__EXPORT_API
int Init (char *arg)
{
    FcitxConfig *fc = (FcitxConfig*)EIM.fc;
    EIM.StringGet = new char[40 * 6];
    memset(EIM.StringGet, 0, 40 * 6);
    CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();
    fac.setPinyinScheme(CSunpinyinSessionFactory::QUANPIN);
    view = fac.createSession();

    instance = new FcitxWindowHandler();
    view->getIC()->setCharsetLevel(1);// GBK
    printf("%d\n", fc->iMaxCandWord);
    view->setCandiWindowSize(fc->iMaxCandWord);
    view->attachWinHandler(instance);
    // page up/down key
    CHotkeyProfile* prof = view->getHotkeyProfile();
    prof->clear();
    prof->addPageUpKey(CKeyEvent(IM_VK_MINUS));
    prof->addPageDownKey(CKeyEvent(IM_VK_EQUALS));
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

