//
// Created by 张俏悦 on 2025/10/17.
//

#ifndef TESTSERVER_ERROR_H
#define TESTSERVER_ERROR_H

#include <string>

struct Error :std::exception{
    std::string error;

    Error() = default;

    explicit Error(const char * str);
};

#endif //TESTSERVER_ERROR_H