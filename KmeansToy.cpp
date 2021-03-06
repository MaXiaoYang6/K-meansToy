#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <functional>

#define INF 999999

using namespace std;
class Point;
class Cluster;
void initRandomCentralPoint(vector<Point>& vPoints, int vNumOfClu, vector<Point>& vCentralPoints);
double calEuclideanDis(Point vPointa, Point vPointb);
Point calEuclideanCentralPoint(vector<Point> points);

class Point
{
private:
	int m_IdPoint, m_IdCluster; //ID of point and ID of cluster
	vector<double> m_Values;//Each value of Point
	int m_TotalValues;// Numbers of Vlues

public:
	Point(int vTotalValues)
	{
		this->m_TotalValues = vTotalValues;
		m_IdCluster = -1;
	};
	Point(int id_point, vector<double>& values)
	{// Init the id_point and values
		this->m_IdPoint = id_point;
		m_TotalValues = values.size();

		for (int i = 0; i < m_TotalValues; i++) {
			this->m_Values.push_back(values[i]);
		}
		m_IdCluster = -1; //At the beginning, point's id_cluster equals -1
	}

	int getID()
	{
		return m_IdPoint;
	}

	void setCluster(int id_cluster)
	{
		this->m_IdCluster = id_cluster;
	}

	int getCluster()
	{
		return m_IdCluster;
	}

	double getValue(int index)
	{
		return m_Values[index];
	}

	int getTotalValues()
	{
		return m_TotalValues;
	}

	void addValue(double value)
	{
		m_Values.push_back(value);
	}

	void printPoint()
	{
		cout << "IdPoint: " << m_IdPoint << " IdCluster: " << m_IdCluster << endl;
		cout << "(";
		for (int i = 0; i < m_TotalValues; i++)
		{
			cout << getValue(i) << " ";
		}
		cout << ")" << endl;
	}
};

class Cluster
{
private:
	int m_IdCluster; //ID of cluster
	vector<Point> m_Points;//Points belongs to the cluster
	Point m_CentralPoint;
public:
	Cluster(int vIdCluster, Point vPoint) : m_IdCluster(vIdCluster), m_CentralPoint(vPoint)
	{//point is the central point
		m_Points.push_back(vPoint);//central point is also the first point of cluster
	}

	void addPoint(Point point)
	{
		m_Points.push_back(point);
	}

	bool removePoint(int id_point)
	{
		for (int i = 0; i < m_Points.size(); i++) {
			if (m_Points[i].getID() == id_point) {
				m_Points.erase(m_Points.begin() + i);
				return true;
			}
		}
		return false;
	}

	Point getPoint(int index) {
		return m_Points[index];
	}

	int getTotalPoints() {
		return m_Points.size();
	}

	int getID() {
		return m_IdCluster;
	}

	Point getCentralPoint()
	{
		return m_CentralPoint;
	}

	void printCluster()
	{
		cout << "m_IdCluster: " << m_IdCluster << endl;
		cout << "m_CentralPoint: "; 
		m_CentralPoint.printPoint();
		cout << "m_Points :";
		for (int i = 0; i < m_Points.size(); i++) {
			m_Points[i].printPoint();
		}
	}

	vector<Point> getPoints()
	{
		return this->m_Points;
	}

	void setCentralPoint(Point vTempPoint)
	{
		this->m_CentralPoint = vTempPoint;
	}
};

class KMeans
{
private:
	int m_NumOfClu;//Numbers of clusters
	int m_TotalValues, m_TotalPoints, m_MaxIterations;
	function<void(vector<Point>&, int, vector<Point>&)> m_InitCentralPointFunc;
	function<double(Point vPointa, Point vPointb)> m_CalDisBetPointsFunc;
	function<Point(vector<Point>)> m_CalCentralPointFunc;
	vector<Cluster> m_Clusters;
	//return ID of nearest center
	int getIDNearestCenter(Point& vPoint) {
		double sum = 0.0, min_dist = INF;
		int id_cluster_center = 0;
		for (int i = 0; i < m_NumOfClu; i++) {
			double dist;
			sum = 0.0;
			dist = m_CalDisBetPointsFunc(vPoint, m_Clusters[i].getCentralPoint());
			if (dist < min_dist) {
				min_dist = dist;
				id_cluster_center = i;
			}
		}
		return id_cluster_center;
	}

public:
	KMeans(int vNumOfClu, int vTotalPoints, int vTotalValues, int vMaxIterations,
		const function<void (vector<Point>&, int, vector<Point>&)>& vInitCentralPointFunc = initRandomCentralPoint,
		const function<double(Point vPointa, Point vPointb)>& vCalDisBetPointsFunc = calEuclideanDis,
		const function<Point(vector<Point>)>& vCalCentralPointFunc = calEuclideanCentralPoint)
	{
		this->m_NumOfClu = vNumOfClu;
		this->m_TotalPoints = vTotalPoints;
		this->m_TotalValues = vTotalValues;
		this->m_MaxIterations = vMaxIterations;
		this->m_InitCentralPointFunc = vInitCentralPointFunc;
		this->m_CalDisBetPointsFunc = vCalDisBetPointsFunc;
		this->m_CalCentralPointFunc = vCalCentralPointFunc;
	}

