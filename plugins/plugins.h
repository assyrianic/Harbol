#ifndef HARBOL_PLUGINS_INCLUDED
#	define HARBOL_PLUGINS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"

#include "tinydir.h"

#ifndef HARBOL_EXPORT
#	define HARBOL_EXPORT    static inline
#endif


#ifdef OS_WINDOWS
#	include <windows.h>
#	include <direct.h>
#	include <synchapi.h>
#	define stat _stat
#else
#	include <dlfcn.h>
#	include <unistd.h>
#endif


#ifdef OS_WINDOWS
#	ifndef MODULE_LOAD
#		define MODULE_LOAD(str)   LoadLibrary(str)
#	endif
#	ifndef MODULE_GET_OBJ
#		define MODULE_GET_OBJ     GetProcAddress
#	endif
#	ifndef MODULE_CLOSE
#		define MODULE_CLOSE       FreeLibrary
#	endif
#else
#	ifndef MODULE_LOAD
#		define MODULE_LOAD(str)   dlopen(str, RTLD_LAZY | RTLD_GLOBAL)
#	endif
#	ifndef MODULE_GET_OBJ
#		define MODULE_GET_OBJ     dlsym
#	endif
#	ifndef MODULE_CLOSE
#		define MODULE_CLOSE       dlclose
#	endif
#endif

#ifdef OS_WINDOWS
#	ifndef DIR_SEPARATOR
#		define DIR_SEPARATOR      "\\"
#	endif
#	ifndef LIB_EXT
#		define LIB_EXT            "dll"
#	endif
#elif defined OS_LINUX_UNIX
#	ifndef DIR_SEPARATOR
#		define DIR_SEPARATOR      "/"
#	endif
#	ifndef LIB_EXT
#		define LIB_EXT            "so"
#	endif
#elif defined OS_MAC
#	ifndef DIR_SEPARATOR
#		define DIR_SEPARATOR      "/"
#	endif
#	ifndef LIB_EXT
#		define LIB_EXT            "dylib"
#	endif
#endif


#ifdef OS_WINDOWS
typedef HMODULE HarbolDLL;
#else
typedef void   *HarbolDLL;
#endif


typedef void   *HarbolPlObj;


enum {
	HarbolPluginErrNone,
	HarbolPluginErrNoPath,      /// 'path' member wasn't initialized/allocated.
	HarbolPluginErrBadPath,     /// 'path' member didn't properly initialize or allocate.
	HarbolPluginErrLibLoadFail, /// DLL failed to load for some reason, check OS specific error.
	HarbolPluginErrNoLibLoaded, /// No DLL was ever loaded.
	HarbolPluginErrCantReload,  /// DLL failed to reload.
};


struct HarbolPlugin {
	HarbolDLL dll;
	char     *path;
	time_t    last_write;
	int       err;
};


typedef void HarbolPluginEvent(struct HarbolPlugin *pl_ctxt, void *userdata, bool reloading);

HARBOL_EXPORT NEVER_NULL(1,2) bool harbol_plugin_load(struct HarbolPlugin *pl, const char path[], HarbolPluginEvent load_fn, void *userdata);

HARBOL_EXPORT NEVER_NULL(1) void harbol_plugin_clear(struct HarbolPlugin *pl, HarbolPluginEvent unload_fn, void *userdata);

HARBOL_EXPORT NO_NULL bool harbol_plugin_changed(struct HarbolPlugin *pl);

HARBOL_EXPORT NEVER_NULL(1) bool harbol_plugin_reload(struct HarbolPlugin *pl, HarbolPluginEvent load_fn, HarbolPluginEvent unload_fn, void *userdata);

HARBOL_EXPORT NO_NULL NONNULL_RET const char *harbol_plugin_get_err(const struct HarbolPlugin *pl);

HARBOL_EXPORT NO_NULL void *harbol_plugin_get_obj(struct HarbolPlugin *pl, const char name[]);


typedef void HarbolPluginDirEvent(const char pl_path[], const char pl_name[], void *userdata);

HARBOL_EXPORT NEVER_NULL(1) bool harbol_plugin_dir_open(const char dir[], HarbolPluginDirEvent dir_fn, void *userdata, const char custom_ext[]);


#ifdef __cplusplus
} /// extern "C"
#endif

#endif /** HARBOL_PLUGINS_INCLUDED */
