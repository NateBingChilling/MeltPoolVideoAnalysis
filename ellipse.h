//
// Created by NateB on 7/14/2025.
//

#ifndef ELLIPSE_H
#define ELLIPSE_H
#include <vector>

class ellipse {
public:
    double a;
    double b;
    double cx;
    double cy;
    double angle;

    ellipse(double a, double b, double cx, double cy, double angle);
    explicit ellipse(const std::vector<std::pair<int,int>> &points);
private:
    void PCA(const std::vector<std::pair<int,int>>& points);
};

#endif //ELLIPSE_H
