#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>


std::string getProcessNameFromPid(pid_t pid);

std::string getProcessCmdlineFromPid(pid_t pid);