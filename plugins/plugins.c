#include "plugins.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT bool harbol_plugin_load(struct HarbolPlugin *const restrict pl, const char path[static 1], HarbolPluginEvent load_fn, void *const restrict userdata) {
	pl->dll = MODULE_LOAD(path);
	if( pl->dll==NULL ) {
		pl->err = HarbolPluginErrLibLoadFail;
		return false;
	}
	
	pl->path = dup_str(path);
	struct stat result;
	if( !stat(path, &result) ) {
		pl->last_write = result.st_mtime;
	} else {
		MODULE_CLOSE(pl->dll); pl->dll = NULL;
		free(pl->path); pl->path = NULL;
		pl->err = HarbolPluginErrBadPath;
		return false;
	}
	
	if( load_fn != NULL ) {
		(*load_fn)(pl, userdata, false);
	}
	return true;
}

HARBOL_EXPORT void harbol_plugin_clear(struct HarbolPlugin *const restrict pl, HarbolPluginEvent unload_fn, void *const restrict userdata) {
	if( unload_fn != NULL ) {
		(*unload_fn)(pl, userdata, false);
	}
	
	free(pl->path); pl->path = NULL;
	if( pl->dll != NULL ) {
		MODULE_CLOSE(pl->dll); pl->dll = NULL;
	}
	*pl = ( struct HarbolPlugin ){0};
}

HARBOL_EXPORT bool harbol_plugin_changed(struct HarbolPlugin *const pl) {
	if( pl->path==NULL ) {
		pl->err = HarbolPluginErrNoPath;
		return false;
	}
	
	struct stat result;
	if( !stat(pl->path, &result) ) {
		const bool change = pl->last_write != result.st_mtime;
		if( change ) {
			pl->last_write = result.st_mtime;
		}
		return change;
	}
	return false;
}

HARBOL_EXPORT bool harbol_plugin_reload(struct HarbolPlugin *const restrict pl, HarbolPluginEvent load_fn, HarbolPluginEvent unload_fn, void *const restrict userdata) {
	if( pl->path==NULL ) {
		pl->err = HarbolPluginErrNoPath;
		return false;
	}
	
	if( pl->dll != NULL ) {
		if( unload_fn != NULL ) {
			(*unload_fn)(pl, userdata, true);
		}
		MODULE_CLOSE(pl->dll);
	}
	
	pl->dll = MODULE_LOAD(pl->path);
	if( pl->dll==NULL ) {
		pl->err = HarbolPluginErrCantReload;
		return false;
	}
	if( load_fn != NULL ) {
		(*load_fn)(pl, userdata, true);
	}
	return true;
}

HARBOL_EXPORT const char *harbol_plugin_get_err(const struct HarbolPlugin *const pl) {
	switch( pl->err ) {
		case HarbolPluginErrNone:        return "Harbol Plugins :: No Error";
		case HarbolPluginErrNoPath:      return "Harbol Plugins :: Err **** No file path recorded for the plugin. ****";
		case HarbolPluginErrBadPath:     return "Harbol Plugins :: Err **** file path failed to initialize/allocate. ****";
		case HarbolPluginErrLibLoadFail: return "Harbol Plugins :: Err **** DLL failed to load. ****";
		case HarbolPluginErrNoLibLoaded: return "Harbol Plugins :: Err **** No DLL is loaded. ****";
		case HarbolPluginErrCantReload:  return "Harbol Plugins :: Err **** DLL failed to reload. ****";
		default:                         return "Harbol Plugins :: Err **** Unknown Error. ****";
	}
}

/*
HARBOL_EXPORT NO_NULL void harbol_plugin_info(const struct HarbolPlugin *const pl) {
	printf("Plugin:: DLL - '%p'\n", pl->dll);
	if( pl->path != NULL ) {
		printf("Plugin:: Path - '%s'\n", pl->path);
	}
	printf("Plugin:: Last Write Time - '%li'\n", pl->last_write);
}
*/

HARBOL_EXPORT void *harbol_plugin_get_obj(struct HarbolPlugin *const restrict pl, const char name[static 1]) {
	if( pl->dll==NULL ) {
		pl->err = HarbolPluginErrNoLibLoaded;
		return NULL;
	}
	pl->err = HarbolPluginErrNone;
	return( void* )( uintptr_t )(MODULE_GET_OBJ(pl->dll, name));
}


static NEVER_NULL(1) void _dir_open(tinydir_dir *const restrict dir, HarbolPluginDirEvent dir_fn, void *const restrict userdata, const char ext[static 1]) {
	while( dir->has_next ) {
		tinydir_file *file = &( tinydir_file ){0};
		if( tinydir_readfile(dir, file) < 0 ) {
			continue;
		} else if( file->is_dir ) {
			if( file->name[0]=='.' ) {
				/// advance if parent dir dot.
				goto dir_iter_loop;
			}
			tinydir_dir *const restrict sub_dir = &( tinydir_dir ){0};
			if( tinydir_open(sub_dir, file->path) < 0 ) {
				/// jumping to tinydir_next at end of loop so we can advance the dir iterator.
				goto dir_iter_loop;
			} else {
				_dir_open(sub_dir, dir_fn, userdata, ext);
			}
		} else if( !strcmp(file->extension, ext) ) {
			(*dir_fn)(file->path, file->name, userdata);
		}
	dir_iter_loop:;
		if( tinydir_next(dir) < 0 ) {
			break;
		}
	}
	tinydir_close(dir);
}

HARBOL_EXPORT bool harbol_plugin_dir_open(const char dir[static 1], HarbolPluginDirEvent dir_fn, void *const restrict userdata, const char custom_ext[static 1]) {
	/// 'FILENAME_MAX' is defined in stdio.h
	char currdir[FILENAME_MAX] = {0};
#ifdef OS_WINDOWS
	if( GetCurrentDirectory(sizeof currdir, currdir) != 0 )
#else
	if( getcwd(currdir, sizeof currdir) != NULL )
#endif
	{
		char *restrict pl_dir = sprintf_alloc("%s%s%s", currdir, DIR_SEPARATOR, dir);
		if( pl_dir==NULL )
			return false;
		
		tinydir_dir *const restrict dir_handle = &( tinydir_dir ){0};
		if( tinydir_open(dir_handle, pl_dir) < 0 ) {
			fprintf(stderr, "Harbol Plugin Directory Error: **** unable to open dir: '%s' ****\n", pl_dir);
			tinydir_close(dir_handle);
		} else {
			_dir_open(dir_handle, dir_fn, userdata, custom_ext==NULL ? LIB_EXT : custom_ext);
		}
		free(pl_dir); pl_dir = NULL;
		return true;
	}
	return false;
}
