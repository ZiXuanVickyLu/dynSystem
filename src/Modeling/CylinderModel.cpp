#include "CylinderModel.h"

#include "GLSurfaceVisualModule.h"
#include "GLWireframeVisualModule.h"

namespace dyno
{
	template<typename TDataType>
	CylinderModel<TDataType>::CylinderModel()
		: ParametricModel<TDataType>()
	{

		this->varRow()->setRange(2, 50);
		this->varColumns()->setRange(3, 50);
		this->varRadius()->setRange(0.001f, 10.0f);
		this->varHeight()->setRange(0.001f, 10.0f);
		this->varEndSegment()->setRange(2, 39);

		this->stateTriangleSet()->setDataPtr(std::make_shared<TriangleSet<TDataType>>());



		glModule = std::make_shared<GLSurfaceVisualModule>();
		glModule->setColor(Vec3f(0.8, 0.52, 0.25));
		glModule->setVisible(true);
		this->stateTriangleSet()->connect(glModule->inTriangleSet());
		this->graphicsPipeline()->pushModule(glModule);


		auto wireframe = std::make_shared<GLWireframeVisualModule>();
		this->stateTriangleSet()->connect(wireframe->inEdgeSet());
		this->graphicsPipeline()->pushModule(wireframe);


	}

	template<typename TDataType>
	void CylinderModel<TDataType>::resetStates()
	{
		auto center = this->varLocation()->getData();
		auto rot = this->varRotation()->getData();
		auto scale = this->varScale()->getData();

		auto radius = this->varRadius()->getData();
		auto row = this->varRow()->getData();
		auto columns = this->varColumns()->getData();
		auto height = this->varHeight()->getData();
		auto end_segment = this->varEndSegment()->getData();
		
		TCylinder3D<Real> tube;
		tube.row = row;
		tube.columns = columns;
		tube.radius = radius;
		tube.height = height;
		tube.end_segment = end_segment;

		this->outCylinder()->setValue(tube);

		auto triangleSet = this->stateTriangleSet()->getDataPtr();

		Real PI = 3.1415926535;
		
		std::vector<Coord> vertices;
		std::vector<TopologyModule::Triangle> triangle;


		int columns_i = int(columns);
		int row_i = int(row);

		uint counter = 0;
		Coord Location;
		Real angle = PI / 180 * 360 / columns_i;
		Real temp_angle = angle;



		//�����ǲ����Ĺ���
		for (int k = 0; k <= row_i; k++)
		{
			Real tempy = height / row_i * k;

			for (int j = 0; j < columns_i; j++) {

				temp_angle = j * angle;

				Location = { sin(temp_angle) * radius , tempy ,cos(temp_angle) * radius };

				vertices.push_back(Location);
			}
		}

		//�����ǵײ����ϲ���Ĺ���
		
		int pt_side_len = vertices.size();

		for (int i = 1; i <= end_segment; i++)
		{
			float offset = i / (float(end_segment) - i);

			for (int p = 0; p < columns; p++)
			{
				Coord buttompt = { (vertices[p][0] + offset * 0) / (1 + offset), (vertices[p][1] + offset * 0) / (1 + offset), (vertices[p][2] + offset * 0) / (1 + offset) };

				vertices.push_back(buttompt);
			}

		}

		for (int i = 1; i <= end_segment; i++)
		{
			float offset = i / (float(end_segment) - i);

			for (int p = 0; p < columns; p++)
			{
				int top_start = pt_side_len - columns + p;

				Coord toppt = { (vertices[top_start][0] + offset * 0) / (1 + offset), (vertices[top_start][1] + offset * height) / (1 + offset), (vertices[top_start][2] + offset * 0) / (1 + offset) };

				vertices.push_back(toppt);
			}

		}


		//�����ǵײ�Բ�ļ��ϲ�Բ�ĵ�Ĺ���
		vertices.push_back(Coord(0, 0, 0));
		vertices.push_back(Coord(0, height, 0));

		//�����ǲ���Ĺ���
		for (int rowl = 0; rowl <= row_i - 1; rowl++)
		{
			for (int faceid = 0; faceid < columns_i; faceid++)
			{

				if (faceid != columns_i - 1)
				{
					
					triangle.push_back(TopologyModule::Triangle(columns_i + faceid + rowl * columns_i, 0 + faceid + rowl * columns_i, 1 + faceid + rowl * columns_i));
					triangle.push_back(TopologyModule::Triangle(columns_i + 1 + faceid + rowl * columns_i, columns_i + faceid + rowl * columns_i, 1 + faceid + rowl * columns_i));
				}
				else
				{
					triangle.push_back(TopologyModule::Triangle(1 + 2 * faceid + rowl * columns_i, 0 + faceid + rowl * columns_i, 0 + rowl * columns_i));
					triangle.push_back(TopologyModule::Triangle(1 + faceid + rowl * columns_i, 1 + 2 * faceid + rowl * columns_i, 0 + rowl * columns_i));
				}

			}
		}

		//�����ǵ���Ͷ���Ĺ���

		//�����ǵ���Ͷ���Ĺ���
		//����ԭ�еĵ�����pt_side_len,

		int pt_len = vertices.size() - 2;
		int top_pt_len = vertices.size() - 2 - pt_side_len;
		int addnum = 0;


		for (int s = 0; s < end_segment; s++)  //�ڲ�ѭ������ÿһȦÿһ��
		{
			int temp = 0;
			//****************�Ƿ�����Ȧ������Ȧʹ���ĸ���Χ������������*****************//
			if (s != end_segment - 1)
			{
				for (int i = 0; i < columns; i++)
				{
					//****************���ж��Ƿ�������һȦ���ǵĻ������������*****************//
					if (s == 0)
					{
						temp = i;  //iΪ0-columns����ţ���+ x * (pt_side_len - columns)����Ϊ������ŵı仯�������յó����� �ϡ���һȦ�����
						addnum = pt_side_len;
					}
					else
					{
						temp = pt_side_len + i + unsigned(s - 1) * columns;
						addnum = columns;
					}


					//****************�Ƿ������һ�У��ǵĻ���β�����ӣ���ֹ���ӵ㻻��*****************//
					if (i != columns - 1)
					{
						triangle.push_back(TopologyModule::Triangle(addnum + temp, temp + 1, temp));	//���ɵ���
						triangle.push_back(TopologyModule::Triangle(addnum + temp, addnum + temp + 1, temp + 1));
					}
					else
					{
						triangle.push_back(TopologyModule::Triangle(addnum + temp, temp - columns + 1, temp));	//���ɵ������һ��

						if (s == 0)		triangle.push_back(TopologyModule::Triangle(addnum + temp, temp - columns + addnum + 1, temp - columns + 1));
						else			triangle.push_back(TopologyModule::Triangle(addnum + temp, temp + 1, temp - columns + 1));

					}

				}
			}
			//****************�Ƿ�������Ȧ��������Ȧʹ���ܳ�����Բ��*****************//
			else
			{

				for (int z = 0; z < columns; z++)
				{
					temp = pt_side_len + z + unsigned(s - 1) * columns;
					if (z != columns - 1)
					{
						triangle.push_back(TopologyModule::Triangle(temp + 1, temp, pt_len));	//���ɵ�������Ȧ

					}
					else
					{
						triangle.push_back(TopologyModule::Triangle(temp - columns + 1, temp, pt_len));	//���ɵ�������Ȧ���һ����

					}
					 
				}
			}

		}
		//*************************�ϲ�************************//

		for (int s = 0; s < end_segment; s++)  //�ڲ�ѭ������ÿһȦÿһ��
		{
			int temp = 0;
			//****************�Ƿ�����Ȧ������Ȧʹ���ĸ���Χ������������*****************//
			if (s != end_segment - 1)
			{
				for (int i = 0; i < columns; i++)
				{
					//****************���ж��Ƿ�������һȦ���ǵĻ������������*****************//
					if (s == 0)
					{
						temp = i + pt_side_len - columns;  //iΪ0-columns����ţ���+ x * (pt_side_len - columns)����Ϊ������ŵı仯�������յó����� �ϡ���һȦ�����
						addnum = columns + end_segment * columns;
					}
					else
					{
						temp = pt_side_len + columns * (end_segment - 1) + i + unsigned(s) * columns;
						addnum = columns;
					}
					//****************�Ƿ������һ�У��ǵĻ���β�����ӣ���ֹ���ӵ㻻��*****************//
					if (i != columns - 1)
					{
						triangle.push_back(TopologyModule::Triangle(temp, temp + 1, addnum + temp));	//���ɵ���
						triangle.push_back(TopologyModule::Triangle(temp + 1, addnum + temp + 1, addnum + temp));
					}
					else
					{
						triangle.push_back(TopologyModule::Triangle(temp, temp - columns + 1, addnum + temp));	//���ɵ������һ��

						if (s == 0)		triangle.push_back(TopologyModule::Triangle(temp - columns + 1, temp - columns + addnum + 1, addnum + temp));
						else			triangle.push_back(TopologyModule::Triangle(temp - columns + 1, temp + 1, addnum + temp));

					}

				}
			}
			//****************�Ƿ�������Ȧ��������Ȧʹ���ܳ�����Բ��*****************//
			else
			{

				for (int z = 0; z < columns; z++)
				{
					temp = pt_side_len + z + unsigned(s - 1) * columns + end_segment * columns;
					if (z != columns - 1)
					{
						triangle.push_back(TopologyModule::Triangle(pt_len + 1, temp, temp + 1));	//���ɵ�������Ȧ

					}
					else
					{
						triangle.push_back(TopologyModule::Triangle(pt_len + 1, temp, temp - columns + 1));	//���ɵ�������Ȧ���һ����

					}

				}
			}

		}


		//�任

		Quat<Real> q = Quat<Real>(M_PI * rot[0] / 180, Coord(1, 0, 0))
			* Quat<Real>(M_PI * rot[1] / 180, Coord(0, 1, 0))
			* Quat<Real>(M_PI * rot[2] / 180, Coord(0, 0, 1));

		q.normalize();

		auto RV = [&](const Coord& v)->Coord {
			return center + q.rotate(v - center);
		};

		int numpt = vertices.size();

		for (int i = 0; i < numpt; i++)
		{
			vertices[i][1] -= height / 2;
			vertices[i] =RV( vertices[i] * scale + RV( center ));
		}


		triangleSet->setPoints(vertices);
		triangleSet->setTriangles(triangle);

		triangleSet->update();



		vertices.clear();
		triangle.clear();
		

	}


	template<typename TDataType>
	void CylinderModel<TDataType>::disableRender() {
		glModule->setVisible(false);
	};



	DEFINE_CLASS(CylinderModel);
}