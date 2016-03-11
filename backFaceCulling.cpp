/*
Assignment 3: Back face culling and projection.
zViewer should be positive and greater than the maximum z coordinate in the objfile 
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <climits>
#include <cmath>

using namespace std;

const double PI = atan(1.0)*4;

struct Point3D
{
	double x,y,z;
};

struct Point2D
{
	double x,y;
};

struct Normal
{
	double a,b,c;
};

struct LineOfSight
{
	double a,b,c;
};

void printVertices(vector<Point3D> vertices)
{
	for(vector<Point3D>::iterator it = vertices.begin() + 1;it!=vertices.end();++it)
  		cout<<it->x<<" "<<it->y<<" "<<it->z<<endl;
}

void printFaces(vector<vector<int> > faces)
{
	for(vector<vector<int> >::iterator it = faces.begin();it!=faces.end();++it)
  	{
  		for(vector<int>::iterator it2 = it->begin();it2!=it->end();++it2)
  			cout<<*it2<<" ";
  		cout<<endl;
  	}
}

int stringToInt(string s)
{
	return atoi(s.c_str());
}

double stringToDouble(string s)
{
	return atof(s.c_str());
}

vector<string> splitLine(string line)
{
	vector<string> ret;
	stringstream ss(line);// Turn the string into a stream.
	string tok;
	  
	while(ss >> tok)
	{
	  ret.push_back(tok);
	}
	return ret;
}

void readObjfile(string filename, vector<Point3D> &vertices, vector<vector<int> > &faces)
{
	/*
	Extract vertices and faces from objfile.
	Faces can be polygons.
	*/
	Point3D p = {0,0,0};//dummy point for proper indexing
	vertices.push_back(p);

	string line;
  	ifstream objfile(filename.c_str());
  	if(objfile.is_open())
  	{
    	while(getline(objfile,line))
    	{
    	  	// cout << line << '\n';
    		if(line.empty())//blank line
    			continue;
    		if(line[0]=='#')//comment line
    			continue;

    		vector<string> tokens = splitLine(line);
    		if(line[0]=='v')//vertex
    		{
    			if(tokens.size()!=4)
    			{
    				cout<<"Invalid obj file"<<endl;
    				exit(0);
    			}
    			Point3D p = {stringToDouble(tokens[1]), stringToDouble(tokens[2]), stringToDouble(tokens[3])};
    			vertices.push_back(p);
    		}
    		else//face
    		{
    			if(tokens.size()<4)
    			{
    				cout<<"Invalid obj file"<<endl;
    				exit(0);
    			}
    			vector<int> face;
    			vector<string>::iterator it = tokens.begin() + 1;//ignore first token
    			for(;it!=tokens.end();++it)
    				face.push_back(stringToInt(*it));
    			faces.push_back(face);
    		}
    	}
    	objfile.close();
  	}
  	else
  	{
  		cout << "Unable to open objfile"<<endl;
  		exit(0);
  	}

  	// printVertices(vertices);
  	// printFaces(faces);
}

double shiftPointsInsideObjectPlane(vector<Point3D> &vertices)
{
	/*
	Find max z value of vertices.
	If any point has positive z, then push it inside.
	return the shift value so that we can shift viewer also.
	*/
	double maxZ = 0;
	vector<Point3D>::iterator it = vertices.begin() + 1;
	for(;it!=vertices.end();++it)
		maxZ = max(maxZ, it->z);

	if(maxZ==0)return 0;

	maxZ+=1;//make all z negative
	for(int i=1;i<vertices.size();++i)
		vertices[i].z -= maxZ;
	return maxZ;
}

Point2D projectPoint(Point3D p, Point3D viewer)
{
	// cout<<p.x<<" "<<p.y<<" "<<p.z<<endl;
	// cout<<viewer.x<<" "<<viewer.y<<" "<<viewer.z<<endl;
	double xPrime, yPrime;
	xPrime = (-p.z * viewer.x + p.x * viewer.z)/(-p.z + viewer.z);
	yPrime = (-p.z * viewer.y + p.y * viewer.z)/(-p.z + viewer.z);
	Point2D projectedPoint = {xPrime, yPrime};
	// cout<<xPrime<<" "<<yPrime<<endl;
	return projectedPoint;
}

vector<Point2D> projectVertices(vector<Point3D> vertices, Point3D viewer)
{
	vector<Point2D> projectedVertices;
	Point2D p = {0,0};//dummy point
	projectedVertices.push_back(p);
	vector<Point3D>::iterator it = vertices.begin() + 1;
	for(;it!=vertices.end();++it)
		projectedVertices.push_back(projectPoint(*it, viewer));

	return projectedVertices;
}

