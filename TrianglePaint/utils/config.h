/* -----------------------------
* File   : config.h
* Author : Yuki Chai
* Created: 2017.03.31
* Project: Yuki
*/
#pragma once
#ifndef __YUKI_UTILS_CONFIG_H__
#define __YUKI_UTILS_CONFIG_H__

#include <map>
#include <string>
#include <fstream>

namespace Yuki {
	class Config {
	private:
		std::map<std::string, std::string> values;
	public:
		// initialize from config file
		Config(const char *file_path)  {
			std::ifstream fin(file_path);
			if (!fin.is_open()) {
				//console_error_exit("Cannot find config file!");
			}

			std::string key;
			std::string val;
			while (fin >> key) {
				if (key[0] == '#') {
					std::getline(fin, key);
					continue;
				}
				fin >> val;
				values[key] = val;
				std::getline(fin, key);
			}
		}

		// overwrite the default value of v, if exit the key
		template <class T>
		void get(std::string key, T &v) {
			std::map<std::string, std::string>::iterator it;
			it = values.find(key);
			if (it != values.end()) {
				v = (T)(atof(it->second.c_str()));
			}
		}
		template <>
		void get(std::string key, std::string &v) {
			std::map<std::string, std::string>::iterator it;
			it = values.find(key);
			if (it != values.end()) {
				v = it->second;
			}
		}
	};
}

#endif  // !__YUKI_UTILS_CONFIG_H__