	void showResult()
	{
		cout << endl;
		cout << "Result :" << endl;
		for (int i = 0; i < m_NumOfClu; i++)
		{
			m_Clusters[i].printCluster();
			cout << endl;
		}
	}

	void run(vector<Point>& vPoints) 
	{
		vector<Point> CentralPoints;
		if (m_NumOfClu > m_TotalPoints)
			return;

		// choose numOfClu centers of the clusters
		m_InitCentralPointFunc(vPoints, m_NumOfClu, CentralPoints);

		//Set initial clusters and its centralPoints
		for (int i = 0; i < CentralPoints.size(); i++)
		{
			Cluster TempCluster(i, CentralPoints[i]);
			m_Clusters.push_back(TempCluster);
		}

		//Debug: Print points and cluster
		//for (int i = 0; i < vPoints.size(); i++)
		//	vPoints[i].printPoint();
		//for (int i = 0; i < m_Clusters.size(); i++)
		//	m_Clusters[i].printCluster();

		int Iter = 1; //Number of iterations

		while (true) {
			bool done = true;
			//associate each point to the nearest center
			for (int i = 0; i < m_TotalPoints; i++) {
				int IdOldCluster = vPoints[i].getCluster();
				int IdNearestCluster = getIDNearestCenter(vPoints[i]);
				if (IdOldCluster != IdNearestCluster) {
					if (IdOldCluster != -1)
						m_Clusters[IdOldCluster].removePoint(vPoints[i].getID());
					vPoints[i].setCluster(IdNearestCluster);
					m_Clusters[IdNearestCluster].addPoint(vPoints[i]);
					done = false;
				}
			}

			for (int i = 0; i < m_NumOfClu; i++)
			{
				Point PointTemp(m_TotalValues);
				PointTemp = m_CalCentralPointFunc(m_Clusters[i].getPoints());
				m_Clusters[i].setCentralPoint(PointTemp);
			}

			if (done == true || Iter >= m_MaxIterations)
				break;
			Iter++;
		}

		//show elements of clusters
		showResult();
	}
};

//***************************************************************************************************************
//FUNCTION??choose NumOfClu distinct values for the centers of the clusters
void initRandomCentralPoint(vector<Point>& vPoints, int vNumOfClu, vector<Point>& vCentralPoints)
{
	vector<int> ProhibitedIndexes;
	int TotalPoints = vPoints.size();
	for (int i = 0; i < vNumOfClu; i++)
	{
		while (true)
		{
			int IndexPoint = rand() % TotalPoints;
			if (find(ProhibitedIndexes.begin(), ProhibitedIndexes.end(), IndexPoint) == ProhibitedIndexes.end())
			{
				ProhibitedIndexes.push_back(IndexPoint);
				vPoints[i].setCluster(i);
				vCentralPoints.push_back(vPoints[i]);
				break;
			}
		}
	}
}

//***************************************************************************************************************
//FUNCTION??caculate the distance between two points
double calEuclideanDis(Point vPointa, Point vPointb)
{
	double Sum = 0;
	for (int i = 0; i < vPointa.getTotalValues(); i++)
	{
		Sum += pow(vPointa.getValue(i) - vPointb.getValue(i), 2.0);
	}
	return sqrt(Sum);
}

//***************************************************************************************************************
//FUNCTION??caculate the Euclidean Central Point of a set of points
Point calEuclideanCentralPoint(vector<Point> points)
{

	Point Ret(points[0].getTotalValues());
	
	for (int i = 0; i < points[0].getTotalValues(); i++)
	{
		double Sum = 0;
		for (int k = 0; k < points.size(); k++)
		{
			Sum += points[k].getValue(i);
		}
		Sum /= points.size();
		Ret.addValue(Sum);
	}
	return Ret;
}

int main()
{
	

	int TotalPoints, TotalValues, NumOfCul, MaxIterations;
	vector<Point> points;

	cout << "Please input the TotalPoints TotalValues NumOfCul MaxIterations" << endl;
	cout << "and then input ToalPoints Point each of whihc has TotalValues values: " << endl;
	cin >> TotalPoints >> TotalValues >> NumOfCul >> MaxIterations;

	srand(time(NULL));
	for (int i = 0; i < TotalPoints; i++) {
		vector<double> values;
		for (int k = 0; k < TotalValues; k++) {
			double value;
			cin >> value;
			values.push_back(value);
		}
		Point p(i, values);
		points.push_back(p);
	}

	KMeans kmeans(NumOfCul, TotalPoints, TotalValues, MaxIterations, initRandomCentralPoint, calEuclideanDis, calEuclideanCentralPoint);
	kmeans.run(points);
	return 0;
}