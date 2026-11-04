#pragma once
namespace qol { inline int clamp_bonus(long long n){ return n<0?0:(n>100000?100000:static_cast<int>(n)); } }
