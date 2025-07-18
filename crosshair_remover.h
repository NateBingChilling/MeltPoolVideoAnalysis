//
// Created by NateB on 7/7/2025.
//

#ifndef CROSSHAIR_REMOVER_H
#define CROSSHAIR_REMOVER_H

#include "CImg.h"
#include "Line.h"

using namespace cimg_library;

class crosshair_remover {

    static int find_horiz(const CImg<int>& image, int col_num);

    static int find_vert(const CImg<int>& image, int row_num);

    static void remove_horiz(CImg<int>& image, const Line &line, int line_width);

    // Removes vertical line
    static void remove_vert(CImg<int>& image, const Line &line, int line_width);

    static CImg<int> kernel_column(CImg<int> column);

    static CImg<int> kernel_row(CImg<int> row);

public:

    static std::pair<int,int> remove_crosshair(CImg<int> &image, int line_width);

};

#endif //CROSSHAIR_REMOVER_H
