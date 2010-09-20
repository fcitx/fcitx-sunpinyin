#include <stdlib.h>
#include <fcitx/im.h>
#include <stdio.h>
#include <string.h>
#include <sunpinyin.h>

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
        0
    };
#ifdef __cplusplus
}
#endif

static FcitxWindowHandler* instance = NULL;
static CIMIView* view = NULL;

__EXPORT_API
void Reset (void)
{
}

__EXPORT_API
INPUT_RETURN_VALUE DoInput (unsigned int keycode, unsigned int state, int count)
{
    INPUT_RETURN_VALUE ret = IRV_TO_PROCESS;
    if (keycode < 0x20 && keycode > 0x7E)
        keycode = 0;
    view->onKeyEvent(CKeyEvent(keycode, keycode, state));

    return IRV_DO_NOTHING;
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
    EIM.StringGet = new char[40 * 6];
    memset(EIM.StringGet, 0, 40 * 6);
    CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();
    fac.setPinyinScheme(CSunpinyinSessionFactory::QUANPIN);
    view = fac.createSession();

    instance = new FcitxWindowHandler();
    view->getIC()->setCharsetLevel(1);// GBK
    view->attachWinHandler(instance);
    // page up/down key
    CHotkeyProfile* prof = view->getHotkeyProfile();
    prof->clear();
    prof->addPageUpKey(CKeyEvent(IM_VK_MINUS));
    prof->addPageDownKey(CKeyEvent(IM_VK_EQUALS));
    view->setCancelOnBackspace(0);
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

