#include "bmp.h"		//	Simple .bmp library
#include<iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#define Baseline 30.0
#define Focal_Length 100
#define Image_Width 35.0
#define Image_Height 35.0
#define Resolution_Row 512
#define Resolution_Col 512
#define View_Grid_Row 9
#define View_Grid_Col 9

struct Point3d
{
	double x;
	double y;
	double z;
	Point3d(double x_, double y_, double z_) :x(x_), y(y_), z(z_) {}
};

struct Point2d
{
	double x;
	double y;
	Point2d(double x_, double y_) :x(x_), y(y_) {}
};


int main(int argc, char** argv)
{
	if(argc < 5 || argc > 6)
	{
		cout << "Arguments prompt: viewSynthesis.exe <LF_dir> <X Y Z> OR: viewSynthesis.exe <LF_dir> <X Y Z> <focal_length>" << endl;
		return 0;
	}
	string LFDir = argv[1];
	double Vx = stod(argv[2]), Vy = stod(argv[3]), Vz = stod(argv[4]);//position of the viewpoint
	double targetFocalLen = 100;//target focal length
	if(argc == 6)
	{
		targetFocalLen = stod(argv[5]);
	}
	

	vector<Bitmap> viewImageList;
	//! loading light field views
	for (int i = 0; i < View_Grid_Col*View_Grid_Row; i++)
	{
		char name[128];
		sprintf(name, "/cam%03d.bmp", i+1);
		string filePath = LFDir + name;
		Bitmap view_i(filePath.c_str());
		viewImageList.push_back(view_i);
	}

	Bitmap targetView(Resolution_Col, Resolution_Row);
	cout << "Synthesizing image from viewpoint: (" << Vx << "," << Vy << "," << Vz << ") with focal length: " << targetFocalLen << endl;
	//! resample pixels of the target view one by one
	for (int r = 0; r < Resolution_Row; r++)
	{
		for (int c = 0; c < Resolution_Col; c++)
		{
			Point3d rayRGB(0, 0, 0);
			//! resample the pixel value of this ray: TODO
            
            if(targetFocalLen == 100)//no need to bilinear interpolate pixels at one viewpoint
            {
                int x, y;
                double a, b;
                
                double X, Y;
                X = Vx + (Vz*(70*c-17885))/102400.0;
                Y = Vy + (Vz*(70*r-17885))/102400.0;
                              
                x = (int)(X + 120)/Baseline;
                y = (int)(120 - Y)/Baseline;

                a = (Vx + 120 - (x * Baseline))/(double)Baseline;
                b = (120 - Vy - (y * Baseline))/(double)Baseline;
                
                unsigned char red1, green1, blue1;
                unsigned char red2, green2, blue2;
                unsigned char red3, green3, blue3;
                unsigned char red4, green4, blue4;
                
                viewImageList[9 * y + x].getColor(c, r, red1, green1, blue1);
                viewImageList[9 * y + (x+1)].getColor(c, r, red2, green2, blue2);
                viewImageList[9 * (y+1) + x].getColor(c, r, red3, green3, blue3);
                viewImageList[9 * (y+1) + (x+1)].getColor(c, r, red4, green4, blue4);
                
                rayRGB.x = (double)((1-b)*((1-a)*red1+a*red2)+b*((1-a)*red3+a*red4));
                rayRGB.y = (double)((1-b)*((1-a)*green1+a*green2)+b*((1-a)*green3+a*green4));
                rayRGB.z = (double)((1-b)*((1-a)*blue1+a*blue2)+b*((1-a)*blue3+a*blue4));
                
            }
            else
            {
                
            }
            
			//! record the resampled pixel value
			targetView.setColor(c, r, (unsigned char) rayRGB.x, (unsigned char) rayRGB.y, (unsigned char) rayRGB.z);
		}
	}
	string savePath = "newView.bmp";
	targetView.save(savePath.c_str());
	cout << "Result saved!" << endl;
	return 0;
}
