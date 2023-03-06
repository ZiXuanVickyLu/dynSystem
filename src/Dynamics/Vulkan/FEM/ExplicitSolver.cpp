#include "ExplicitSolver.h"
#include "Node.h"
#include "Matrix/MatrixFunc.h"
#define WORKGROUP_SIZE 64

struct Push1 {
	Real dt; uint num;
	Push1(Real dt_, uint n) : dt(dt_), num(n) {};
	Push1() = default;
};

struct Push2 {
	Real la; Real mu; uint num;
	Push2(Real la_, Real mu_, uint n) : la(la_), mu(mu_), num(n) {};
	Push2() = default;
};

struct Push3 {
	uint vNum; uint eNum;
	Push3(uint vNum_, uint eNum_) : vNum(vNum_), eNum(eNum_) {};
	Push3() = default;
};

namespace dyno
{
	

	ExplicitSolver::ExplicitSolver()
		: ConstraintModule()
	{
		this->addKernel(
			"Integrate",
			std::make_shared<VkProgram>(
				BUFFER(Vec3f),	//inout: vertex position
				BUFFER(Vec3f),	//inout: vertex velocity
				BUFFER(Vec3f),	//in: vertex force
				CONSTANT(Push1)	//const: vNum
			)
		);
		kernel("Integrate")->load(getAssetPath() + "shaders/glsl/elastic/Integrate.comp.spv");

		this->addKernel(
			"Init",
			std::make_shared<VkProgram>(
				BUFFER(Vec3f),	//inout: vertex position
				BUFFER(Tetrahedron),	//in: tet
				BUFFER(Mat3f),	//inout: m_B
				BUFFER(Real),	//inout: volume
				CONSTANT(uint)  //in:eNum 
				)
		);
		kernel("Init")->load(getAssetPath() + "shaders/glsl/elastic/Init.comp.spv");

		this->addKernel(
			"Compute",
			std::make_shared<VkProgram>(
				BUFFER(Vec3f),	//inout: vertex position
				BUFFER(Tetrahedron),	//inout: tet
				BUFFER(Mat3f),	//inout: m_B
				BUFFER(Mat3f),  //out: m_F
				BUFFER(Mat3f),  //out: m:PK1
				BUFFER(Real),	//inout: m_J
				CONSTANT(Push2) //const 
				)
		);
		kernel("Compute")->load(getAssetPath() + "shaders/glsl/elastic/Compute.comp.spv");

		this->addKernel(
			"ComputeForce",
			std::make_shared<VkProgram>(
				BUFFER(Mat3f),  //in: PK1
				BUFFER(Mat3f),	//inout: element force
				BUFFER(Tetrahedron),	//inout: tet
				BUFFER(Real),	//inout: volume
				BUFFER(Mat3f),   //inout: m_B
				CONSTANT(uint)  //in:eNum 
				)
		);
		kernel("ComputeForce")->load(getAssetPath() + "shaders/glsl/elastic/ComputeForce.comp.spv");

		this->addKernel(
			"ComputeVertexForce",
			std::make_shared<VkProgram>(
				BUFFER(Mat3f),	//inout: element force
				BUFFER(Tetrahedron),	//inout: tet
				BUFFER(Vec3f),   //inout: force
				CONSTANT(Push3)  //in:eNum 
				)
		);
		kernel("ComputeVertexForce")->load(getAssetPath() + "shaders/glsl/elastic/ComputeVertexForce.comp.spv");

		Log::sendMessage(Log::Info, "Explicit Solver created \n");
	}

	ExplicitSolver::~ExplicitSolver()
	{	
		m_F.clear();				
		m_J.clear();		
		m_B.clear();
		m_H.clear();
		m_force.clear();
		m_volume.clear();
		m_PK1.clear();	
		Log::sendMessage(Log::Info, "Explicit Solver release \n");
	}

	void ExplicitSolver::constrain()
	{

		Log::sendMessage(Log::Info, "Explicit Solver constrain \n");

		this->init();//only once
	
		calculateMomentum();

		timeIntegration();


	}

	void ExplicitSolver::init()
	{
		//init volume
		if (this->m_F.size() != this->inPosition()->getData().size());
		{
			auto vNum = this->inPosition()->getData().size();
			auto eNum = this->inTetrahedron()->getData().size();
			this->m_F.resize(eNum);
			this->m_J.resize(eNum);
			this->m_force.resize(vNum);
			this->m_H.resize(eNum);
			this->m_PK1.resize(eNum);
			this->m_volume.resize(eNum);
			this->m_B.resize(eNum);

			Real E = Real(this->inE()->getData());
			Real nu = Real(this->inNu()->getData());
			mu = E / (2.0f * (1.0f + nu));
			lambda = E * nu / ((1.0f + nu) * (1.0f - 2.0f * nu));

			m_EleNum.setValue(eNum);
			m_VertexNum.setValue(vNum);
			//init 
			kernel("Init")->flush(
				vkDispatchSize(eNum, WORKGROUP_SIZE),
				this->inRestShape()->getDataPtr()->handle(),
				this->inTetrahedron()->getDataPtr()->handle(),
				m_B.handle(),
				m_volume.handle(),
				&m_EleNum);

		}
	}

	void ExplicitSolver::timeIntegration()
	{
		//adopt forward-Euler time integration
		auto vNum = this->inPosition()->getData().size();

		

		VkConstant<Push1> p; p.setValue(Push1(this->inTimeStep()->getData(), vNum));

		Real dt_ = this->inTimeStep()->getData();
		kernel("Integrate")->flush(
			vkDispatchSize(vNum, WORKGROUP_SIZE),
			this->inPosition()->getDataPtr()->handle(),
			this->inVelocity()->getData().handle(),
			m_force.handle(),
			&p);

 		CArray<Vec3f> force_, vel_; 
		CArray<Mat3f> P; P.assign(m_H);
		CArray<Tetrahedron> tet; tet.assign(this->inTetrahedron()->getData());
		force_.assign(m_force);
		vel_.assign(this->inVelocity()->getData());	

	}

	void ExplicitSolver::calculateMomentum()
	{
		//calc F, J, PK1
		auto eNum = this->inTetrahedron()->getData().size();
		auto vNum = this->inPosition()->getData().size();

		VkConstant<Push2> p; p.setValue(Push2(this->lambda,this->mu, eNum));

		kernel("Compute")->flush(
			vkDispatchSize(eNum, WORKGROUP_SIZE),
			this->inPosition()->getDataPtr()->handle(),
			this->inTetrahedron()->getDataPtr()->handle(),
			m_B.handle(),
			m_F.handle(),
			m_PK1.handle(),
			m_J.handle(),
			&p);

		
		//calc H, force
	
		kernel("ComputeForce")->flush(
			vkDispatchSize(eNum, WORKGROUP_SIZE),
			m_PK1.handle(),
			m_H.handle(),
			this->inTetrahedron()->getDataPtr()->handle(),
			m_volume.handle(),
			m_B.handle(),
			&m_EleNum);

		VkConstant<Push3> p2; p2.setValue(Push3(vNum, eNum));
		kernel("ComputeVertexForce")->flush(
			vkDispatchSize(vNum, WORKGROUP_SIZE),
			m_H.handle(),
			this->inTetrahedron()->getDataPtr()->handle(),
			m_force.handle(),
			&p2);
	
	}

}