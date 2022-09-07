#include "utils.h"

std::vector<irr::f32> utils::get_dimensions(irr::scene::IAnimatedMesh* mesh)
{
	irr::core::aabbox3d<irr::f32> aBox = mesh->getBoundingBox();

	irr::core::vector3d<irr::f32> gEdges[8];
	aBox.getEdges(&gEdges[0]);

	return {
		gEdges[1].Y - gEdges[0].Y,
		gEdges[4].X - gEdges[0].X,
		gEdges[2].Z - gEdges[0].Z
	};
}

float utils::n4_Avg(std::vector<std::vector<int>>& layout, int i, int j)
{
	if (layout.size() > 0 && i > -1 && j > -1)
	{
		int vertSum = 0, horSum = 0;
		int _i, _j;
		int s_i, s_j;

		s_i = _i = ((i - 1) < 0) ? 0 : i - 1;
		s_j = _j = ((j - 1) < 0) ? 0 : j - 1;

		for (; _i < layout.size() && _i <= i + 1; _i++)
		{
			vertSum += layout[_i][j];
		}
		for (; _j < layout[0].size() && _j <= j + 1; _j++)
		{
			horSum += layout[i][_j];
		}
		float avg = float(vertSum + horSum - layout[i][j]) / float((_i - s_i) * (_j - s_j));
		return avg;
	}
	else
	{
		return NULL;
	}
}
