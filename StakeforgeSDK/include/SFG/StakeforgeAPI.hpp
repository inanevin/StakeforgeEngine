
#ifndef SFG_EXPORT_H
#define SFG_EXPORT_H

#ifdef SFG_STATIC_DEFINE
#define SFG_EXPORT
#define SFG_NO_EXPORT
#else
#ifndef SFG_EXPORT
#ifdef StakeforgeSDK_EXPORTS
/* We are building this library */
#define SFG_EXPORT __declspec(dllexport)
#else
/* We are using this library */
#define SFG_EXPORT __declspec(dllimport)
#endif
#endif

#ifndef SFG_NO_EXPORT
#define SFG_NO_EXPORT
#endif
#endif

#ifndef SFG_DEPRECATED
#define SFG_DEPRECATED __declspec(deprecated)
#endif

#ifndef SFG_DEPRECATED_EXPORT
#define SFG_DEPRECATED_EXPORT SFG_EXPORT SFG_DEPRECATED
#endif

#ifndef SFG_DEPRECATED_NO_EXPORT
#define SFG_DEPRECATED_NO_EXPORT SFG_NO_EXPORT SFG_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef SFG_NO_DEPRECATED
#define SFG_NO_DEPRECATED
#endif
#endif

#endif /* SFG_EXPORT_H */
