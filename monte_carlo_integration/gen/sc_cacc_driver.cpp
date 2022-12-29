#include <iomanip>
#include <sst/sit/sit.hpp>

#include "/home/sabbir/Desktop/SIT-examples/monte_carlo_integration/systemc/sc_cacc.hpp"

void align_signal_width(int width, std::string& signal) {
    int _len = signal.length();
    if (_len < width) {
        signal = std::string(width - _len, '0') + signal;
    }
}

int sc_main(int, char* argv[]) {

    sc_report_handler::set_actions(SC_ID_NO_SC_START_ACTIVITY_, SC_DO_NOTHING);
    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<float> dist;
    sc_signal<sc_uint<32>> inner;
    sc_signal<sc_uint<32>> outer;
    sc_signal<sc_uint<32>> new_inner;
    sc_signal<sc_uint<32>> new_outer;

    // Connect the DUT
    sc_cacc DUT("sc_cacc");
    DUT.dist(dist);
    DUT.inner(inner);
    DUT.outer(outer);
    DUT.new_inner(new_inner);
    DUT.new_outer(new_outer);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    // Initialize signal handlers
    SocketSignal m_signal_io(33, false);
    m_signal_io.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    m_signal_io.set(std::to_string(getpid()));
    m_signal_io.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    std::ostringstream _data_out;

    while (true) {

        // RECEIVING
        m_signal_io.recv();
        std::string _data_in = m_signal_io.get();

        if (_data_in[0] == '0') {
            break;
        }
        dist = std::stof(_data_in.substr(1, 12));
        inner = std::stol(_data_in.substr(13, 10));
        outer = std::stol(_data_in.substr(23, 10));

        // SENDING
        sc_start();

        std::string new_inner_str = std::to_string(new_inner.read());
        align_signal_width(10, new_inner_str);
        std::string new_outer_str = std::to_string(new_outer.read());
        align_signal_width(10, new_outer_str);

        // _data_out << std::fixed << std::setprecision(9) << new_inner;
        // _data_out << std::fixed << std::setprecision(9) << new_outer;

        // m_signal_io.set(_data_out.str());
        m_signal_io.set(new_inner_str + new_outer_str);
        m_signal_io.send();

        _data_out.str(std::string());
    }

    return 0;
}
