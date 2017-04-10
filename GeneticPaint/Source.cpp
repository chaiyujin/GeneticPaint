#include "triangle.h"
#include "genetic.h"
using namespace cv;
using namespace std;

int main() {

	Mat img = imread("small.jpg");

	Triangles trs0, trs1;
	for (int i = 0; i < 5; ++i) {
		trs0.push_back(random_triangle(img.cols, img.rows));
		trs1.push_back(random_triangle(img.cols, img.rows));
	}
	Chromosome c0(trs0);
	Chromosome c1(trs1);
	auto canvas0 = c0.render(img.cols, img.rows);
	auto canvas1 = c1.render(img.cols, img.rows);
	
	//c0.cross_with(c1, 1, false);
	c0.mutate(4, false);
	
	auto canvas2 = c0.render(img.cols, img.rows);

	imshow("can0", canvas0);
	imshow("can1", canvas1);
	imshow("can2", canvas2);
	waitKey();

	return 0;
}