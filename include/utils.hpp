#pragma once
#include "env.hpp"

#include <functional>
#include <string>

using namespace std;

using LanderController = function<double(const LanderState &)>;
void log2csv(LanderController controller, const string &filename);
