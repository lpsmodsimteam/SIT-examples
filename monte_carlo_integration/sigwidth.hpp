#ifndef SIGWIDTH_HPP
#define SIGWIDTH_HPP

#include <iomanip>
#include <sstream>

#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

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

class LinkWrapper : public SST::Link {

    bool *m_keep_send{}, *m_keep_recv{};

public:
    SST::Link *din_link, *dout_link;
    LinkWrapper(bool* keep_send, bool* keep_recv) {
        m_keep_send = keep_send;
        m_keep_recv = keep_recv;
    }

    inline std::string const& to_string(std::string const& s) {
        return s;
    }

    template <typename... Args> void send(Args const&... args) {

        std::string s;
        int unpack[]{0, (s += to_string(args), 0)...};
        static_cast<void>(unpack);

        din_link->send(new SST::Interfaces::StringEvent(
            std::to_string(*m_keep_send) + std::to_string(*m_keep_recv) + s));
    }
};

#endif // SIGWIDTH_HPP
