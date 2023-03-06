#pragma once

#include <string>
#include <vector>
#include <utility>
#include "VkUniform.h"
#include "Node.h"
#include "Smesh.h"
namespace dyno
{

	class ElasticBody : public Node
	{
	
	public:
	

		typedef typename float Real;
		typedef typename Vec3f Coord;
		typedef typename Vector<uint, 4>	Tetrahedron;
		typedef typename Vector<uint, 3>	Triangle;

		typedef struct {
			DArray<Coord> m_Points;
			DArray<Vector<uint, 3>> m_Triangles;
			DArray<Vector<uint, 4>> m_Tetrahedrons;
		}TetrahedronSet;

		ElasticBody(std::string name = "default");
		virtual ~ElasticBody();

		void resetStates() override;
		inline void setE(Real E_);
		inline void setNu(Real Mu_);
		void loadTetGenFile(std::string filename, Vec3f move = Vec3f(0));
		
	protected:
	

		DEF_ARRAY_STATE(Tetrahedron, Tetrahedron, DeviceType::GPU, "tet index");

		DEF_ARRAY_STATE(Coord, Position, DeviceType::GPU, "Vertex position");

		DEF_ARRAY_STATE(Coord, RestShape, DeviceType::GPU, "Vertex rest position restored");

		DEF_ARRAY_STATE(Coord, Velocity, DeviceType::GPU, "Vertex velocity");

		DEF_VAR_STATE(Real, E, 5000, "Young Moudulus");

		DEF_VAR_STATE(Real, Nu, 0.01, "Possion Ratio");

		DEF_ARRAY_STATE(Coord, Color, DeviceType::GPU, "Color Map for Debug");
	
		TetrahedronSet tetSet;

	};
}