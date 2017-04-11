#include "triangle.h"
#include "const.h"
#include "utils/yuki.h"
using namespace cv;
using namespace std;
using namespace Yuki;

void print(Triangle &tr) {
	for (int i = 0; i < 6; i += 2) {
		printf("Point: %d %d\t", tr.v[0], tr.v[1]);
	}
	printf("\nColor ");
	for (int i = 0; i < 3; ++i) {
		printf("%d ", tr.color[i]);
	}
	printf("%f\n", tr.color.a);
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

	for (int i = 0; i < 3; ++i) {
		auto x1 = triangle.v[i].x;
		auto y1 = triangle.v[i].y;
		int j = i + 1; if (j >= 3) j %= 3;
		auto x2 = triangle.v[j].x;
		auto y2 = triangle.v[j].y;
		draw_line(x1, y1, x2, y2, range);
	}

	float alpha = triangle.color.a / 255.f;

//#pragma omp parallel for schedule(dynamic)
	for (int r = 0; r < canvas.rows; ++r) {
		byte *data = canvas.ptr<byte>(r);
		min_max = range[r];
		for (int c = min_max.first; c <= min_max.second; ++c) {
			for (int k = 0; k < 3; ++k) {
				data[c * 3 + k] = (byte)(data[c * 3 + k] * (1 - alpha) + triangle.color[2 - k] * alpha);
			}
		}
	}
}

/*--------------Vertex------------------------------*/
Vertex random_vertex(int width, int height) {
	Vertex ret;
	ret.x = Tools::random_int(0, width);
	ret.y = Tools::random_int(0, height);
	return ret;
}

void Vertex::mutate() {
	if (Tools::will_mutate(Tools::Vertex_Max_Move_Rate)) {
		x = Tools::random_int(0, Tools::Max_Width);
		y = Tools::random_int(0, Tools::Max_Height);
	}

	if (Tools::will_mutate(Tools::Vertex_Mid_Move_Rate)) {
		x += Tools::random_int(-Tools::Vertex_Mid_Move_Range, Tools::Vertex_Mid_Move_Range);
		y += Tools::random_int(-Tools::Vertex_Mid_Move_Range, Tools::Vertex_Mid_Move_Range);
		x = Tools::clamp(x, 0, Tools::Max_Width - 1);
		y = Tools::clamp(y, 0, Tools::Max_Height - 1);
	}

	if (Tools::will_mutate(Tools::Vertex_Min_Move_Rate)) {
		x += Tools::random_int(-Tools::Vertex_Min_Move_Range, Tools::Vertex_Min_Move_Range);
		y += Tools::random_int(-Tools::Vertex_Min_Move_Range, Tools::Vertex_Min_Move_Range);
		x = Tools::clamp(x, 0, Tools::Max_Width - 1);
		y = Tools::clamp(y, 0, Tools::Max_Height - 1);
	}
}

/*--------------Color-------------------------------*/
Color random_color() {
	Color c;
	c.r = (byte)Tools::random_int(0, 256);
	c.g = (byte)Tools::random_int(0, 256);
	c.b = (byte)Tools::random_int(0, 256);
	c.a = (byte)Tools::random_int(0, 256);
	return c;
}

void Color::mutate() {
	for (int i = 0; i < 4; ++i) {
		if (Tools::will_mutate(Tools::Color_Mutate_Rate)) {
			(*this)[i] = (byte)Tools::random_int(Tools::Color_Min, Tools::Color_Max);
		}
	}
}

/*----------------Triangle-------------------------*/
Triangle random_triangle(int width, int height) {
	Triangle ret;
	for (int i = 0; i < 3; ++i) {
		ret.v[i] = random_vertex(width, height);
	}
	ret.color = random_color();
	return ret;
}

void Triangle::mutate(bool mutate_vertex, bool mutate_color) {
	if (mutate_vertex) {
		FOR(i, 3) {
			v[i].mutate();
		}
	}
	if (mutate_color) {
		color.mutate();
	}
}

/*--------------Triangles---------------------------*/
void Triangles::add() {
	if (list.size() == 0 || Tools::will_mutate(Tools::Add_New_Triangle_Rate)) {
		list.push_back(random_triangle(Tools::Max_Width, Tools::Max_Height));
	}
	else {
		int pos = Tools::random_int(0, list.size());
		Triangle new_one = list[pos];
		new_one.mutate(true, false);
		list.push_back(new_one);
	}
}

void Triangles::remove() {
	if (list.size() == 0) return;
	int pos = Tools::random_int(0, list.size());
	for (int i = pos; i + 1 < list.size(); ++i) {
		list[i] = list[i + 1];
	}
	list.pop_back();
}

void Triangles::move() {
	if (list.size() < 3) return;
	int l = 0, r = 0;
	while (l == r) {
		l = Tools::random_int(0, list.size());
		r = Tools::random_int(0, list.size());
		if (l > r) { int t = l; l = r; r = t; }
	}
	Triangle tmp = list[l];
	for (int i = l; i + 1 <= r; ++i) {
		list[i] = list[i + 1];
	}
	list[r] = tmp;
}

void Triangles::mutate() {
	if (Tools::will_mutate(Tools::Add_Triangle_Rate)) {
		add();
	}
	
	if (Tools::will_mutate(Tools::Remove_Triangle_Rate)) {
		remove();
	}

	if (Tools::will_mutate(Tools::Move_Triangle_Rate)) {
		move();
	}

	for (int i = 0; i < list.size(); ++i)
		list[i].mutate();
}

Mat Triangles::render() {
	Mat img(Tools::Max_Height, Tools::Max_Width, CV_8UC3, Scalar(0, 0, 0));
	render_triangles(img, *this);
	return img;
}

void allocate_and_copy(byte *&mem, const vector<Triangle> &triangles) {
	size_t length = sizeof(Triangle) * triangles.size();
	mem = new byte[length];
	memcpy(mem, triangles.data(), length);
}

void copy_from_memory(vector<Triangle> &triangles, const byte *mem) {
	size_t length = sizeof(Triangle) * triangles.size();
	memcpy(triangles.data(), mem, length);
}

double difference_between(cv::Mat img0, cv::Mat img1) {
	CHECK(img0.cols == img1.cols && img0.rows == img1.rows);
	double error = 0.0;

//#pragma omp parallel for schedule(dynamic)
	FOR(r, img0.rows) {
		byte *data0 = img0.ptr<byte>(r);
		byte *data1 = img1.ptr<byte>(r);
		FOR(c, img0.cols) {
			FOR(k, 3) {
				const int index = c * 3 + k;
//#pragma omp critical
				error += abs(data0[index] - data1[index]);
			}
		}
	}
	return error;
}

double similarity_between(cv::Mat img0, cv::Mat img1) {
	CHECK(img0.cols == img1.cols && img0.rows == img1.rows);
	double sim = 0.0;
	FOR(r, img0.rows) {
		byte *data0 = img0.ptr<byte>(r);
		byte *data1 = img1.ptr<byte>(r);
		FOR(c, img0.cols) {
			/*bool white0 = true;
			bool white1 = true;
			FOR(k, 3) {
				const int index = c * 3 + k;
				if (data0[index] < 220) white0 = false;
				if (data1[index] < 220) white1 = false;
			}

			if (white0 && white1) continue;*/
			FOR(k, 3) {
				const int index = c * 3 + k;
				sim += 256 - abs(data0[index] - data1[index]);
			}
		}
	}
	sim /= 3;
	return sim;
}