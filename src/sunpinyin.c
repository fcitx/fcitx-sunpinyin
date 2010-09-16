#include <stdlib.h>
#include <fcitx/im.h>
#include <stdio.h>
#include <string.h>

static void Reset (void);
static INPUT_RETURN_VALUE DoInput (int);
static INPUT_RETURN_VALUE GetCandWords(SEARCH_MODE);
static char *GetCandWord (int);
static int Init (char *arg);
static int Destroy (void);

#ifdef _
#undef _
#endif

#define _(x) x

extern EXTRA_IM EIM;
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

void Reset (void)
{
}

INPUT_RETURN_VALUE DoInput (int iKey)
{
    INPUT_RETURN_VALUE ret = IRV_TO_PROCESS;
    fprintf(stderr, "%d\n", iKey);
    return IRV_DO_NOTHING;
}

INPUT_RETURN_VALUE GetCandWords(SEARCH_MODE searchMode)
{
    return IRV_DO_NOTHING;
}

char *GetCandWord (int iIndex)
{
    return "a";
}

int Init (char *arg)
{
    EIM.StringGet = malloc(40 * 6);
    memset(EIM.StringGet, 0, 40 * 6);
    return 0;
}

int Destroy (void)
{
    return 0;
}

