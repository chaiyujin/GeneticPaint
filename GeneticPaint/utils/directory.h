/* -----------------------------
* File   : directory.h
* Author : Yuki Chai
* Created: 2017.03.31
* Project: Yuki
*/
#pragma once
#ifndef __YUKI_UTILS_DIRECTORY_H__
#define __YUKI_UTILS_DIRECTORY_H__

#include <io.h>
#include <stdlib.h>
#include <direct.h>
#include <string>
#include <vector>
#include "log.h"

namespace Yuki {
	const int YUKI_MAX_PATH = 2048;
	
	class Directory {
    private:
		/* non static */
		std::string origin_path;

		/* static */
		static std::string current_path() {
			char path[YUKI_MAX_PATH];
			_getcwd(path, YUKI_MAX_PATH);
			return unix_style(path);
		}

		static void dfs_find_files(std::string pattern, std::vector<std::string> &results, bool recursive) {
			std::string old_path = current_path();
			
			// find files in this directory
			intptr_t handle;
			_finddata_t info;
			if ((handle = _findfirst(pattern.c_str(), &info)) != -1) {
				// find the first
				do {
					// if not a directory, insert into result
					if (!(info.attrib & _A_SUBDIR)) {
						std::string file_path = old_path + info.name;
						results.emplace_back(file_path);
						DEBUG("Find file: %s\n", file_path.c_str());
					}
				} while (_findnext(handle, &info) == 0);
				_findclose(handle);
			}

			// if recursive, find in sub directories.
			if (recursive) {
				std::vector<std::string> sub_dirs;
				if ((handle = _findfirst("*", &info)) != -1) {
					// find the first
					do {
						// sub dir
						if (info.attrib & _A_SUBDIR) {
							if (strcmp(info.name, ".") &&
								strcmp(info.name, ".."))
								sub_dirs.emplace_back(info.name);
						}
					} while (_findnext(handle, &info) == 0);
					_findclose(handle);
				}
				for (size_t i = 0; i < sub_dirs.size(); ++i) {
					_chdir(sub_dirs[i].c_str());
					dfs_find_files(pattern, results, recursive);
					_chdir("..");
				}
			}

			return;
		}

    public:
		/* non static */
		// the instance will record the current path,
		// and return back to this path when destoryed.
		Directory() {
			origin_path = current_path();
		}
		~Directory() {
			cd(origin_path);
		}

		/* static */
		// return the abs path of the file with certain parten
		static std::vector<std::string> find_files(std::string pattern = "*.*", bool recursive = false) {
			std::vector<std::string> results;
			dfs_find_files(pattern, results, recursive);
			return results;
		}

		// unix_style
		static std::string unix_style(std::string path) {
			std::string ret;
			FOR(i, path.length()) {
				if (path[i] == '\\') {
					// "\\" and "\" turn into "/"
					if (i == 0 ||
						(i > 0 && path[i - 1] != '\\')) {
						ret += '/';
					}
				}
				else {
					ret += path[i];
				}
			}
			if (*(ret.end() - 1) != '/') ret += '/';
			return ret;
		}
		// exists
		static bool exists(std::string path) { return _access(path.c_str(), 0) != -1; }
        // mkdir
		static bool mkdir(std::string path) { return _mkdir(path.c_str()) != -1; }
		// mkdirs
		static bool mkdirs(std::string path) {
			bool ret = true;
			path = unix_style(path);
			FOR(i, path.length()) {
				if (path[i] == '/') {
					if (i > 0 && path[i - 1] == ':') continue; // skip such as D:/
					std::string sub_str = path.substr(0, i);
					if (exists(sub_str)) continue; // skip if exists
					ret &= mkdir(sub_str);
				}
			}
			return ret;
		}
		// true: directory exits; false: no such directory
		static bool cd(std::string path = ".") { return _chdir(path.c_str()) == 0; }
        // get dir
		static std::string getcwd() { return current_path(); }
        // delete
		static bool delete_file(std::string path) { return !remove(path.c_str()); }
    };
}

#endif  // !__YUKI_UTILS_DIRECTORY_H__