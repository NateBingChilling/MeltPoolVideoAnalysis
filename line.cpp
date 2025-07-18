
#include "Line.h"
#include <vector>
#include <cmath>

Line::Line() {
    slope = start = end = 0;
    width = 1;
}

Line::Line(const int start_intercept, const int end_intercept, const int line_width) {
    slope = ((end = end_intercept) - (start = start_intercept)) / static_cast<double>(width = line_width);
}

std::pair<int,int> Line::intersection(const Line &line) const {
    return std::make_pair(
    static_cast<int>(lround((line.slope * start + line.start)/(1 - line.slope * slope))),
    static_cast<int>(lround((slope * line.start + start)/(1 - line.slope * slope))));
}

int Line::evaluate(const int input) const {
    return static_cast<int>(roundl(start + input * slope));
}

int Line::get_width() const {
    return width;
}

int Line::get_start() const {
    return start;
}

int Line::get_end() const {
    return end;
}

double Line::get_slope() const {
    return slope;
}

void Line::set_width(const int width) {
    this->width = width;
    calc_slope();
}

void Line::set_start(const int start) {
    this->start = start;
    calc_slope();
}

void Line::set_end(const int end) {
    this->end = end;
    calc_slope();
}

void Line::calc_slope() {
    this->slope = (end - start) / static_cast<double>(width);
}
