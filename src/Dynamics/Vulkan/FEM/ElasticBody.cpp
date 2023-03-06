
#include "ElasticBody.h"
#include "Topology/TetrahedronSet.h"

#include "ExplicitSolver.h"
#include <cstdlib>
#include "Smesh.h"



namespace dyno
{

	
	ElasticBody::ElasticBody(std::string name)
		: Node(name)
	{
		this->statePosition()->allocate(); this->stateRestShape()->allocate();
		this->stateVelocity()->allocate(); this->stateColor()->allocate();
		this->stateTetrahedron()->allocate();

		auto elastic = std::make_shared<ExplicitSolver>();
		
		this->setDt(5e-4); //set timestep
		this->stateTimeStep()->connect(elastic->inTimeStep());
		this->statePosition()->connect(elastic->inPosition());
		this->stateVelocity()->connect(elastic->inVelocity());
		this->stateRestShape()->connect(elastic->inRestShape());
		this->stateColor()->connect(elastic->inColor());
		this->stateTetrahedron()->connect(elastic->inTetrahedron());
		this->stateE()->connect(elastic->inE());
		this->stateNu()->connect(elastic->inNu());
		this->animationPipeline()->pushModule(elastic);
	}


	ElasticBody::~ElasticBody()
	{
		Log::sendMessage(Log::Info, "ElasticBody released \n");
		this->tetSet.m_Points.clear();
		this->tetSet.m_Tetrahedrons.clear();
		this->tetSet.m_Triangles.clear();
	}



	void ElasticBody::resetStates()
	{
		Log::sendMessage(Log::Info, "ElasticBody reset state \n");
		
		uint vNum = this->tetSet.m_Points.size();
		uint eNum = this->tetSet.m_Tetrahedrons.size();
		std::vector<Vec3f> initBuffer; initBuffer.resize(vNum);
		for (auto& ele : initBuffer) ele = Vec3f(0);

		//Assign the state to vertex size.
		if (this->statePosition()->isEmpty() || this->statePosition()->getData().size() != vNum) {
			this->statePosition()->allocate(); this->stateRestShape()->allocate();
			this->stateVelocity()->allocate(); this->stateColor()->allocate();
			printf("vNum: %u\n", vNum);
			this->statePosition()->getData().assign(this->tetSet.m_Points);
			this->stateRestShape()->getData().assign(this->tetSet.m_Points);
			this->stateVelocity()->getData().assign(initBuffer);
			this->stateColor()->getData().assign(initBuffer);
		}

		if (this->stateTetrahedron()->isEmpty() || this->stateTetrahedron()->getData().size() != eNum) {
			this->stateTetrahedron()->allocate();
			this->stateTetrahedron()->getData().assign(this->tetSet.m_Tetrahedrons);
		}

		Log::sendMessage(Log::Info, "ElasticBody reset state finished \n");
	}

	inline void ElasticBody::setE(Real E_)
	{
		this->stateE()->setValue(E_);
	}

	inline void ElasticBody::setNu(Real Mu_)
	{
		this->stateNu()->setValue(Mu_);
	}

	void ElasticBody::loadTetGenFile(std::string filename, Vec3f move)
	{
		Smesh_t meshLoader;
		meshLoader.loadNodeFile(filename + ".node", move);
		meshLoader.loadTetFile(filename + ".ele");
		meshLoader.loadTriangleFile(filename + ".face");
		this->tetSet.m_Points.assign(meshLoader.m_points);
		this->tetSet.m_Triangles.assign(meshLoader.m_triangles);
		this->tetSet.m_Tetrahedrons.assign(meshLoader.m_tets);
		
	}

}