Normal getNormal(vector<Point3D> vertices, vector<int> face)
{
	//12 cross 23
	int v1 = face[0], v2 = face[1], v3 = face[2];
	// cout<<v1<<v2<<v3<<endl;
	double x21,y21,z21;
	x21 = vertices[v2].x - vertices[v1].x;
	y21 = vertices[v2].y - vertices[v1].y;
	z21 = vertices[v2].z - vertices[v1].z;

	double x32,y32,z32;
	x32 = vertices[v3].x - vertices[v2].x;
	y32 = vertices[v3].y - vertices[v2].y;
	z32 = vertices[v3].z - vertices[v2].z;

	//Normal to this face
	double a = y21*z32 - y32*z21;
	double b = z21*x32 - z32*x21;
	double c = x21*y32 - x32*y21;

	Normal n = {a,b,c};
	// cout<<a<<" "<<b<<" "<<c<<endl;
	return n;
}

LineOfSight getLineOfSight(Point3D p, Point3D viewer)
{
	double a = p.x - viewer.x;
	double b = p.y - viewer.y;
	double c = p.z - viewer.z;
	LineOfSight los = {a,b,c};
	// cout<<a<<" "<<b<<" "<<c<<endl;
	return los;
}

bool isVisible(Normal n, LineOfSight los)
{
	return (n.a*los.a + n.b*los.b + n.c*los.c) < 0;
}

double findCentroidZ(vector<int> face, vector<Point3D> vertices)
{
	double z = 0.0;
	vector<int>::iterator it;
	for(it=face.begin();it!=face.end();++it)
	{
		int v = *it;
		z += vertices[v].z;
	}
	z /= face.size();
	// cout<<z<<endl;//should be negative
	return z;
}

void findExtremesOfProjectedVertices(vector<Point2D> projectedVertices, double &xMin, double &yMin, double &xMax, double &yMax)
{
	xMin = INT_MAX;
	yMin = INT_MAX;
	xMax = INT_MIN;
	yMax = INT_MIN;
	vector<Point2D>::iterator it;
	for(it = projectedVertices.begin()+1;it!=projectedVertices.end();++it)
	{
		xMin = min(xMin, it->x);
		yMin = min(yMin, it->y);
		xMax = max(xMax, it->x);
		yMax = max(yMax, it->y);
	}
	// cout<<xMin<<" "<<yMin<<" "<<xMax<<" "<<yMax<<endl;
}

void scaleProjectedVertices(vector<Point2D> &projectedVertices, double scale)
{
	for(int i=1;i<projectedVertices.size();++i)
	{
		projectedVertices[i].x *= scale;
		projectedVertices[i].y *= scale;
	}
}

void translateProjectedVertices(vector<Point2D> &projectedVertices, double tx, double ty)
{
	for(int i=1;i<projectedVertices.size();++i)
	{
		projectedVertices[i].x -= tx;
		projectedVertices[i].y -= ty;
		// cout<<projectedVertices[i].x<<" "<<projectedVertices[i].y<<endl;
	}
}

Point3D findCenterOfObject(vector<Point3D> vertices)
{
	vector<Point3D>::iterator it;
	double x=0,y=0,z=0;
	for(it=vertices.begin() + 1;it!=vertices.end();++it)
	{
		x += it->x;
		y += it->y;
		z += it->z;
	}
	x /= vertices.size() - 1;
	y /= vertices.size() - 1;
	z /= vertices.size() - 1;

	Point3D p = {x,y,z};
	// cout<<x<<" "<<y<<" "<<z<<endl;
	return p;
}

void rotateObject(vector<Point3D> &vertices, double thetaX, double thetaY, double thetaZ)
{
	Point3D center = findCenterOfObject(vertices);
	double a,b;
	for(int i=1;i<vertices.size();++i)
	{
		double x = vertices[i].x;
		double y = vertices[i].y;
		double z = vertices[i].z;

		x -= center.x;
		y -= center.y;
		z -= center.z;

		//rotate about x
		a = y*cos(thetaX) - z*sin(thetaX);
		b = y*sin(thetaX) + z*cos(thetaX);
		y=a;z=b;

		//rotate about y
		a = z*cos(thetaY) - x*sin(thetaY);
		b = z*sin(thetaY) + x*cos(thetaY);
		z=a;x=b;

		//rotate about z
		a = x*cos(thetaZ) - y*sin(thetaZ);
		b = x*sin(thetaZ) + y*cos(thetaZ);
		x=a;y=b;

		x += center.x;
		y += center.y;
		z += center.z;

		vertices[i].x = x;
		vertices[i].y = y;
		vertices[i].z = z;
	}
}

