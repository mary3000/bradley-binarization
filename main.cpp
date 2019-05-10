#include <utility>

#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <string>
#include <dirent.h>
#include <chrono>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "lib/stb_image_write.h"
#include "lib/stb_image.h"

#define CHANNELS_COUNT 3

class Image {
public:
    Image(std::string path) : path_(std::move(path)) {
        int channels_count;
        data_ = stbi_load(path_.data(), &width_, &height_, &channels_count, 3);
        block_width_ = width_ / block_fraq_;
    }

    Image(const Image& other) = delete;
    Image& operator=(const Image& other) = delete;

    Image(Image&& other) = delete;
    Image& operator=(Image&& other) = delete;

    ~Image() {
        stbi_image_free(data_);
    }

    long PixelCount() {
        return width_ * height_;
    }

    void GrayScale() {
        for (int i = 0; i < height_; ++i) {
            for (int j = 0; j < width_; ++j) {
                uint8_t mean = Get(i, j, 0) * 0.2126 + Get(i, j, 1) * 0.7152 + Get(i, j, 2) * 0.0722;
                Set(i, j, mean);
            }
        }
    }

    void BradleyAlgorithm() {
        std::vector<std::vector<long>> integral_data(height_);
        for (int i = 0; i < height_; ++i) {
            integral_data[i] = std::vector<long>(width_);
            int row = 0;
            for (int j = 0; j < width_; ++j) {
                row += Get(i, j, 0);
                if (i == 0) {
                    integral_data[i][j] = row;
                } else {
                    integral_data[i][j] = integral_data[i - 1][j] + row;
                }
            }
        }

        for (int i = 0; i < height_; ++i) {
            for (int j = 0; j < width_; ++j) {
                int x1 = std::max(0, i - block_width_);
                int x2 = std::min(height_ - 1, i + block_width_);
                int y1 = std::max(0, j - block_width_);
                int y2 = std::min(width_ - 1, j + block_width_);

                int count = (y2 - y1) * (x2 - x1);

                long sum = integral_data[x2][y2] - integral_data[x1][y2] - integral_data[x2][y1] + integral_data[x1][y1];
                uint8_t color = 255;
                if (1.0 * Get(i, j, 0) * count < sum * (1.0 - threshold_)) {
                    color = 0;
                }
                Set(i, j, color);
            }
        }
    }

    void Write(const std::string& path) {
        stbi_write_jpg(path.data(), width_, height_, CHANNELS_COUNT, data_, 100);
    }

private:
    int Index(int i, int j, int color) {
        return (i * width_ + j) * CHANNELS_COUNT + color;
    }

    uint8_t Get(int i, int j, int color) {
        return data_[Index(i, j, color)];
    }

    void Set(int i, int j, uint8_t val) {
        for (uint8_t color = 0; color < CHANNELS_COUNT; ++color) {
            data_[Index(i, j, color)] = val;
        }
    }

private:
    uint8_t* data_;
    int width_;
    int height_;
    const std::string path_;

    // Values for Bradley algorithm
    const int block_fraq_ = 16;
    int block_width_; // 8-width blocks
    const float threshold_ = 0.15;
};

void ThresholdDirs(const std::string &dir_name, std::vector<std::string> &image_names, const std::string &out) {
    for (auto& image_name : image_names) {
        Image image(dir_name + image_name);
        image.GrayScale();

        auto begin = std::chrono::steady_clock::now();
        image.BradleyAlgorithm();
        auto elapsed = std::chrono::steady_clock::now() - begin;
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);

        image.Write(out + image_name);

        // Here we should use these calculations:
        // elapsed_s = elapsed_ms / 1e6
        // performance (for 1e6 pixels) = 1e6 * elapsed_s / image_size
        // So, resulting formula:
        // performance = elapsed_ms / image_size
        std::cout << "Processed\t" + image_name << ", elapsed\t" + std::to_string(elapsed_ms.count()) + " μs"
        << ", performance\t" << std::to_string(elapsed_ms.count() * 1.0 / image.PixelCount()) << " s/megapixel" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *ent;
    std::vector<std::string> images;
    std::string dir_name = "../Dataset";
    std::string out = "../result/";
    if (argc > 1) {
        dir_name = argv[1];
    }
    if (argc > 2) {
        out = argv[2];
    }

    if ((dir = opendir (dir_name.data())) != nullptr) {
        while ((ent = readdir (dir)) != nullptr) {
            if (ent->d_type == 8) {
                images.emplace_back(ent->d_name);
            }
        }
        closedir (dir);
    } else {
        perror ("");
        return 1;
    }

    ThresholdDirs(dir_name + "/", images, out);

    return 0;
}