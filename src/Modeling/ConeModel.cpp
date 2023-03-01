#include "ConeModel.h"

#include "GLSurfaceVisualModule.h"
#include "GLWireframeVisualModule.h"



namespace dyno
{
	template<typename TDataType>
	ConeModel<TDataType>::ConeModel()
		: ParametricModel<TDataType>()
	{
		this->varRow()->setRange(2, 50);
		this->varColumns()->setRange(3, 50);
		this->varRadius()->setRange(0.001f, 10.0f);
		this->varHeight()->setRange(0.001f, 10.0f);

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
	void ConeModel<TDataType>::resetStates()
	{
		auto center = this->varLocation()->getData();
		auto rot = this->varRotation()->getData();
		auto scale = this->varScale()->getData();

		auto radius = this->varRadius()->getData();
		auto row = this->varRow()->getData();
		auto columns = this->varColumns()->getData();
		auto height = this->varHeight()->getData();

		TCone3D<Real> cone;
		cone.row = row;
		cone.columns = columns;
		cone.radius = radius;
		cone.height = height;

		this->outCone()->setValue(cone);

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
		Real x, y, z;


		//�����ǲ����Ĺ���


		for (int j = 0; j < columns; j++) {

			temp_angle = j * angle;

			x = sin(temp_angle) * radius;
			y = 0;
			z = cos(temp_angle) * radius;

			vertices.push_back(Coord(x, y, z));
		}


		int pt_side_len = vertices.size();


		//�����ǵײ����ϲ���Ĺ���

		for (int i = 1; i <= row_i; i++)
		{
			float offset = i / (float(row_i) - i);

			for (int p = 0; p < columns; p++)
			{
				Coord buttompt = { (vertices[p][0] + offset * 0) / (1 + offset), (vertices[p][1] + offset * 0) / (1 + offset), (vertices[p][2] + offset * 0) / (1 + offset) };

				vertices.push_back(buttompt);
			}

		}

		for (int i = 1; i <= row_i; i++)
		{
			float offset = i / (float(row_i) - i);

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
			vertices[i][1] -= 1*height / 3;
			vertices[i] = RV(vertices[i] * scale + RV(center));
		}


		//�����ǵ���Ͷ���Ĺ���

		//�����ǵ���Ͷ���Ĺ���
		//����ԭ�еĵ�����pt_side_len,

		int pt_len = vertices.size() - 2;
		int top_pt_len = vertices.size() - 2 - pt_side_len;
		int addnum = 0;

		for (int s = 0; s < row; s++)  //�ڲ�ѭ������ÿһȦÿһ��
		{
			int temp = 0;
			//****************�Ƿ�����Ȧ������Ȧʹ���ĸ���Χ������������*****************//
			if (s != row_i - 1)
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

		for (int s = 0; s < row_i; s++)  //�ڲ�ѭ������ÿһȦÿһ��
		{
			int temp = 0;
			//****************�Ƿ�����Ȧ������Ȧʹ���ĸ���Χ������������*****************//
			if (s != row_i - 1)
			{
				for (int i = 0; i < columns; i++)
				{
					//****************���ж��Ƿ�������һȦ���ǵĻ������������*****************//
					if (s == 0)
					{
						temp = i + pt_side_len - columns;  //iΪ0-columns����ţ���+ x * (pt_side_len - columns)����Ϊ������ŵı仯�������յó����� �ϡ���һȦ�����
						addnum = columns + row_i * columns;
					}
					else
					{
						temp = pt_side_len + columns * (row_i - 1) + i + unsigned(s) * columns;
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
					temp = pt_side_len + z + unsigned(s - 1) * columns + row_i * columns;
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



		triangleSet->setPoints(vertices);
		triangleSet->setTriangles(triangle);

		triangleSet->update();

		vertices.clear();
		triangle.clear();
	}

	template<typename TDataType>
	void ConeModel<TDataType>::disableRender() {
		glModule->setVisible(false);
	};

	DEFINE_CLASS(ConeModel);
}