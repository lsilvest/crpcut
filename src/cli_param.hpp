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



#ifndef CLI_PARAM_HPP_
#define CLI_PARAM_HPP_

#include <exception>
#include <string>

namespace crpcut {
  class cli_param
  {
  public:
    class exception;
    template <size_t N>
    cli_param(char short_form, const char (&long_form)[N],
              const char *param_description);
    template <size_t N>
    cli_param(char short_form, const char (&long_form)[N],
              const char *value_description, const char *param_description);
    virtual ~cli_param();
    const char *const *match(const char *const *);
    friend std::ostream &operator<<(std::ostream &os, const cli_param &p)
    {
      return p.print_to(os);
    }
  private:
    std::ostream &print_to(std::ostream &) const;
    std::ostream &syntax(std::ostream &) const;
    virtual bool match_value(const char *, bool is_short);
    const char *const long_form_;
    const size_t      long_form_len_;
    const char        short_form_;
    const char *const value_description_;
    const char *const param_description_;
  };

  class cli_param::exception : public std::exception
  {
  public:
    exception(std::string s) : s_(s) {}
    const char *what() const throw () { return s_.c_str(); }
  private:
    std::string s_;
  };
  template <size_t N>
  cli_param::cli_param(char short_form, const char (&long_form)[N],
                       const char *param_description)
    : long_form_(long_form),
      long_form_len_(N - 1),
      short_form_(short_form),
      value_description_(0),
      param_description_(param_description)
  {
  }

  template <size_t N>
  cli_param::cli_param(char short_form, const char (&long_form)[N],
                       const char *value_description,
                       const char *param_description)
    : long_form_(long_form),
      long_form_len_(N - 1),
      short_form_(short_form),
      value_description_(value_description),
      param_description_(param_description)
  {
  }

}





#endif // CLI_PARAM_HPP_
