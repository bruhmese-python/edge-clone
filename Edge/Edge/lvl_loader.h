#ifndef LVL_LOADER 
#define LVL_LOADER

#define TEST_LEVEL "lvl1.lvl"

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <exception>
#include <vector>
#include <algorithm>
#include <map>

#pragma once


namespace lvl_loader
{
	struct levelData
	{
		std::vector<std::vector<int>> layout;
		std::pair<int, int> startNode{ 0,0 }, endNode{0,0};
	};

	std::pair<int, int> get_simplified_y(std::vector<std::vector<int>>& layout);

	std::pair<int, int> get_simplified_x(std::vector<std::vector<int>>& layout);

	void croplayout(std::vector<std::vector<int>>& layout, int p_xmin, int p_ymin, int p_xmax, int p_ymax);

	void read_from_file(std::string filename, std::vector<std::vector<int>>& layout);

	std::pair<std::pair<int, int>, std::pair<int, int> > get_eNodes(std::vector<std::vector<int>>&);

	levelData get_lvl_data();

}
#endif 