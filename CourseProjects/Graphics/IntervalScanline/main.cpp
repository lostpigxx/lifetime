/*
	Author: Liang Xiao.
	Data: 2017-12-1 20:32:40
*/


#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <opencv2\opencv.hpp>  

#include "ds.h"

using namespace std;

int **framebuffer;
int edgeCounter = 0;
int polygonCounter = 0;
int zRange = 1;
int tmp = 1;

vector<polygon> polygonTable;
vector<edge> edgeTable;
map<int, edge> activeEdgeTable;
vector<xPoint> crossPoints;


void buildPolygonTableAndEdgeTable1(vector<polygon> &PT, vector<edge> &ET)
{
	// 导入obj，填充PT和ET
	vector<point> objPoints;
	
	zRange = 5000;

	char tmpBuffer[1000];
	ifstream fileIn("teapot.obj");
	while (fileIn.getline(tmpBuffer, 1000)) {
		if (tmpBuffer[0] == 'v') {
			point newPoint;
			sscanf(tmpBuffer, "v %lf %lf %lf", &newPoint.x, &newPoint.y, &newPoint.z);
			newPoint.x = (int)((newPoint.x * 1000) + 4000);
			newPoint.y = (int)((newPoint.y * 1000) + 1000);
			newPoint.z = (int)((newPoint.z * 1000) + 3000);
			objPoints.push_back(newPoint);
		}
		else if (tmpBuffer[0] == 'f') {
			int idxA, idxB, idxC;
			sscanf(tmpBuffer, "f %d %d %d", &idxA, &idxB, &idxC);
			polygon newPolygon(objPoints[idxA - 1], objPoints[idxB - 1], objPoints[idxC - 1], zRange, polygonCounter++);
			PT.push_back(newPolygon);
			//if (polygonCounter == 5111) {
			//	cout << objPoints[idxA - 1].x << endl;
			//	cout << objPoints[idxA - 1].y << endl;
			//	cout << objPoints[idxA - 1].z << endl;
			//	cout << objPoints[idxB - 1].x << endl;
			//	cout << objPoints[idxB - 1].y << endl;
			//	cout << objPoints[idxB - 1].z << endl;
			//	cout << objPoints[idxC - 1].x << endl;
			//	cout << objPoints[idxC - 1].y << endl;
			//	cout << objPoints[idxC - 1].z << endl;
			//}

			if (objPoints[idxA - 1].y != objPoints[idxB - 1].y)
				ET.push_back(edge(objPoints[idxA - 1], objPoints[idxB - 1], newPolygon.ID, edgeCounter++));
			if (objPoints[idxA - 1].y != objPoints[idxC - 1].y)
				ET.push_back(edge(objPoints[idxA - 1], objPoints[idxC - 1], newPolygon.ID, edgeCounter++));
			if (objPoints[idxB - 1].y != objPoints[idxC - 1].y)
				ET.push_back(edge(objPoints[idxB - 1], objPoints[idxC - 1], newPolygon.ID, edgeCounter++));
		}
		else {
			continue;
		}
	}
	// 计算framebuffer的总体大小
	int width, height;
	width = MAX_WIDTH;
	height = MAX_HEIGHT;
	framebuffer = new int*[height];
	for (int i = 0; i < height; i++) {
		framebuffer[i] = new int[width];
		for (int j = 0; j < width; j++) {
			framebuffer[i][j] = 255;
		}
	}

	double minX = 1000000, maxX = 0;
	double minY = 1000000, maxY = 0;
	double minZ = 1000000, maxZ = 0;
	for (vector<point>::iterator it = objPoints.begin(); it != objPoints.end(); it++) {
		if (it->x < minX)
			minX = it->x;
		if (it->x > maxX)
			maxX = it->x;
		if (it->y < minY)
			minY = it->y;
		if (it->y > maxY)
			maxY = it->y;
		if (it->z < minZ)
			minZ = it->z;
		if (it->z > maxZ)
			maxZ = it->z;
	}
	cout << minX << " " << maxX << " " <<  endl;
	cout << minY << " " << maxY << " " << endl;
	cout << minZ << " " << maxZ << " " << endl;

	cout << objPoints.size() << endl;
	cout << PT.size() << endl;
	cout << ET.size() << endl;
}


bool comparePoint(const xPoint &a, const xPoint &b)
{
	if ((int)a.x == (int)b.x) {
		return activeEdgeTable[a.edgeID].polygonID < activeEdgeTable[b.edgeID].polygonID;
	}
	return (int)a.x < (int)b.x; // 升序排序
}

bool compareEdgeByYmax(const edge &a, const edge &b)
{
	return a.yMax > b.yMax; //yMax按照降序排序，扫描线从上到下进行
}

void findMaxAndMinY(int &scanLineMaxY, int &scanLineMinY)
{
	scanLineMaxY = MAX_HEIGHT;
	scanLineMinY = MIN_HEIGHT;
}

