#pragma once
#include "triangle.h"
#include <string>

class Group;

class Chromosome {
	friend class Group;
private:
	byte *data;
	int n_triangle;
	int size_in_byte;
	int size_in_bit;
public:
	Chromosome(const Triangles &triangles);

	cv::Mat render(int width, int height);
	void cross_with(Chromosome &chrom, int position, bool byte_mode = true);
	void mutate(int position, bool byte_mode = true);

	int size(bool byte_mode = true) {
		if (byte_mode) return size_in_byte;
		else return size_in_bit;
	}

	void destroy() {
		delete[] data;
	}
};

//class Group {
//private:
//	static cv::Mat target_image;
//	
//	std::vector<Chromosome> chroms;
//public:
//	static void init_target(std::string image_path);
//	static Group first_generation(int n_chromosome, int n_triangles);
//
//	Group select();
//	void cross(float rate);
//	void mutate(float rate);
//
//	void destroy() {
//		FOR(i, chroms.size()) {
//			chroms[i].destroy();
//		}
//		chroms.clear();
//	}
//};