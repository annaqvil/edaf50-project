#pragma once

#include <sys/stat.h>
#include <string>
#include <algorithm>
#include <stdio.h>

#if defined(_WIN32)
	#include <filesystem>
	#include <direct.h>
#endif

int makeDirectory(const std::string address) {
#if defined(_WIN32)
	//std::replace(address.begin(), adress.end(), '/', '\\'); // replace all '/' to '\\'
	return _mkdir(address.c_str());
#else
	//std::replace(address.begin(), address.end(), '\\', '/'); // replace all '\\' to '/'
	mode_t mode = 0755;
	return mkdir(address.c_str(), mode);
#endif
}

int deleteDirectory(const std::string address) {
#if defined(_WIN32)
	
	return rmdir(address.c_str());
#else
	std::string command = "rm -r " + address;
	return system(command.c_str());
#endif
}

int deleteFile(const std::string address) {
	return remove(address.c_str());
}