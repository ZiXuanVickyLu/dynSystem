#pragma once
#include "Module/ConstraintModule.h"
#include "Topology/TetrahedronSet.h"
#include "VkUniform.h"
#include "ElasticBody.h"

namespace dyno {

	class ExplicitSolver : public ConstraintModule
	{
	
	public:
		typedef typename float Real;
		typedef typename Vec3f Coord;
		typedef typename Mat3f Matrix;
		typedef typename ElasticBody::Tetrahedron Tetrahedron; //vec4

		ExplicitSolver();
		~ExplicitSolver() override;

		void constrain() override;
		
	public:
		DEF_VAR_IN(Real, TimeStep, "Time Step");

		/**
		 * @brief Particle positions
		 */
		DEF_ARRAY_IN(Coord, Position, DeviceType::GPU, "Input vertex position");

		/**
		 * @brief Particle velocities
		 */
		DEF_ARRAY_IN(Coord, Velocity, DeviceType::GPU, "Input vertex velocity");

		DEF_ARRAY_IN(Coord, RestShape, DeviceType::GPU, "Input rest position");

		DEF_ARRAY_IN(Tetrahedron, Tetrahedron, DeviceType::GPU, "Input tet");

		DEF_ARRAY_IN(Coord, Color, DeviceType::GPU, "Input colorMap");

		DEF_VAR_IN(Real, E, "");

		DEF_VAR_IN(Real, Nu, "");

	private:
		void init(); //only once

		void timeIntegration();

		void calculateMomentum();

		//assume mass density = 1.0.
		//TODO: try other mass density.
		DArray<Matrix> m_F;				//Deformation Gradient
		DArray<Matrix> m_B;				//Rest Shape Matrix inverse.
		DArray<Real> m_J;				//Deformed volume
		DArray<Vec3f> m_force;			//force 
		DArray<Matrix> m_PK1;			//Stress PK1
		DArray<Real> m_volume;			// Rest shape volume
		DArray<Matrix> m_H;				// element force
		Real mu;
		Real lambda;					// two lame parameters

		VkConstant<uint> m_EleNum;
		VkConstant<uint> m_VertexNum;
	};



}