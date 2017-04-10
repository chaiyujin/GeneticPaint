#include "genetic.h"
using namespace cv;
using namespace std;
using namespace Yuki;

Chromosome::Chromosome(const Triangles &triangles) {
	allocate_and_copy(data, triangles);
	n_triangle = (int)triangles.size();
	size_in_byte = n_triangle * sizeof(Triangle);
	size_in_bit = size_in_byte * 8;
}

Mat Chromosome::render(int width, int height) {
	Triangles triangles(n_triangle);
	copy_from_memory(triangles, data);
	Mat result = Mat::zeros(height, width, CV_8UC3);
	render_triangles(result, triangles);
	return result;
}

void Chromosome::cross_with(Chromosome &chrom, int position, bool byte_mode) {
	if (byte_mode) {
		// cross in byte mode
		CHECK(position >= 0 && position < size_in_byte);
		for (int k = position; k < size_in_byte; ++k) {
			byte tmp = this->data[k];
			this->data[k] = chrom.data[k];
			chrom.data[k] = tmp;
		}
	}
	else {
		// cross in bit mode
		CHECK(position >= 0 && position < size_in_bit);
		if (position % 8 == 0) {
			cross_with(chrom, position / 8, true);
			return;
		}
		// byte after the positoin
		for (int k = position / 8 + 1; k < size_in_byte; ++k) {
			byte tmp = this->data[k];
			this->data[k] = chrom.data[k];
			chrom.data[k] = tmp;
		}
		// byte at the position
		int i = position / 8;
		int k = 8 - position % 8;
		byte mask = (1 << k) - 1;
		byte revs = ~mask;
		//LOG("%X %X\n", this->data[i], chrom.data[i]);
		byte this_l = this->data[i] & revs;
		byte that_l = chrom.data[i] & revs;
		byte this_r = this->data[i] & mask;
		byte that_r = chrom.data[i] & mask;
		this->data[i] = this_l | that_r;
		chrom.data[i] = that_l | this_r;
		//LOG("%X %X\n", this->data[i], chrom.data[i]);
	}
}

void Chromosome::mutate(int position, bool byte_mode) {
	if (byte_mode) {
		// mutate in byte mode
		CHECK(position >= 0 && position < size_in_byte);
		this->data[position] = ~this->data[position];
	}
	else {
		// mutate in bit mode
		CHECK(position >= 0 && position < size_in_bit);
		int i = position / 8;
		int k = 7 - position % 8;
		byte mask = (1 << k);
		byte revs = ~mask;
		//LOG("%X\n", this->data[i]);
		byte val = this->data[i];
		this->data[i] = (val & mask)
						? (val & revs)
						: (val | mask);
		//LOG("%X\n", this->data[i]);
	}
}


/*------------------------------------------------------------------------*/

Mat Group::target_image;

void Group::init_target(std::string image_path) {
	if (!Directory::exists(image_path)) {
		ERROR_EXIT("No such image %s\n", image_path.c_str());
	}
	target_image = imread(image_path);
	if (!(target_image.rows == 256 && target_image.cols == 256)) {
		ERROR_EXIT("Image should be 256x256\n");
	}
}

Group Group::first_generation(int n_chromosome, int n_triangles) {
	CHECK(n_chromosome > 0 && n_triangles > 0);
	Group popu;
	FOR(i, n_chromosome) {
		Triangles trs;
		FOR(j, n_triangles) {
			trs.push_back(random_triangle(target_image.cols, target_image.rows));
		}
		popu.chroms.emplace_back(trs);
	}
	return popu;
}

void Group::calc_similarity() {
	similarity.clear();
	FOR(i, chroms.size()) {
		auto image = chroms[i].render(target_image.cols, target_image.rows);
		double fit = 100.0 - difference_between(image, target_image);
		if (fit < 0) fit = 0;
		similarity.push_back(fit);
	}
}

int Group::best_chromosome_index() {
	if (similarity.size() == 0)
		calc_similarity();
	int ret = -1;
	int sim = -1;
	FOR(i, similarity.size()) {
		if (similarity[i] > sim) {
			sim = similarity[i];
			ret = i;
		}
	}
	return ret;
}

Group Group::select() {
	if (chroms.size() == 0) return Group();

	// calcualte the probability
	if (similarity.size() == 0)
		calc_similarity();
	double fit_sum = 0.0;
	vector<double> fitness;
	FOR(i, chroms.size()) {
		double fit = similarity[i];
		fitness.push_back(fit);
		fit_sum += fit;
	}
	FOR(i, fitness.size()) {
		fitness[i] /= fit_sum;
	}
	vector<double> fitness_sum(fitness.size());
	fitness_sum[0] = fitness[0];
	for (size_t i = 1; i < fitness_sum.size(); ++i) {
		fitness_sum[i] = fitness_sum[i - 1] + fitness[i];
	}
	// select
	Group next;
	Random random;
	for (size_t i = 0; i < chroms.size(); ++i) {
		float rate = random.random<float>(1);
		int l = 0, r = fitness_sum.size() - 1;
		int k = -1;
		while (l <= r) {
			int mid = (l + r) / 2;
			if (rate <= fitness_sum[mid]) {
				k = mid;
				r = mid - 1;
			}
			else {
				l = mid + 1;
			}
		}
		//LOG("%d %f %f\n", k, fitness_sum[k], rate);
		next.chroms.push_back(chroms[k]);
		auto &last = next.chroms[next.chroms.size() - 1];
		last.data = new byte[last.size_in_byte];
		memcpy(last.data, chroms[k].data, last.size_in_byte);
	}

	this->destroy();

	return next;
}

void Group::cross(float cross_rate) {
	Random random;
	for (int i = 0; i < chroms.size(); i += 2) {
		float rate = random.random<float>(1);
		if (rate <= cross_rate) {
			int mi = i, fi = i + 1;
			auto &male = chroms[mi];
			auto &female = chroms[fi];

			bool byte_mode = false;
			int cp = random.random<int>(male.size(byte_mode));
			male.cross_with(female, cp, byte_mode);
		}
	}
}

void Group::mutate(float mutate_rate) {
	Random random;
	for (int i = 0; i < chroms.size(); ++i) {
		float rate = random.random<float>(1);
		if (rate <= mutate_rate) {
			auto &chrom = chroms[i];

			bool byte_mode = false;
			int cp = random.random<int>(chrom.size(byte_mode));
			chrom.mutate(cp, byte_mode);
		}
	}
}

Group Group::evolve(float cross_rate, float mutate_rate) {
	Group next = select();
	next.cross(cross_rate);
	next.mutate(mutate_rate);
	return next;
}