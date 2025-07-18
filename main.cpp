#include "CImg.h"
#include <filesystem>
#include <string>
#include <iostream>
#include "crosshair_remover.h"
#include "ellipse.h"
#include <fstream>
#include <vector>
#include <set>

namespace fs = std::filesystem;
using namespace cimg_library;

bool user_input() {
  std::string input;
  std::getline(std::cin, input);
  for (char& c : input) c = std::tolower(c);
  while (input != "y" && input != "n" && input != "yes" && input != "no") {
    std::getline(std::cin, input);
    for (char& c : input) c = std::tolower(c);
  }
  return (input == "y" || input == "yes");
}

bool is_image_file(const fs::path& path) {
  std::string ext = path.extension().string();
  for (char& c : ext) c = std::tolower(c);
  return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".png";
}

bool is_video_file(const fs::path& path) {
  std::string ext = path.extension().string();
  for (char& c : ext) c = std::tolower(c);
  return ext == ".mp4" || ext == ".mov";
}

fs::path get_folder_path(const fs::path& local_path, const std::string& folder_name) {
  fs::path folder_path(local_path.string() + "\\" + folder_name);
  if (!fs::exists(folder_path)) {
    create_directory(folder_path);
    std::cout << "Creating folder: " + folder_name + " at " + local_path.string() << std::endl;
  }
  return folder_path;
}

void clear_folder(const fs::path& folder_path) {
  for (const auto& entry : fs::directory_iterator(folder_path)) {
    fs::remove_all(entry.path());
  }
}

fs::path collect_images(const fs::path& local_path) {
  fs::path image_folder = get_folder_path(local_path, "images");
  std::cout << "Generate frames from video? (Y/N):" << std::endl;
  if (user_input()) {
    clear_folder(image_folder);
    fs::path video_folder = get_folder_path(local_path, "videos");
    for (const auto& entry : fs::directory_iterator(video_folder)) {
      std::cout << entry.path().filename().string() << "\n";
      if (is_video_file(entry.path())) {
        system(("ffmpeg -i " + entry.path().string() + " -q:v 2 " + image_folder.string() + "/frame_%04d.png").c_str());
      }
    }
  }
  return image_folder;
}

// Not used, debugging only
void dispPixels(const CImg<int>& image) {
  for (const auto& pixel : image) {
    std::cout << pixel << "\n";
  }
}

CImg<int> to_greyscale(CImg<unsigned char>& image) {
  CImg<int> newImage = CImg(image.width(),image.height(),1,1);
  newImage = image.RGBtoYCbCr().get_channel(0);
  return newImage;
}

void organize_images(const fs::path& image_folder_path, std::vector<CImg<unsigned char>>& original_images, std::vector<CImg<int>>& edited_images, std::vector<std::string>& filenames) {
  int num_images = 0;
  for ([[maybe_unused]] const auto& entry : fs::directory_iterator(image_folder_path)) num_images++;
  int count = 0;
  std::cout << "Reading " << num_images << " images:" << std::endl;
  for (const auto& entry : fs::directory_iterator(image_folder_path)) {
    if (entry.is_regular_file() && is_image_file(entry.path())) {
      auto image = CImg<unsigned char>(entry.path().string().c_str());
      original_images.push_back(image);
      edited_images.push_back(to_greyscale(image).normalize(0,255));
      filenames.push_back(entry.path().filename().string());
      count++;
      if (!(count%10)) {
        std::cout << "Reading images [ " << count << " / " << num_images << " ]\n";
      }
    }
  }
}

std::vector<std::pair<int,int>> remove_crosshairs(std::vector<CImg<int>>& images) {
  std::vector<std::pair<int,int>> centers;
  std::cout << "Remove Crosshairs? (Y/N):" << std::endl;
  if (user_input()) {
    int count = 0;
    for (auto& image : images) {
      centers.push_back(crosshair_remover::remove_crosshair(image, 5));
      count++;
      if (!(count%50)) {
        std::cout << "Removing crosshairs: [ " << count << " / " << images.size() << " ]\n";
      }
    }
  } else {
    for (auto& image : images) {
      centers.emplace_back(image.width()/2, image.height()/2);
    }
  }
  return centers;
}

