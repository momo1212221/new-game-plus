#include <cassert>
#include "logic.hpp"
int main(){ assert(qol::clamp_bonus(-1)==0); assert(qol::clamp_bonus(5000)==5000); assert(qol::clamp_bonus(200000)==100000); }
