#include "triangle.h"
#include "utils/yuki.h"
using namespace std;
using namespace Yuki;

void print(Triangle &tr) {
	for (int i = 0; i < 6; i += 2) {
		printf("Point: %d %d\t", tr.v[0], tr.v[1]);
	}
	printf("\nColor ");
	for (int i = 0; i < 3; ++i) {
		printf("%d ", tr.c[i]);
	}
	printf("%f\n", tr.alpha);
}

void draw_line(int x1, int y1, int x2, int y2, vector<pair<int, int>> &range) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int ux = ((dx > 0) << 1) - 1;	//x的增量方向，取或-1
	int uy = ((dy > 0) << 1) - 1;	//y的增量方向，取或-1
	int x = x1, y = y1, eps;		//eps为累加误差

	eps = 0; dx = abs(dx); dy = abs(dy);
	if (dx > dy) {
		for (x = x1; x != x2; x += ux) {
			if (x < range[y].first) range[y].first = x;
			if (x > range[y].second) range[y].second = x;
			
			eps += dy;
			if ((eps << 1) >= dx) {
				y += uy; eps -= dx;
			}
		}
	}
	else {
		for (y = y1; y != y2; y += uy) {
			if (x < range[y].first) range[y].first = x;
			if (x > range[y].second) range[y].second = x;

			eps += dx;
			if ((eps << 1) >= dy) {
				x += ux; eps -= dy;
			}
		}
	}
}

void render_triangle(cv::Mat canvas, const Triangle &triangle) {
	std::vector<std::pair<int, int>> range;
	std::pair<int, int> min_max;
	min_max.first = canvas.cols + 1;
	min_max.second = -1;

	for (int k = 0; k < canvas.rows; ++k)
		range.push_back(min_max);

	for (int i = 0; i < 6; i += 2) {
		auto x1 = triangle.v[i];
		auto y1 = triangle.v[i + 1];
		int j = i + 2; if (j >= 6) j %= 6;
		auto x2 = triangle.v[j];
		auto y2 = triangle.v[j + 1];
		draw_line(x1, y1, x2, y2, range);
	}

	float alpha = triangle.alpha / 255.f;
	for (size_t r = 0; r < canvas.rows; ++r) {
		byte *data = canvas.ptr<byte>(r);
		min_max = range[r];
		for (int c = min_max.first; c <= min_max.second; ++c) {
			for (int k = 0; k < 3; ++k) {
				data[c * 3 + k] = (byte)(data[c * 3 + k] * (1 - alpha) + triangle.c[2 - k] * alpha);
			}
		}
	}
}

Triangle make_triangle(int x0, int y0, int x1, int y1, int x2, int y2, int r, int g, int b, float alpha) {
	Triangle ret;
	ret.v[0] = (byte)x0;
	ret.v[1] = (byte)y0;
	ret.v[2] = (byte)x1;
	ret.v[3] = (byte)y1;
	ret.v[4] = (byte)x2;
	ret.v[5] = (byte)y2;
	ret.c[0] = (byte)r;
	ret.c[1] = (byte)g;
	ret.c[2] = (byte)b;
	ret.alpha = (byte)(alpha * 255.0);

	return ret;
}

Triangle random_triangle(int width, int height) {
	Random random;
	Triangle ret;
	ret.v[0] = (byte)random.random<int>(width);
	ret.v[1] = (byte)random.random<int>(height);
	ret.v[2] = (byte)random.random<int>(width);
	ret.v[3] = (byte)random.random<int>(height);
	ret.v[4] = (byte)random.random<int>(width);
	ret.v[5] = (byte)random.random<int>(height);

	ret.c[0] = (byte)random.random<int>(256);
	ret.c[1] = (byte)random.random<int>(256);
	ret.c[2] = (byte)random.random<int>(256);

	ret.alpha = (byte)random.random<int>(256);

	return ret;
}

void allocate_and_copy(byte *&mem, const Triangles &triangles) {
	size_t length = sizeof(Triangle) * triangles.size();
	mem = new byte[length];
	memcpy(mem, triangles.data(), length);
}

void copy_from_memory(Triangles &triangles, const byte *mem) {
	size_t length = sizeof(Triangle) * triangles.size();
	memcpy(triangles.data(), mem, length);
}

double difference_between(cv::Mat img0, cv::Mat img1) {
	CHECK(img0.cols == img1.cols && img0.rows == img1.rows);
	double error = 0.0;
	FOR(r, img0.rows) {
		byte *data0 = img0.ptr<byte>(r);
		byte *data1 = img1.ptr<byte>(r);
		FOR(c, img0.cols) {
			FOR(k, 3) {
				const int index = c * 3 + k;
				error += abs(data0[index] - data1[index]);
			}
		}
	}
	error /= 3 * img0.rows * img0.cols;
	return error;
}