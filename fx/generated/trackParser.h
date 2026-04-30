#pragma once
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>

inline bool parse_generated(std::istream& in, _track& t) {
    memset(&t, 0, sizeof(_track));
    std::string line; int i_ch = -1, i_cl = -1; bool found = false;
    while (std::getline(in, line)) {
        if (line.find('.') == std::string::npos) continue;
        found = true;
        if (line.find(".maxClipLen") != std::string::npos) {
            size_t eq = line.find('='); if (eq != std::string::npos) {
                const char* v = line.c_str() + eq + 1;
                while (*v == ' ' || *v == '\t') v++;
                if (line.find("off") != std::string::npos) t.maxClipLen = 0;
                else if (line.find("on") != std::string::npos) t.maxClipLen = 1;
                else t.maxClipLen = std::atoi(v);
            }
            continue;
        }
        if (line.find(".masterBPM") != std::string::npos) {
            size_t eq = line.find('='); if (eq != std::string::npos) {
                const char* v = line.c_str() + eq + 1;
                while (*v == ' ' || *v == '\t') v++;
                if (line.find("off") != std::string::npos) t.channels[i_ch].masterBPM = 0;
                else if (line.find("on") != std::string::npos) t.channels[i_ch].masterBPM = 1;
                else t.channels[i_ch].masterBPM = std::atoi(v);
            }
            continue;
        }
        if (line.find(".volume") != std::string::npos) {
            size_t eq = line.find('='); if (eq != std::string::npos) {
                const char* v = line.c_str() + eq + 1;
                while (*v == ' ' || *v == '\t') v++;
                if (line.find("off") != std::string::npos) t.channels[i_ch].volume = 0;
                else if (line.find("on") != std::string::npos) t.channels[i_ch].volume = 1;
                else t.channels[i_ch].volume = std::atoi(v);
            }
            continue;
        }
        if (line.find(".channels") != std::string::npos) {
            if (++i_ch >= 32) return false;
            i_cl = -1; t.channelsCount = i_ch + 1;
            continue;
        }
    }
    return found;
}
