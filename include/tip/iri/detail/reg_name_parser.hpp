/*
 * reg_name_parser.hpp
 *
 *  Created on: Mar 11, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_DETAIL_REG_NAME_PARSER_HPP_
#define TIP_IRI_DETAIL_REG_NAME_PARSER_HPP_

#include <tip/iri/detail/iri_part.hpp>
#include <tip/iri/detail/char_class.hpp>

#include <string>
#include <algorithm>

namespace tip {
namespace iri {

inline namespace v2 {

template <>
struct parser<iri_part::reg_name>
    : detail::parser_base<parser<iri_part::reg_name>, iri_part::reg_name> {

    using base_type     = detail::parser_base<parser<iri_part::reg_name>, iri_part::reg_name>;
    using value_type    = ::std::string;
    using parser_state  = detail::parser_state;
    using feed_result   = detail::feed_result;
    using ucs_parser    = hex_encoded_parser<hex_range::ucschar>;

    parser() : state_{pstate::chars} {}

    feed_result
    feed_char(char c)
    {
        bool consumed = false;
        if (want_more()) {
            start();
            auto icls = iri_char_classification::classify(c);
            switch (state_) {
                case pstate::chars:
                    if (any(icls & iri_part::reg_name)) {
                        val_.push_back(c);
                        consumed = true;
                    } else if (c == '%'){
                        state_ = pstate::wait_x;
                        consumed = true;
                    } else {
                        finish();
                    }
                    break;
                case pstate::wait_x:
                    if (c == 'x') {
                        state_ = pstate::usc_char;
                    } else {
                        state_ = pstate::pct_encoded;
                        pct_parser_.feed_char(c);
                        if (pct_parser_.failed())
                            return {fail(), false};
                    }
                    consumed = true;
                    break;
                case pstate::pct_encoded:
                    pct_parser_.feed_char(c);
                    if (pct_parser_.failed())
                        return {fail(), false};
                    consumed = true;
                    if (pct_parser_.done()) {
                        state_ = pstate::chars;
                        val_.push_back(pct_parser_.value());
                        pct_parser_.clear();
                    }
                    break;
                case pstate::usc_char: {
                    auto cls = char_classification::classify(c);
                    if (any(cls & char_type::xdigit)) {
                        auto res = ucs_parser_.feed_char(c);
                        consumed = res.second;
                    } else {
                        ucs_parser_.finish();
                    }
                    if (ucs_parser_.failed())
                        return {fail(), consumed};
                    if (ucs_parser_.done()) {
                        state_ = pstate::chars;
                        auto v = ucs_parser_.value();
                        ::std::copy(v.begin(), v.end(), ::std::back_inserter(val_));
                        ucs_parser_.clear();
                    }
                    if (!(cls & char_type::xdigit)) {
                        // The char was not processed yet
                        return feed_char(c);
                    }
                    break;
                }
            }
        }
        return {state, consumed};
    }
    parser_state
    finish()
    {
        if (want_more()) {
            if (state_ == pstate::usc_char) {
                ucs_parser_.finish();
                if (ucs_parser_.failed())
                    return fail();
                state_ = pstate::chars;
                auto v = ucs_parser_.value();
                ::std::copy(v.begin(), v.end(), ::std::back_inserter(val_));
                ucs_parser_.clear();
            } else if (state_ != pstate::chars) {
                return fail();
            }
            if (val_.empty())
                return fail();
            base_type::finish();
        }
        return state;
    }

    void
    clear()
    {
        val_.clear();
        pct_parser_.clear();
        ucs_parser_.clear();
        base_type::reset();
    }

    value_type const&
    value() const
    { return val_; }
private:
    enum class pstate {
        chars,
        // Immediately after a % sign. If an 'x' chars are encountered,
        // this is a ucs_char, else - pct encoding
        wait_x,
        pct_encoded,
        usc_char
    };

    pstate              state_;
    value_type          val_;
    pct_encoded_parser  pct_parser_;
    ucs_parser          ucs_parser_;
};

} /* namespace v2 */

} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_DETAIL_REG_NAME_PARSER_HPP_ */
