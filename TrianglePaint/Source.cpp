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
	string input_file = "sample.jpg";
	string output_file = "result.jpg";
	double scale = 1;
	if (argc > 1) input_file = argv[1];
	if (argc > 2) output_file = argv[2];
	if (argc > 3) scale = atof(argv[3]);
	polygon_draw(input_file, output_file, scale);

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
		int old_w = image.cols;
		int old_h = image.rows;
		double scale = old_w / 256.0;
		if (old_h > old_w) scale = old_h / 256.0;
		// size of now
		Tools::Max_Width = old_w / scale;
		Tools::Max_Height = old_h / scale;
		Tools::Max_Width = Tools::clamp(Tools::Max_Width, 0, 256);
		Tools::Max_Height = Tools::clamp(Tools::Max_Height, 0, 256);
		scale = (double)old_w / (double)Tools::Max_Width;
		Tools::Render_Scale = scale * output_scale;

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
			auto img = polys.render(Tools::Render_Scale);
			imshow("image", img);
			waitKey(1);
			if (iter % 10000 == 0) {
				imwrite(output_file, img);
			}
		}
		LOG("Iter %d, Polygons %d, Points %f, Err %f\r",
			iter++, polys.size(), 
			polys.size_of_vertices() / (float)polys.size(),
			error);
	}
}

void group_draw() {
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