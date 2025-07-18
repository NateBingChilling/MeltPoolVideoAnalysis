//
// Created by NateB on 7/14/2025.
//

#include "ellipse.h"
#include <cmath>

ellipse::ellipse(const double a, const double b, const double cx, const double cy, const double angle) {
    this->a = a;
    this->b = b;
    this->cx = cx;
    this->cy = cy;
    this->angle = angle;
}
ellipse::ellipse(const std::vector<std::pair<int, int> > &points) {
    long sx = 0, sy = 0;
    const long num_points = static_cast<long>(points.size());
    for (auto [px, py] : points) {
        sx += px;
        sy += py;
    }
    cx = static_cast<double>(sx) / num_points;
    cy = static_cast<double>(sy) / num_points;

    double covXX = 0, covYY = 0, covXY = 0;
    for (auto [px, py] : points) {
        const double dx = px - cx;
        const double dy = py - cy;
        covXX += dx * dx;
        covXY += dx * dy;
        covYY += dy * dy;
    }
    covXX /= num_points;
    covYY /= num_points;
    covXY /= num_points;

    const double trace = covXX + covYY;
    const double det = covXX * covYY - covXY * covXY;
    const double disc = sqrt(trace * trace / 4 - det);

    const double eigVal1 = trace / 2 + disc;
    const double eigVal2 = trace / 2 - disc;

    double eigVecX, eigVecY;
    if (covXY == 0) {
        if (covXX >= covYY) {
            eigVecX = 1;
            eigVecY = 0;
        } else {
            eigVecX = 0;
            eigVecY = 1;
        }
    } else {
        // Eigenvector for eigVal1
        eigVecX = eigVal1 - covYY;
        eigVecY = covXY;
        // Normalize the eigenvector
        const double mag = sqrt(eigVecX * eigVecX + eigVecY * eigVecY);
        eigVecX /= mag;
        eigVecY /= mag;
    }

    // 4. Determine ellipse parameters from eigenvalues and eigenvectors.
    // The angle of the ellipse is the angle of the principal eigenvector.
    angle = atan2(eigVecY, eigVecX);

    // The lengths of the semi-axes are proportional to the square root of the eigenvalues.
    // A scaling factor of 2 is often used to capture the main spread of the points.
    a = sqrt(eigVal1) * 2;
    b = sqrt(eigVal2) * 2;
}

void ellipse::PCA(const std::vector<std::pair<int, int> > &points) {
    long sx = 0, sy = 0;
    const long num_points = static_cast<long>(points.size());
    for (auto point : points) {
        sx += point.first;
        sy += point.second;
    }
    cx = static_cast<double>(sx) / num_points;
    cy = static_cast<double>(sy) / num_points;

    double covXX = 0, covYY = 0, covXY = 0;
    for (auto [px, py] : points) {
        double dx = px - cx;
        double dy = py - cy;
        covXX += dx * dx;
        covXY += dx * dy;
        covYY += dy * dy;
    }
    covXX /= num_points;
    covYY /= num_points;
    covXY /= num_points;

    const double trace = covXX + covYY;
    const double det = covXX * covYY - covXY * covXY;
    const double disc = sqrt(trace * trace / 4 - det);

    const double eigVal1 = trace / 2 + disc;
    const double eigVal2 = trace / 2 - disc;

    double eigVecX, eigVecY;
    if (covXY == 0) {
        if (covXX >= covYY) {
            eigVecX = 1;
            eigVecY = 0;
        } else {
            eigVecX = 0;
            eigVecY = 1;
        }
    } else {
        // Eigenvector for eigVal1
        eigVecX = eigVal1 - covYY;
        eigVecY = covXY;
        // Normalize the eigenvector
        const double mag = sqrt(eigVecX * eigVecX + eigVecY * eigVecY);
        eigVecX /= mag;
        eigVecY /= mag;
    }

    // 4. Determine ellipse parameters from eigenvalues and eigenvectors.
    // The angle of the ellipse is the angle of the principal eigenvector.
    angle = atan2(eigVecY, eigVecX);

    // The lengths of the semi-axes are proportional to the square root of the eigenvalues.
    // A scaling factor of 2 is often used to capture the main spread of the points.
    a = sqrt(eigVal1) * 2;
    b = sqrt(eigVal2) * 2;

}