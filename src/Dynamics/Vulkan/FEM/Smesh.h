#pragma once
#include <vector>
#include <set>
#include "Vector.h"
#include "Module/TopologyModule.h"
#include "ElasticBody.h"
namespace dyno
{
	class Smesh_t {

	public:
		typedef typename Vector<uint, 4>	Tetrahedron;
		typedef typename Vector<uint, 3>	Triangle;
		void loadFile(std::string filename);
		void loadNodeFile(std::string filename);
		void loadNodeFile(std::string filename, Vec3f move);
		void loadTriangleFile(std::string filename);
		void loadTetFile(std::string filename);

		std::vector <dyno::Vec3f> m_points;
		std::vector<Triangle> m_triangles;
		std::vector<Tetrahedron> m_tets;

	};

}