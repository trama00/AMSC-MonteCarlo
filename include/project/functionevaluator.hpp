#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "../muparser-2.3.4/include/muParser.h"


double evaluateFunction(const std::string &expression, const std::vector<double> &point, mu::Parser &parser);