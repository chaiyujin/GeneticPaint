#pragma once

#include "utils/yuki.h"
#include "triangle.h"
#include <vector>
#include <string>

class Chromosome;
typedef std::vector<Chromosome> Group;

class Chromosome {
private:
	size_t triangles;
	size_t length;
	unsigned char *gene;
	static cv::Mat target;

public:
	Chromosome(std::vector<Triangle> &triangles);
	~Chromosome();

	float p;

	float ability_to_adapt();
	cv::Mat render();
	
	static void  init(std::string target_image);
	static Group first_generation(int n_triangle, int n_chrom);
	static void  select(Group &population);
	static void  cross(Group &parents, float rate);
	static void  mutate(Group &population, float rate);
};

inline bool cmp(const Chromosome &chrom0, const Chromosome &chrom1) {
	return chrom0.p > chrom1.p;
}