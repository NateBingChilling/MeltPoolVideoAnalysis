//
// Created by NateB on 7/9/2025.
//

#ifndef LINE_H
#define LINE_H
#include <vector>

class Line {
    int width, start, end;
    double slope;
public:
    Line();

    Line(int start_intercept, int end_intercept, int line_width);

    [[nodiscard]] std::pair<int,int> intersection(const Line &line) const;

    [[nodiscard]] int evaluate(int input) const;

    [[nodiscard]] int get_width() const;

    void set_width(int width);

    [[nodiscard]] int get_start() const;

    void set_start(int start);

    [[nodiscard]] int get_end() const;

    void set_end(int end);

    [[nodiscard]] double get_slope() const;

private:
    void calc_slope();
};



#endif //LINE_H