string filename;
int height, width;
double xv,yv,zv;
double thetaX=0, thetaY=0, thetaZ=0;	
void input()
{
	//Input
	cout<<"Enter objfile name:";cin>>filename;

	cout<<"Enter height of viewport:";cin>>height;
	cout<<"Enter width of viewport:";cin>>width;

	cout<<"Enter x coordinate of camera:";cin>>xv;
	cout<<"Enter y coordinate of camera:";cin>>yv;
	cout<<"Enter z coordinate of camera:";cin>>zv;

	cout<<"Enter angle of rotation about x-axis(enter 0 if not needed):";cin>>thetaX;
	cout<<"Enter angle of rotation about y-axis(enter 0 if not needed):";cin>>thetaY;
	cout<<"Enter angle of rotation about z-axis(enter 0 if not needed):";cin>>thetaZ;
	cout<<endl;
}

vector<pair<double, int> > findVisibleFaces(vector<vector<int> > faces, vector<Point3D> vertices, Point3D viewer)
{
	//Find visible faces
	vector<vector<int> >::iterator it;
	int faceIndex=0;
	vector<pair<double, int> > visibleFaces;
	for(it=faces.begin();it!=faces.end();++it)
	{
		Normal n = getNormal(vertices, *it);
		LineOfSight los = getLineOfSight(vertices[(*it)[0]], viewer);
		if(isVisible(n, los))//Face is visible
		{
			double z = findCentroidZ(*it, vertices);
			// cout<<(*it)[0]<<" "<<(*it)[1]<<" "<<(*it)[2]<<" "<<z<<endl;
			visibleFaces.push_back(make_pair(z, faceIndex));
		}
		faceIndex++;
	}
	//Sort by z values of faces to apply painters algorithm
	sort(visibleFaces.begin(), visibleFaces.end());
	return visibleFaces;
}

void constructSVG(vector<pair<double, int> > visibleFaces, vector<vector<int> > faces, vector<Point2D> projectedVertices)
{
	//Construct the svg file with the visible faces
	string svgString = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" ";
	char numstr[21]; // enough to hold all numbers up to 64-bits
	sprintf(numstr, "%d", width);
	svgString += "width=\"" + string(numstr) + string("\" ");
	sprintf(numstr, "%d", height);
	svgString += "height=\"" + string(numstr) + string("\" ");
	svgString += ">\n";
	for (int i = 0; i < visibleFaces.size(); ++i)
	{
		pair<double, int> p = visibleFaces[i];
		int faceIndex = p.second;
		svgString += "<polygon points=\"";
		for(vector<int>::iterator it = faces[faceIndex].begin();it!=faces[faceIndex].end();++it)
		{
			int v = *it;
			// cout<<v<<" ";
			float x = float(projectedVertices[v].x);
			float y = float(projectedVertices[v].y);
			// cout<<x<<" "<<y<<endl;
			sprintf(numstr, "%lf", x);
			svgString += string(" ");
			svgString += numstr;
			svgString += string(",");
			sprintf(numstr, "%lf", y);
			svgString += numstr;
			svgString += string(" ");
		}
		// cout<<endl;
		svgString += "\" style=\"fill:#de5700;fill-opacity:1;stroke:#000000;stroke-width:1;stroke-linejoin:round;stroke-opacity:1\" /> \n";
	}
	svgString += "</svg> ";

	// cout<<svgString<<endl;

	ofstream out("output.svg");
	out << svgString;
	out.close();
}

int main()
{
	input();

	//Convert degrees to radians
	thetaX*=PI;thetaX/=180.0;
	thetaY*=PI;thetaY/=180.0;
	thetaZ*=PI;thetaZ/=180.0;

	vector<Point3D> vertices;
	vector<vector<int> > faces;
	readObjfile(filename, vertices, faces);

	rotateObject(vertices, thetaX, thetaY, thetaZ);

	// cout<<vertices.size()<<" "<<faces.size()<<endl;

	//Send points inside the object plane if they have positive z value
	double shiftValue = shiftPointsInsideObjectPlane(vertices);
	// cout<<shiftValue<<endl;
	zv-=shiftValue;
	Point3D viewer = {xv,yv,zv};

	//Find projected vertices
	vector<Point2D> projectedVertices = projectVertices(vertices, viewer);

	//Find visible faces
	vector<pair<double, int> > visibleFaces = findVisibleFaces(faces, vertices, viewer);	

	//Scale the object according to the view port
	double xMin, yMin, xMax, yMax;
	findExtremesOfProjectedVertices(projectedVertices, xMin, yMin, xMax, yMax);
	double scale = min((width - 20)/(xMax - xMin), (height - 20)/(yMax - yMin));
	// cout<<scale<<endl;
	scaleProjectedVertices(projectedVertices, scale);


	//Translate the object according to the view port leaving 10 pixel gap on all sides
	findExtremesOfProjectedVertices(projectedVertices, xMin, yMin, xMax, yMax);
	double tx = xMin - 10;
	double ty = yMin - 10;
	// cout<<tx<<" "<<ty<<endl;
	translateProjectedVertices(projectedVertices, tx, ty);

	constructSVG(visibleFaces, faces, projectedVertices);	

  	return 0;
}