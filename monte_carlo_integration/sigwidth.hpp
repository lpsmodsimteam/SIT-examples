#ifndef SIGWIDTH_HPP
#define SIGWIDTH_HPP

#include <iomanip>
#include <sstream>

class SigWidth {

public:
    static void align_signal_width(int width, std::string& signal) {
        int _len = signal.length();
        if (_len < width) {
            signal = std::string(width - _len, '0') + signal;
        }
    }

    static void append_signal(const char chr, int width, std::string& signal) {
        int _len = signal.length();
        if (_len < width) {
            signal += std::string(width - _len, chr);
        }
    }

    static std::string align_signal_width(int width, float signal) {
        std::ostringstream _data_out;
        _data_out << std::fixed << std::setprecision(width) << signal;
        return _data_out.str().substr(0, width);
    }
};

#endif // SIGWIDTH_HPP
