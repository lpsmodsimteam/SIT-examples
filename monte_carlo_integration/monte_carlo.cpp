#include "monte_carlo.hpp"

monte_carlo::monte_carlo(SST::ComponentId_t id, SST::Params& params)
    : SST::Component(id) {

    seed1 = params.find<std::string>("SEED1", "0000000000");
    seed2 = params.find<std::string>("SEED2", "0000000000");
    iterations = params.find<uint64_t>("ITER", 100);
    SIMTIME = MT_CYCLES + iterations * 2 + 3;
    LOOPEND = SIMTIME;

    x_rand32_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    x_rand32_link->set_din_link(configureLink("x_rand32_din"));
    x_rand32_link->set_dout_link(configureLink(
        "x_rand32_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::x_rand32)
    ));

    y_rand32_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    y_rand32_link->set_din_link(configureLink("y_rand32_din"));
    y_rand32_link->set_dout_link(configureLink(
        "y_rand32_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::y_rand32)
    ));

    div_x_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    div_x_link->set_din_link(configureLink("div_x_din"));
    div_x_link->set_dout_link(configureLink(
        "div_x_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::div_x)
    ));

    div_y_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    div_y_link->set_din_link(configureLink("div_y_din"));
    div_y_link->set_dout_link(configureLink(
        "div_y_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::div_y)
    ));

    sum_sq_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    sum_sq_link->set_din_link(configureLink("sum_sq_din"));
    sum_sq_link->set_dout_link(configureLink(
        "sum_sq_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::sum_sq)
    ));

    cacc_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    cacc_link->set_din_link(configureLink("cacc_din"));
    cacc_link->set_dout_link(configureLink(
        "cacc_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::cacc)
    ));

    div_areas_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    div_areas_link->set_din_link(configureLink("div_areas_din"));
    div_areas_link->set_dout_link(configureLink(
        "div_areas_dout",
        new SST::Event::Handler<monte_carlo>(this, &monte_carlo::div_areas)
    ));

    x_val_rdy = false;
    y_val_rdy = false;
    clock_high = false;

    m_output.init(
        "\033[93mmonte_carlo-" + getName() + "\033[0m -> ",
        1,
        0,
        SST::Output::STDOUT
    );

    // m_output.setVerboseLevel(output);

    // Just register a plain clock for this simple example
    registerClock(
        "1Hz", new SST::Clock::Handler<monte_carlo>(this, &monte_carlo::tick)
    );

    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
}

void monte_carlo::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");
}

void monte_carlo::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());
}

bool monte_carlo::tick(SST::Cycle_t current_cycle) {

    m_cycle = current_cycle;
    clock_high = current_cycle % 2;
    m_keep_send = current_cycle < SIMTIME;
    m_keep_recv = current_cycle < SIMTIME - 1;
    mt19937_rdy = current_cycle > MT_CYCLES;

    std::string mt_rst = "0";
    std::string mt_seed_start = "0";
    std::string mt_rdy = "0";

    if (current_cycle == 1) {
        mt_rst = "1";
    }

    if (current_cycle == 3) {
        mt_seed_start = "1";
    }

    if (current_cycle > 4) {
        mt_rdy = "1";
    }

    x_rand32_link->send(
        mt_rst, seed1, mt_seed_start, mt_rdy, std::to_string(clock_high)
    );

    y_rand32_link->send(
        mt_rst, seed2, mt_seed_start, mt_rdy, std::to_string(clock_high)
    );

    if (x_val_rdy && y_val_rdy) {
        sum_sq_link->send(
            std::to_string(x_val_norm), std::to_string(y_val_norm)
        );
        x_val_rdy = false;
        y_val_rdy = false;
    }

    if (current_cycle > MT_CYCLES && clock_high && estimate) {

        m_output.verbose(
            CALL_INFO,
            1,
            0,
            "cycle: %ld iter: %3d π: %.6f\n",
            current_cycle,
            ++cur,
            estimate
        );
    }

    if (current_cycle == SIMTIME) {
        primaryComponentOKToEndSim();
    }

    return current_cycle == SIMTIME;
}

void monte_carlo::x_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (mt19937_rdy && m_keep_recv && se) {
        std::string temp_se = se->getString();
        temp_se = temp_se.substr(0, temp_se.length() - 2);
        SigWidth::align_signal_width(10, temp_se);
        div_x_link->send(temp_se, "4294967295");
    }

    delete se;
}

void monte_carlo::y_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (mt19937_rdy && m_keep_recv && se) {
        std::string temp_se = se->getString();
        temp_se = temp_se.substr(0, temp_se.length() - 2);
        SigWidth::align_signal_width(10, temp_se);
        div_y_link->send(temp_se, "4294967295");
    }

    delete se;
}

void monte_carlo::div_x(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se && se->getString().length()) {
        x_val_norm = std::stof(se->getString());
        x_val_rdy = true;
    }

    delete se;
}

void monte_carlo::div_y(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se && se->getString().length()) {
        y_val_norm = std::stof(se->getString());
        y_val_rdy = true;
    }

    delete se;
}

void monte_carlo::sum_sq(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se && se->getString().length()) {
        std::string dist_str = se->getString();
        SigWidth::align_signal_width(12, dist_str);
        std::string inner_str = std::to_string(inner);
        SigWidth::align_signal_width(10, inner_str);
        std::string outer_str = std::to_string(outer);
        SigWidth::align_signal_width(10, outer_str);
        cacc_link->send(dist_str, inner_str, outer_str);
    }

    delete se;
}

void monte_carlo::cacc(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se && clock_high) {
        // std::cout << "coming back here " << se->getString() << '\n';
        std::string temp_se = se->getString();
        inner = std::stol(temp_se.substr(0, 10));
        outer = std::stol(temp_se.substr(10, 10));
        // std::cout << "inner: " << inner << " outer: " << outer << '\n';

        std::string denom = std::to_string(inner + outer);
        SigWidth::align_signal_width(10, denom);
        div_areas_link->send(temp_se.substr(0, 10), denom);
    }

    delete se;
}

void monte_carlo::div_areas(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se && se->getString().length()) {
        // std::cout << "divided? " << se->getString() << '\n';
        estimate = 4 * std::stof(se->getString());
    }

    delete se;
}
