#ifndef EVENT_HANDLERS_HPP
#define EVENT_HANDLERS_HPP

#include "monte_carlo.hpp"

void monte_carlo::x_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (mt19937_rdy && m_keep_recv && se) {
        std::string temp_se = se->getString();
        temp_se = temp_se.substr(0, temp_se.length() - 2);
        div_x_link->send(temp_se, MAX_32);
    }

    delete se;
}

void monte_carlo::y_rand32(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (mt19937_rdy && m_keep_recv && se) {
        std::string temp_se = se->getString();
        temp_se = temp_se.substr(0, temp_se.length() - 2);
        div_y_link->send(temp_se, MAX_32);
    }

    delete se;
}

void monte_carlo::div_x(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se) {
        x_val_norm = std::stof(se->getString());
        x_val_rdy = true;
    }

    delete se;
}

void monte_carlo::div_y(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se) {
        y_val_norm = std::stof(se->getString());
        y_val_rdy = true;
    }

    delete se;
}

void monte_carlo::sum_sq(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se) {
        std::string dist_str = se->getString();

        cacc_link->send(dist_str, inner_str, outer_str);
    }

    delete se;
}

void monte_carlo::cacc(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se && clock_high) {
        std::string temp_se = se->getString();

        inner_str = temp_se.substr(0, 10);
        inner = std::stol(inner_str);

        outer_str = temp_se.substr(10, 10);
        outer = std::stol(outer_str);

        std::string denom = std::to_string(inner + outer);
        div_areas_link->send(inner_str, denom);
    }

    delete se;
}

void monte_carlo::div_areas(SST::Event* ev) {

    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (m_keep_recv && se) {
        estimate = 4 * std::stof(se->getString());
    }

    delete se;
}

#endif  // EVENT_HANDLERS_HPP