void scanLine()
{	

	// 对ET按照上顶点y坐标进行排序(由大到小)
	sort(edgeTable.begin(), edgeTable.end(), compareEdgeByYmax);
	vector<edge>::iterator itET = edgeTable.begin();

	// DEBUG
	//for (vector<edge>::iterator it = edgeTable.begin(); it != edgeTable.end(); it++) {
	//	it->print();
	//}

	int scanLineMaxY, scanLineMinY;
	findMaxAndMinY(scanLineMaxY, scanLineMinY);

	for (int scanLineY = scanLineMaxY - 1; scanLineY >= scanLineMinY; scanLineY--) {
		// DEBUG
		cout << scanLineY << " " << activeEdgeTable.size() << " " << crossPoints.size() << " "<< endl;

		// 求扫描线和AET中所有边的交点
		// 如果交点不是线段的端点，则直接使用上一个端点的dx求出该端点的x值
		for (vector<xPoint>::iterator it = crossPoints.begin(); it != crossPoints.end(); it++) {
			if (activeEdgeTable[it->edgeID].counter > 0) {
				it->x += activeEdgeTable[it->edgeID].dx;	//增量求交
			}
			else {
				activeEdgeTable.erase(it->edgeID); //将counter=0的边从AET中删除
				it = crossPoints.erase(it); //返回下一个有效的元素
				it--;
			}
		}

		// 将扫描线新触及的边加入AET，并且将交点加入
		while (itET != edgeTable.end() && itET->yMax == (double)scanLineY) {

			activeEdgeTable[itET->ID] = *itET;
			xPoint newPoint;
			newPoint.x = itET->xMax;
			newPoint.edgeID = itET->ID;
			crossPoints.push_back(newPoint);
			itET++;
		}

		// 如果此时AEP是空，则直接跳过该循环
		if (activeEdgeTable.size() == 0) {
			continue;
		}

		// DEBUG
		//for (map<int, edge>::iterator it = activeEdgeTable.begin(); it != activeEdgeTable.end(); it++) {
		//	it->second.print();
		//}


		// 对交点排序
		sort(crossPoints.begin(), crossPoints.end(), comparePoint);



		// 开始处理每个线段
		for (vector<xPoint>::iterator itDeal = crossPoints.begin(); itDeal != crossPoints.end() - 1; itDeal++) {
			// 设置多边形flag
			if ((int)itDeal->x == (int)(itDeal + 1)->x
					&& activeEdgeTable[itDeal->edgeID].polygonID == activeEdgeTable[(itDeal + 1)->edgeID].polygonID
					&& ((int)activeEdgeTable[itDeal->edgeID].yMax - scanLineY) + ((int)activeEdgeTable[(itDeal + 1)->edgeID].yMax - scanLineY) > 0) {
				polygonTable[activeEdgeTable[itDeal->edgeID].polygonID].flag = !polygonTable[activeEdgeTable[itDeal->edgeID].polygonID].flag;
			}

			polygonTable[activeEdgeTable[itDeal->edgeID].polygonID].flag = !polygonTable[activeEdgeTable[itDeal->edgeID].polygonID].flag;



			// 取中点
			double middlePointX = (itDeal->x + (itDeal + 1)->x) / 2;

			double minZ = BG_Z_VALUE;
			int fillColor = BG_COLOR;
			if (fabs(itDeal->x - (itDeal + 1)->x) < 350) {
				// 找到所有中点所在的平面，并且计算他们的z值，记录最大的z值和其所在的平面
				for (vector<polygon>::iterator itPoly = polygonTable.begin(); itPoly != polygonTable.end(); itPoly++) {
					if (itPoly->flag) {
						if (itPoly->calcZ(middlePointX, (double)scanLineY) < minZ) {
							minZ = itPoly->calcZ(middlePointX, (double)scanLineY);
							fillColor = itPoly->color;
							//fillColor = (int)((it->calcZ(middlePointX, scanLineY) / zRange) * 255);
						}
					}
				}
			}

			
			// 确定了所在平面，将该平面的颜色填充至framebuffer
			for (int i = (int)itDeal->x; i <= (int)(itDeal + 1)->x; i++) {
				//framebuffer[scanLineY][i] = polygonTable[minPolygonID].color;
				framebuffer[scanLineY][i] = fillColor;
			}
		}

		//if (scanLineY == 3475) {
		//	cout << framebuffer[scanLineY][5900] << endl;
		//	cout << framebuffer[3500][5900] << endl;
		//	for (vector<xPoint>::iterator it = crossPoints.begin(); it != crossPoints.end(); it++) {
		//		it->print();
		//	}
		//}


		//重置flag
		//polygonTable[activeEdgeTable[(crossPoints.end() - 1)->edgeID].polygonID].flag = !polygonTable[activeEdgeTable[(crossPoints.end() - 1)->edgeID].polygonID].flag;
		for (vector<polygon>::iterator it = polygonTable.begin(); it != polygonTable.end(); it++) {
			it->flag = false;
		}

		// 将AET中每条边的counter-=1
		for (map<int, edge>::iterator it = activeEdgeTable.begin(); it != activeEdgeTable.end(); it++) {
			it->second.counter--;
			//if (it->second.counter == 0) {
			//	it = activeEdgeTable.erase(it);
			//}
		}
	}

	cv::Mat mat(MAX_HEIGHT, MAX_WIDTH, CV_8UC1);
	for (int i = MIN_HEIGHT; i < MAX_HEIGHT; i++) {
		for (int j = MIN_WIDTH; j < MAX_WIDTH; j++) {
			mat.at<unsigned char>(i, j) = (unsigned char)framebuffer[MAX_HEIGHT - i - 1][j];
		}
	}
	cv::imwrite("result.jpg", mat);
}

int main()
{
	buildPolygonTableAndEdgeTable1(polygonTable, edgeTable);
	scanLine();
	cout << "hello world" << endl;
	system("pause");
}