#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#include <sst/sit/sit.hpp>

class sc_cacc : public SST::Component {
   private:
    // Prepare the signal handler
    SITSocketBuffer m_signal_io;

    // SST parameters
    SST::Output m_output;
    std::string m_clock, m_proc, m_ipc_port;
    SST::Link *m_din_link, *m_dout_link;

   public:
    // constructor for component
    sc_cacc(SST::ComponentId_t id, SST::Params& params)
        : SST::Component(id),
          m_signal_io(20),
          m_clock(params.find<std::string>("clock", "")),
          m_proc(params.find<std::string>("proc", "")),
          m_ipc_port(params.find<std::string>("ipc_port", "")),
          m_din_link(configureLink(
              "sc_cacc_din",
              new SST::Event::Handler<sc_cacc>(this, &sc_cacc::handle_event)
          )),
          m_dout_link(configureLink("sc_cacc_dout")) {

        m_output.init("gen-" + getName() + " -> ", 1, 0, SST::Output::STDOUT);
        m_output.setVerboseLevel(params.find<bool>("OUTPUT", true));

        registerClock(
            m_clock, new SST::Clock::Handler<sc_cacc>(this, &sc_cacc::tick)
        );

        if (!(m_din_link && m_dout_link)) {
            m_output.fatal(CALL_INFO, -1, "Failed to configure port\n");
        }
    }

    void setup() {

        int child_pid = fork();

        if (!child_pid) {

            char* args[] = {&m_proc[0u], &m_ipc_port[0u], nullptr};
            m_output.verbose(
                CALL_INFO, 1, 0, "Forking process \"%s\"...\n", m_proc.c_str()
            );
            execvp(args[0], args);

        } else {

            m_signal_io.set_addr(m_ipc_port);
            m_signal_io.recv();
            if (child_pid == std::stoi(m_signal_io.get())) {
                m_output.verbose(
                    CALL_INFO,
                    1,
                    0,
                    "Process \"%s\" successfully synchronized\n",
                    m_proc.c_str()
                );
            }
        }
    }

    bool tick(SST::Cycle_t) {
        return false;
    };

    void finish() {
        m_output.verbose(
            CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str()
        );
    }

    void handle_event(SST::Event* ev) {

        auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);

        if (se) {

            std::string _data_in = se->getString();
            bool keep_send = _data_in[0] != '0';
            bool keep_recv = _data_in[1] != '0';
            _data_in = 'X' + _data_in.substr(2);

            // inputs from parent SST model, outputs to SystemC child process
            m_signal_io.set(_data_in);

            if (keep_send) {
                m_signal_io.set_state(keep_recv);
                m_signal_io.send();
            }
            if (keep_recv) {
                m_signal_io.recv();
            }

            // inputs to parent SST model, outputs from SystemC child process
            std::string _data_out = m_signal_io.get();
            m_dout_link->send(new SST::Interfaces::StringEvent(_data_out));
        }
    }

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        sc_cacc,        // class
        "monte_carlo",  // element library
        "sc_cacc",      // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        {"sc_cacc_din", "sc_cacc data in", {"sst.Interfaces.StringEvent"}},
        {"sc_cacc_dout", "sc_cacc data out", {"sst.Interfaces.StringEvent"}}
    )
};
