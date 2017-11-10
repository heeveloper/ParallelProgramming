#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <sstream>
#include <fstream>
#include <stdint.h>

using namespace std;

void makeLog(size_t tid);
void printLog(size_t tid, size_t commit_id, size_t i, size_t j, size_t k);

#endif
