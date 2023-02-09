#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#include <sst/sit/sit.hpp>

class mt19937 : public SST::Component {
   private:
    // Prepare the signal handler
    SITSocketBuffer sit_buf;

    // SST parameters
    SST::Output m_output;
    std::string m_clock, m_proc, m_ipc_port;
    SST::Link *m_din_link, *m_dout_link;

   public:
    mt19937(SST::ComponentId_t id, SST::Params& params)
        : SST::Component(id),
          sit_buf(12),
          m_clock(params.find<std::string>("clock", "")),
          m_proc(params.find<std::string>("proc", "")),
          m_ipc_port(params.find<std::string>("ipc_port", "")),
          m_din_link(configureLink(
              "mt19937_din",
              new SST::Event::Handler<mt19937>(this, &mt19937::handle_event)
          )),
          m_dout_link(configureLink("mt19937_dout")) {

        m_output.init("gen-" + getName() + " -> ", 1, 0, SST::Output::STDOUT);
        m_output.setVerboseLevel(params.find<bool>("OUTPUT", true));

        registerClock(
            m_clock, new SST::Clock::Handler<mt19937>(this, &mt19937::tick)
        );

        if (!(m_din_link && m_dout_link)) {
            m_output.fatal(CALL_INFO, -1, "Failed to configure port\n");
        }
    }

    void setup() {
        int child_pid = fork();

        if (!child_pid) {
            std::string cmd = m_proc + m_ipc_port;

            char* args[8];
            int i = 0;
            args[i] = std::strtok(&cmd[0u], " ");

            while (args[i] != nullptr) {
                args[++i] = strtok(nullptr, " ");
            }
            args[i] = nullptr;

            m_output.verbose(
                CALL_INFO, 1, 0, "Forking process \"%s\"...\n", cmd.c_str()
            );
            execvp(args[0], args);

        } else {
            sit_buf.set_addr(m_ipc_port);
            sit_buf.recv();
            if (child_pid == std::stoi(sit_buf.get())) {
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
    }

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

            // inputs from parent SST model, outputs to PyRTL child process
            sit_buf.set(_data_in);

            if (keep_send) {
                sit_buf.set_state(keep_recv);
                sit_buf.send();
            }
            if (keep_recv) {
                sit_buf.recv();
            }

            // inputs to parent SST model, outputs from PyRTL child process
            std::string _data_out = sit_buf.get();
            m_dout_link->send(new SST::Interfaces::StringEvent(_data_out));
        }
    }

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        mt19937, // class
        "monte_carlo", // element library
        "mt19937", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        {"mt19937_din", "mt19937 data in", {"sst.Interfaces.StringEvent"}},
        {"mt19937_dout", "mt19937 data out", {"sst.Interfaces.StringEvent"}}
    )
};
