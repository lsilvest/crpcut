/*
 * Copyright 2012 Bjorn Fahller <bjorn@fahller.se>
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include "param.hpp"
#include "../wrapped/posix_encapsulation.hpp"
#include <cassert>
#include <sstream>
namespace {
}

namespace crpcut {
  namespace cli {

    const char *const *
    param
    ::match(const char *const *p)
    {
      assert(p);
      if (*p == nullptr) return nullptr;
      const char *parameter_name = *p++;
      if (*parameter_name != '-') return nullptr;
      if (short_form_ != 0
          && parameter_name[1] == short_form_
          && parameter_name[2] == 0)
        {
          return p + match_value(*p, true);
        }
      if (parameter_name[1] != '-') return nullptr;
      parameter_name+= 2;
      const char *value = match_or_end(parameter_name, '=');
      const auto len = size_t(value - parameter_name);
      if (len != long_form_len_
          || wrapped::strncmp(long_form_, parameter_name, len) != 0)
        {
          return nullptr;
        }
      match_value(*value ? value + 1 : nullptr, false);
      return p;
    }

    std::ostream&
    param
    ::syntax(std::ostream &os) const
    {
      if (short_form_)
        {
          os << '-' << short_form_;
          if (value_description_)
            {
              os << " ";
              if (req_ == optional) os << "{";
              os << value_description_;
              if (req_ == optional) os << "}";
            }
          os << " / ";
        }
      os << "--" << long_form_;
      if (value_description_)
        {
          if (req_ == optional) os << "{";
          os << '=' << value_description_;
          if (req_ == optional) os << "}";
        }
      return os;
    }

    bool
    param
    ::match_value(const char *p, bool is_short)
    {
      if (!p) return false;
      if (is_short) return false;
      if (*p == '-') return false;
      std::ostringstream os;
      syntax(os) << " does not accept any value";
      throw exception(os.str());
    }

    std::ostream &
    param
    ::print_to(std::ostream &os) const
    {
      os << "   ";
      syntax(os);
      const char *begin = param_description_;
      while (*begin)
        {
          auto p = match_or_end(begin, '\n');
          os << "\n        ";
          os.write(begin, p - begin);
          begin = p + (*p == '\n');
        }
      return os;
    }

    const char *
    param
    ::match_or_end(const char *p, char c)
    {
      assert(p);
      while (*p && *p != c) ++p;
      return p;
    }

    const char *const *
    param_list
    ::match_all(const char *const *cli_arg)
    {
      assert(cli_arg);
      auto match_arg = [this](auto* to_match) {
        constexpr const char *const *end = nullptr;
        for (auto& p : *this)
        {
          if (auto arg = p.match(to_match))
          {
            return arg;
          }
        }
        return end;
      };
      while (*cli_arg)
        {
          auto next = match_arg(cli_arg);
          if (!next) break;
          cli_arg = next;
        }
      return cli_arg;
    }
  }
}






