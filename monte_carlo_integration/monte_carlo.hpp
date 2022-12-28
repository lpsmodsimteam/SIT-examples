#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#include "sigwidth.hpp"

const unsigned int MT_ITERS = 624;
const unsigned int MT_BITS = 32;
const unsigned int MT_CYCLES = 2 * MT_ITERS * MT_BITS + 4;

class monte_carlo : public SST::Component {
   public:
    monte_carlo(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void x_rand32(SST::Event *);

    void y_rand32(SST::Event *);

    void div_x(SST::Event *);

    void div_y(SST::Event *);

    void sum_sq(SST::Event *);

    void cacc(SST::Event *);

    void div_areas(SST::Event *);

    SST_ELI_REGISTER_COMPONENT(
        monte_carlo, "monte_carlo", "monte_carlo",
        SST_ELI_ELEMENT_VERSION(1, 0, 0), "Simulator for the monte_carlo",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        {"x_rand32_din", "x_rand32_din", {"sst.Interfaces.StringEvent"}},
        {"x_rand32_dout", "x_rand32_dout", {"sst.Interfaces.StringEvent"}},
        {"y_rand32_din", "y_rand32_din", {"sst.Interfaces.StringEvent"}},
        {"y_rand32_dout", "y_rand32_dout", {"sst.Interfaces.StringEvent"}},
        {"div_x_din", "div_x_din", {"sst.Interfaces.StringEvent"}},
        {"div_x_dout", "div_x_dout", {"sst.Interfaces.StringEvent"}},
        {"div_y_din", "div_y_din", {"sst.Interfaces.StringEvent"}},
        {"div_y_dout", "div_y_dout", {"sst.Interfaces.StringEvent"}},
        {"sum_sq_din", "sum_sq_din", {"sst.Interfaces.StringEvent"}},
        {"sum_sq_dout", "sum_sq_dout", {"sst.Interfaces.StringEvent"}},
        {"cacc_din", "cacc_din", {"sst.Interfaces.StringEvent"}},
        {"cacc_dout", "cacc_dout", {"sst.Interfaces.StringEvent"}},
        {"div_areas_din", "cacc_din", {"sst.Interfaces.StringEvent"}},
        {"div_areas_dout", "cacc_dout", {"sst.Interfaces.StringEvent"}},
    )

   private:
    // SST parameters
    std::string seed1, seed2;
    unsigned int iterations{};
    bool clock_high;

    // SST attributes
    SST::Output m_output;

    LinkWrapper *x_rand32_link, *y_rand32_link, *div_x_link, *div_y_link,
        *sum_sq_link, *cacc_link, *div_areas_link;

    bool m_keep_send{}, m_keep_recv{};

    bool mt19937_rdy{}, x_mt19937_valid{}, y_mt19937_valid{};
    float x_val_norm{}, y_val_norm{};
    bool x_val_rdy{}, y_val_rdy{}, dist_rdy{};
    float dist{}, estimate{};
    int inner{}, outer{};

    unsigned int cur{};
    unsigned int SIMTIME;
};

#endif  // MONTE_CARLO_HPP
