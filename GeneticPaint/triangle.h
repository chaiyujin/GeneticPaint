#pragma once

#include "utils/yuki.h"
#include <opencv2/opencv.hpp>
#include <vector>
typedef unsigned char byte;

// gene struct for triangle mesh
struct Triangle {
	// tree vertex
	byte v[6];
	// color
	byte c[3];
	// alpha
	byte alpha;
};

typedef std::vector<Triangle> Triangles;

Triangle make_triangle(
	int x0, int y0,
	int x1, int y1,
	int x2, int y2,
	int r, int g, int b,
	float alpha);
Triangle random_triangle(int width, int height);

void print(Triangle &tr);
void draw_line(int x1, int y1, int x2, int y2, std::vector<std::pair<int, int>> &range);
void render_triangle(cv::Mat canvas, const Triangle &triangle);
inline void render_triangles(cv::Mat canvas, const Triangles &triangles) {
	for (size_t i = 0; i < triangles.size(); ++i) {
		render_triangle(canvas, triangles[i]);
	}
}

void allocate_and_copy(byte *&mem, const Triangles &triangle);
void copy_from_memory(Triangles &triangle, const byte *mem);

double difference_between(cv::Mat img0, cv::Mat img1);

