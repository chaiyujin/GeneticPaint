#include "triangle.h"
#include "genetic.h"
using namespace cv;
using namespace std;

void test0();

int main() {

	Yuki::Directory::mkdir("result");
	Group::init_target("small.jpg");
	Group group = Group::first_generation(100, 5);
	int iter = 0;
	while (1) {
		group = group.evolve(0.6, 0.01);
		int index = group.best_chromosome_index();
		int sim = (int)group.similarity_at(index);
		LOG("Best at %d %f\n", index, sim);
		Mat img = group.render(index);
		if (sim > 80) {
			char buf[512];
			sprintf(buf, "result/Iter%d_Sim%d.jpg", iter, sim);
			imwrite(buf, img);
		}
		if (sim > 95) break;
		++iter;
	}

	system("pause");
	return 0;
}

void test0() {
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
}