#include "lvl_loader.h"

std::pair<int, int> lvl_loader::get_simplified_y(std::vector<std::vector<int>>& layout)
{
	int ymin = layout[1].size(), ymax = 0;
	for (int i = 0; i < layout[0].size(); i++)
	{
		for (int j = 0; j < layout.size(); j++)
		{
			if (layout[i][j] != 0)
			{
				ymin = std::min(ymin, j); // should give 10 or 9
				ymax = std::max(ymax, j); // should give 11 or 10
			}
		}
	}
	return { ymin, ymax };
}

std::pair<int, int> lvl_loader::get_simplified_x(std::vector<std::vector<int>>& layout)
{
	int xmin = layout.size(), xmax = 0;
	for (int i = 0; i < layout[0].size(); i++)
	{
		for (int j = 0; j < layout.size(); j++)
		{
			if (layout[j][i] != 0)
			{
				xmin = std::min(xmin, j); // should give 10 or 9
				xmax = std::max(xmax, j); // should give 11 or 10
			}
		}
	}
	return { xmin, xmax };
}
void lvl_loader::croplayout(std::vector<std::vector<int>>& layout, int p_xmin, int p_ymin, int p_xmax, int p_ymax)
{
	std::vector<std::vector<int>> simplified_layout;
	for (int i = p_xmin; i <= p_xmax; i++)
	{
		std::vector<int> row;
		for (int j = p_ymin; j <= p_ymax; j++)
		{
			row.push_back(layout[i][j]);
		}
		simplified_layout.push_back(row);
	}
	layout = simplified_layout;
}
void lvl_loader::read_from_file(std::string filename, std::vector<std::vector<int>>& layout)
{
	std::string line;
	std::ifstream file(filename);
	if (file.is_open())
	{
		std::set<int> _size;
		while (getline(file, line))
		{
			std::vector<int> row;
			_size.insert(line.size());
			if (_size.size() > 1)
				throw std::invalid_argument("Bad file");
			for (char c : line)
				row.push_back(atoi(std::string(1, c).c_str()));
			layout.push_back(row);
		}
		file.close();
	}
}

std::pair<std::pair<int, int>, std::pair<int, int> > lvl_loader::get_eNodes(std::vector<std::vector<int>>& layout)
{
	std::pair<int, int>  start(0, 0), end(0, 0);
	for (int i = 0; i < layout.size(); i++)
	{
		for (int j = 0; j < layout[0].size(); j++)
		{
			if (layout[i][j] == 3)
				start = { i,j };
			else if (layout[i][j] == 4)
				end = { i,j };
		}
	}
	return { start,end };
}

lvl_loader::levelData lvl_loader::get_lvl_data()
{
	lvl_loader::levelData data;
	try
	{
		read_from_file(TEST_LEVEL, data.layout);
		if (data.layout.size() == 0)
			throw std::exception("Could not read data");
		auto [p_xmin, p_xmax] = get_simplified_x(data.layout);
		auto [p_ymin, p_ymax] = get_simplified_y(data.layout);
		croplayout(data.layout, p_xmin, p_ymin, p_xmax, p_ymax);
	}
	catch (std::invalid_argument e)
	{
		std::cout << e.what();
	}
	catch (std::exception e)
	{
		std::cout << e.what();
	}

	auto enodes = get_eNodes(data.layout);

	data.startNode = enodes.first;
	data.endNode = enodes.second;

	return data;
}