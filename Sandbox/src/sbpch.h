#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#ifndef MH_DEBUG
#include <robin_hood.h>
#else
#include <unordered_map>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <Mahakam.h>