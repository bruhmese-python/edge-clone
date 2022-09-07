#include<irrlicht.h>
#include<vector>
#ifndef UTILS 
#define UTILS

#pragma once

namespace utils
{
	std::vector<irr::f32> get_dimensions(irr::scene::IAnimatedMesh* mesh);
	float n4_Avg(std::vector<std::vector<int>>& layout, int i, int j);
}

#endif