#include "genetic.h"
using namespace cv;
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