std::vector<std::pair<int, int>> find_points(const CImg<int> &image, const int threshold) {
  CImg<unsigned char> binary = image.get_threshold(threshold).erode(15,15);
  CImg<unsigned int> labels = binary.get_label();
  int cx = image.width() / 2;
  int cy = image.height() / 2;
  unsigned int center_label = labels(cx, cy);
  std::vector<std::pair<int, int>> pixels;
  cimg_forXY(labels, x, y) {
    if (labels(x ,y) != center_label) {
      binary(x,y) = 0;
    }
  }
  binary.dilate(15,15);

  labels = binary.get_label();
  center_label = labels(cx, cy);
  cimg_forXY(labels, x, y) {
    if (labels(x, y) == center_label) {
      pixels.emplace_back(x, y);
    }
  }
  return pixels;
}

std::ofstream create_csv(const fs::path& local_path) {
  auto data_directory = get_folder_path(local_path,"data");
  auto number_set = std::set<int>();
  int size = 0;
  for (auto& entry : fs::directory_iterator(data_directory)) {
    if (entry.path().extension().string() == ".csv" && entry.path().filename().string().substr(0,4) == "data") {
      size++;
      number_set.insert(std::stoi(entry.path().filename().string().substr(4)));
    }
  }
  for (int i = 0; i<=size; i++) {
    if (!number_set.contains(i)) {
      std::ofstream csv(data_directory.string() + "\\data" + std::to_string(i) + ".csv");
      return csv;
    }
  }
  std::ofstream csv(data_directory.string() + "\\error.csv");
  return csv;
}

void fit_ellipses(const std::vector<CImg<int>>& images, std::vector<CImg<unsigned char>>& original_images, std::ofstream& csv, const std::vector<std::pair<int,int>>& centers) {
  std::cout << "Fitting Ellipses: " << std::endl;
  auto all_points = std::vector<std::vector<std::pair<int,int>>>();
  constexpr unsigned char BLUE[] = {0,0,255};
  constexpr unsigned char RED[] = {255,0,0};
  for (int i = 0; i < images.size(); i++) {
    all_points.push_back(find_points(images.at(i),235));
    for (auto&[x, y] : all_points.at(i)) {
      original_images.at(i).draw_point(x,y, BLUE, 0.5);
    }
    const ellipse ellipse(all_points.at(i));
    csv <<
      std::to_string(i) + "," +
      std::to_string(ellipse.a) + "," +
      std::to_string(ellipse.b) + "," +
      std::to_string(ellipse.angle) + "," +
      std::to_string(ellipse.cx - centers.at(i).first) + "," +
      std::to_string(ellipse.cy - centers.at(i).second) + ","
    << std::endl;
    original_images.at(i).draw_ellipse(static_cast<int>(ellipse.cx), static_cast<int>(ellipse.cy), ellipse.a, ellipse.b, ellipse.angle * 180 / std::numbers::pi,RED,1,10);
    if (!(i%10)) {
      std::cout << "Fitting ellipses: [ " + std::to_string(i) + " / " + std::to_string(images.size()) + " ]" << std::endl;
    }
  }
}

int main() {
  std::cout << "Melt Pool Analysis Program\n";
  const fs::path local_path = fs::current_path().parent_path();
  const fs::path image_folder_path = collect_images(local_path);

  std::ofstream csv = create_csv(local_path);
  csv << "frame,a (pixels),b (pixels),angle (radians),x (pixels),y (pixels)" << std::endl;

  std::vector<CImg<unsigned char>> original_images;
  std::vector<CImg<int>> edited_images;
  std::vector<std::string> filenames;

  organize_images(image_folder_path, original_images, edited_images, filenames);

  if (edited_images.empty()) {
    std::cerr << "No valid image files found in folder.\n";
    return 1;
  }

  std::vector<std::pair<int,int>> centers = remove_crosshairs(edited_images);

  fit_ellipses(edited_images, original_images, csv, centers);

  std::cout << "Fitting complete. Displaying images. Data has been written to a csv in " << local_path.string() << "\\data." << std::endl;

  int index = 0;
  CImgDisplay disp(original_images.at(index), filenames[index].c_str());
  disp.move(0,0);
  while (!disp.is_closed()) {
    // ReSharper disable once CppDFAUnreachableCode
    disp.wait();
    if (disp.is_keyARROWRIGHT()) {
      index = (index + 1) % original_images.size();
    } else if (disp.is_keyARROWLEFT()) {
      index = (index - 1 + original_images.size()) % original_images.size();
    }
    disp.display(original_images[index].get_normalize(0,255));
    disp.set_title(filenames[index].c_str());
  }

  return 0;
}
