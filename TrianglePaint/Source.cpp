#include "triangle.h"
#include "polygon.h"
#include "genetic.h"
#include <string>
using namespace cv;
using namespace std;
using namespace Yuki;

// test
void test();
void test_poly();
void triangle_draw();
void group_draw();

// main loop
void polygon_draw(string input_file, string output_path, double scale);

int main(int argc, char **argv) {
	string input_file = "small.jpg";
	string output_file = "result.jpg";
	double scale = 1;
	if (argc > 1) input_file = argv[1];
	if (argc > 2) output_file = argv[2];
	if (argc > 3) scale = atof(argv[3]);
	polygon_draw(input_file, output_file, scale);

	//group_draw();

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
	Mat image = imread("small.jpg");
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

void polygon_draw(string input_file, string output_file, double output_scale) {
	Yuki::Directory::mkdir("result");
	Mat image = imread(input_file);
	{
		Tools::Max_Width = image.cols;
		Tools::Max_Height = image.rows;

		// scale origin image
		resize(image, image, Size(Tools::Max_Width, Tools::Max_Height));
		imshow("target", image);
		waitKey(1);
	}
	Polygons polys;
	int iter = 0;
	double error = polys.difference_with(image);
	while (1) {
		Polygons next(polys);
		next.mutate();
		double next_err = next.difference_with(image);
		if (next_err <= error) {
			polys = next;
			error = next_err;
		}
		if (iter % 1000 == 0) {
			auto img = polys.render();
			imshow("image", img);
			waitKey(1);
			if (iter % 10000 == 0) {
				char buf[512];
				sprintf(buf, "result/iter%d.jpg", iter);
				imwrite(buf, img);
			}
		}
		LOG("Iter %d, Polygons %d, Points %f, Err %f\r",
			iter++, polys.size(), 
			polys.size_of_vertices() / (float)polys.size(),
			error);
		// hack
		if (iter % 100000 == 0) {
			Tools::Add_Polygon_Rate /= 2;
			if (Tools::Add_Polygon_Rate < 5) Tools::Add_Polygon_Rate = 5;
		}
	}
}

void group_draw() {

	Polygons p0, p1;
	for (int i = 0; i < 5; ++i) {
		p0.add();
		p1.add();
	}
	for (int i = 0; i < 1000; ++i) {
		p0.mutate();
		p1.mutate();
	}

	{
		auto img0 = p0.render();
		auto img1 = p1.render();
		imshow("img0", img0);
		imshow("img1", img1);
		waitKey();
	}

	Chromosome<Polygons> ch0(p0);
	Chromosome<Polygons> ch1(p1);
	ch0.cross_with(ch1);

	{
		auto img0 = ch0.render();
		auto img1 = ch1.render();
		imshow("img0", img0);
		imshow("img1", img1);
		waitKey();
	}



	Mat image = imread("small.jpg");
	Group<Polygons> group(image);
	group.generate_chromosome(10);
	int iter = 0;
	while (1) {
		group = group.evolve(0.6);

		auto &best = group[0];
		auto &polys = best.shape;
		double error = best.error;
		if (iter % 100 == 0) {
			auto img = best.render();
			imshow("image", img);
			waitKey(1);
		}
		LOG("Iter %d, Polygons %d, Points %f, Err %f\r",
			iter++, polys.size(),
			polys.size_of_vertices() / (float)polys.size(),
			error);
	}
}