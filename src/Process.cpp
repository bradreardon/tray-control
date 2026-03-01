#include "Process.h"

std::string getProcessNameFromPid(pid_t pid) {
    std::string processName;
    std::string commPath = "/proc/" + std::to_string(pid) + "/comm";
    std::ifstream commFile(commPath);

    if (commFile.is_open()) {
        std::getline(commFile, processName);
        commFile.close();
    } else {
         processName = "<unknown>";
    }
    return processName;
}

std::string getProcessCmdlineFromPid(pid_t pid) {
    std::string processCmdline;
    std::string cmdlinePath = "/proc/" + std::to_string(pid) + "/cmdline";
    std::ifstream cmdlineFile(cmdlinePath);

    if (cmdlineFile.is_open()) {
        std::getline(cmdlineFile, processCmdline);
        cmdlineFile.close();

        // cmdline uses null terminators, replace with spaces for readability if needed
        for (char &c : processCmdline) {
            if (c == '\0') c = ' ';
        }

        // strip final space
        if (processCmdline.back() == ' ') {
            processCmdline.pop_back();
        }
    } else {
        processCmdline = "<unknown>";
    }
    return processCmdline;
}