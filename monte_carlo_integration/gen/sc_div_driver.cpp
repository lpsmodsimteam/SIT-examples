#include <iomanip>
#include <sst/sit/sit.hpp>

#include "/home/sabbir/SIT-Examples/monte_carlo_integration/systemc/sc_div.hpp"

int sc_main(int, char* argv[]) {

    sc_report_handler::set_actions(SC_ID_NO_SC_START_ACTIVITY_, SC_DO_NOTHING);
    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<sc_uint<32>> op1;
    sc_signal<sc_uint<32>> op2;
    sc_signal<float> data_out;

    // Connect the DUT
    sc_div DUT("sc_div");
    DUT.op1(op1);
    DUT.op2(op2);
    DUT.data_out(data_out);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    // Initialize signal handlers
    SocketSignal m_signal_io(21, false);
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
        op1 = std::stol(_data_in.substr(1, 10));
        op2 = std::stol(_data_in.substr(11, 10));

        // SENDING
        sc_start();

        _data_out << std::fixed << std::setprecision(9) << data_out;

        m_signal_io.set(_data_out.str());
        m_signal_io.send();

        _data_out.str(std::string());
    }

    return 0;
}
