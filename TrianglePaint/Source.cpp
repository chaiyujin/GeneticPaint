#include "triangle.h"
using namespace cv;
using namespace std;
using namespace Yuki;

void test();

int main() {
	Mat image = imread("chrome.jpg");
	Triangles triangles;
	int iter = 0;
	double error = triangles.difference_with(image);
	while (1) {
		Triangles next(triangles);
		next.mutate();
		double next_err = next.difference_with(image);
		if (next_err < error) {
			triangles = next;
			error = next_err;
		}
		if (iter % 1000 == 0) {
			auto img = triangles.render();
			imshow("image", img);
			waitKey(1);
		}
		LOG("Iter %d, Triangles %d, Err %f\r", iter++, triangles.size(), error);
	}

	system("pause");
	return 0;
}

void test() {
	Mat image = imread("chrome.jpg");
	Triangles trs;
	for (int i = 0; i < 10; ++i) {
		auto tr = random_triangle(image.cols, image.rows);
		trs.push_back(tr);
	}
	FOR(s, 10) {
		trs.move();
		LOG("%d %d\r", s, trs.size());

		auto empty = trs.render();
		if (1) {
			imshow("canvas", empty);
			waitKey(0);
		}
	}
}