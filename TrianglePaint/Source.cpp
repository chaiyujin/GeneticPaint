#include "triangle.h"
#include "polygon.h"
using namespace cv;
using namespace std;
using namespace Yuki;

void test();
void test_poly();
void triangle_draw();
void polygon_draw();

int main() {

	polygon_draw();

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

void test_poly() {
	Polygon poly;

	FOR(i, 10) {
		Mat image = Mat::zeros(Tools::Max_Height, Tools::Max_Width, CV_8UC3);
		poly.add();
		poly.render_on(image);
		imshow("img", image);
		waitKey();
	}
}

void triangle_draw() {
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
			if (iter > 200000 && iter % 10000 == 0) {
				imwrite("result.jpg", img);
			}
		}
		LOG("Iter %d, Triangles %d, Err %f\r", iter++, triangles.size(), error);
	}
}

void polygon_draw() {
	Mat image = imread("chrome.jpg");
	Polygons polys;
	int iter = 0;
	double error = polys.difference_with(image);
	while (1) {
		Polygons next(polys);
		next.mutate();
		double next_err = next.difference_with(image);
		if (next_err < error) {
			polys = next;
			error = next_err;
		}
		if (iter % 1000 == 0) {
			auto img = polys.render();
			imshow("image", img);
			waitKey(1);
			if (iter > 200000 && iter % 10000 == 0) {
				imwrite("result.jpg", img);
			}
		}
		LOG("Iter %d, Polygons %d, Points %f, Err %f\r",
			iter++, polys.size(), 
			polys.size_of_vertices() / (float)polys.size(),
			error);
	}
}