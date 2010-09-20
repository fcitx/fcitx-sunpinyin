#ifdef __cplusplus
#define __EXPORT_API extern "C"
#else
#define __EXPORT_API
#endif

__EXPORT_API void Reset (void);
__EXPORT_API INPUT_RETURN_VALUE DoInput (unsigned int keycode, unsigned int state, int count);
__EXPORT_API INPUT_RETURN_VALUE GetCandWords(SEARCH_MODE);
__EXPORT_API char *GetCandWord (int);
__EXPORT_API int Init (char *arg);
__EXPORT_API int Destroy (void);

