//
// Created by NateB on 7/7/2025.
//

#include "crosshair_remover.h"

int crosshair_remover::find_horiz(const CImg<int>& image, const int col_num) {
    CImg<int> column = kernel_column(kernel_column(image.get_crop(col_num,col_num)));
    int max_val = 0, max_index = 0;
    for (int i = 0; i<column.height(); i++) {
        if (column(0, i) > max_val) {
            max_val = column(0, i);
            max_index = i;
        }
    }
    return max_index;
}

int crosshair_remover::find_vert(const CImg<int>& image, const int row_num) {
    CImg<int> row = kernel_row(kernel_row(image.get_crop(0,row_num,image.width(),row_num)));
    int max_val = 0, max_index = 0;
    for (int i = 0; i<row.width(); i++) {
        if (row(i,0) > max_val) {
            max_val = row(i,0);
            max_index = i;
        }
    }
    return max_index;
}

void crosshair_remover::remove_horiz(CImg<int> &image, const Line &line, const int line_width) {
    int val;
    for (int x = 0 ; x < image.width(); x++) {
        for (int y = (val = line.evaluate(x)) - line_width; y <= val + line_width; y++) {
            image(x,y) =
                static_cast<int>(roundl((image(x,val-line_width-1) * (val-y+line_width+1) + (image(x,val+line_width+1) * (y-val+line_width+1))) / (1.0 * (2 * line_width + 2))));
        }
    }
}

void crosshair_remover::remove_vert(CImg<int> &image, const Line &line, const int line_width) {
    int val;
    for (int y = 0; y < image.height(); y++) {
        for (int x = (val = line.evaluate(y)) - line_width; x <= val + line_width; x++) {
            image(x,y) =
                static_cast<int>(roundl((image(val-line_width-1,y) * (val-x+line_width+1) + (image(val+line_width+1,y) * (x-val+line_width+1))) / (1.0 * (2 * line_width + 2))));

        }
    }
}

CImg<int> crosshair_remover::kernel_column(CImg<int> column) {
    int prev1 = column(0,0);
    for (int i = 1; i<column.height()-1; i++) {
        const int prev2 = column(0, i);
        column(0,i) = column(0,i+1) - prev1;
        prev1 = prev2;
    }
    column(0,0) = column(0,1);
    column(0,column.height()-1) = column(0,column.height()-2);
    return column;
}

CImg<int> crosshair_remover::kernel_row(CImg<int> row) {
    int prev1 = row(0,0);
    for (int i = 1; i<row.width()-1; i++) {
        const int prev2 = row(i, 0);
        row(i,0) = row(i+1,0) - prev1;
        prev1 = prev2;
    }
    row(0,0) = row(1,0);
    row(row.width()-1,0) = row(row.width()-2,0);
    return row;
}

std::pair<int, int> crosshair_remover::remove_crosshair(CImg<int> &image, const int line_width) {
    const Line horiz_line(find_horiz(image, 1), find_horiz(image, image.width()-2), image.width());
    const Line vert_line(find_vert(image, 10), find_vert(image, image.height()-2), image.height());

    remove_horiz(image, horiz_line, line_width);
    remove_vert(image, vert_line, line_width);

    return horiz_line.intersection(vert_line);
}
