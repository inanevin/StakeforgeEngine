
#ifndef SFGEDITOR_EXPORT_H
#define SFGEDITOR_EXPORT_H

#ifdef SFGEDITOR_STATIC_DEFINE
#define SFGEDITOR_API
#define SFGEDITOR_NO_EXPORT
#else
#ifndef SFGEDITOR_API
#ifdef StakeforgeEditor_EXPORTS
/* We are building this library */
#define SFGEDITOR_API __declspec(dllexport)
#else
/* We are using this library */
#define SFGEDITOR_API __declspec(dllimport)
#endif
#endif

#ifndef SFGEDITOR_NO_EXPORT
#define SFGEDITOR_NO_EXPORT
#endif
#endif

#ifndef SFGEDITOR_DEPRECATED
#define SFGEDITOR_DEPRECATED __declspec(deprecated)
#endif

#ifndef SFGEDITOR_DEPRECATED_EXPORT
#define SFGEDITOR_DEPRECATED_EXPORT SFGEDITOR_API SFGEDITOR_DEPRECATED
#endif

#ifndef SFGEDITOR_DEPRECATED_NO_EXPORT
#define SFGEDITOR_DEPRECATED_NO_EXPORT SFGEDITOR_NO_EXPORT SFGEDITOR_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef SFGEDITOR_NO_DEPRECATED
#define SFGEDITOR_NO_DEPRECATED
#endif
#endif

#endif /* SFGEDITOR_EXPORT_H */
