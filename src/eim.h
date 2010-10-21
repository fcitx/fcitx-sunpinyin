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
};

CONFIG_BINDING_DECLARE(FcitxSunpinyinConfig);

__EXPORT_API void Reset (void);
__EXPORT_API INPUT_RETURN_VALUE DoInput (unsigned int keycode, unsigned int state, int count);
__EXPORT_API INPUT_RETURN_VALUE GetCandWords(SEARCH_MODE);
__EXPORT_API char *GetCandWord (int);
__EXPORT_API int Init (char *arg);
__EXPORT_API int Destroy (void);

#endif
