#include "Cloth.h"
#include "VulkanTools.h"
#include "VkSystem.h"
#include "VkContext.h"
#include "VkTransfer.h"
#include "VkProgram.h"
#include "Topology/TriangleSet.h"
#include "Topology/PointSet.h"

#include <vector>
#include <set>
#include <random>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

namespace dyno
{
#define WORKGROUP_SIZE 64

	Cloth::Cloth(std::string name)
		: dyno::Node()
	{
		auto ptSet = std::make_shared<PointSet>();
		this->stateTopology()->setDataPtr(ptSet);
	}

	Cloth::~Cloth()
	{
	}

	void Cloth::updateStates()
	{
	}

	void Cloth::resetStates()
	{
	}

	void Cloth::loadObjFile(std::string filename)
	{
		if (filename.size() < 5 || filename.substr(filename.size() - 4) != std::string(".obj")) {
			std::cerr << "Error: Expected OBJ file with filename of the form <name>.obj.\n";
			exit(-1);
		}

		std::ifstream infile(filename);
		if (!infile) {
			std::cerr << "Failed to open. Terminating.\n";
			exit(-1);
		}

		int ignored_lines = 0;
		std::string line;
		std::vector<dyno::Vec3f> vertList;
	
		while (!infile.eof()) {
			std::getline(infile, line);

			//.obj files sometimes contain vertex normals indicated by "vn"
			if (line.substr(0, 1) == std::string("v") && line.substr(0, 2) != std::string("vn")) {
				std::stringstream data(line);
				char c;
				dyno::Vec3f point;
				data >> c >> point[0] >> point[1] >> point[2];
				vertList.push_back(point);
			}
			else {
				++ignored_lines;
			}
		}
		infile.close();

		auto topo = std::dynamic_pointer_cast<PointSet>(this->stateTopology()->getDataPtr());

		topo->mPoints.resize(vertList.size());

		topo->mPoints.assign(vertList);


		topo->update();

		vertList.clear();
	}
}