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

#include <crpcut.hpp>

namespace {
  const char *get_parameter_string(const char *n)
  {
    using namespace crpcut;
    const char *p = test_case_factory::get_parameter(n);
    if (!p)
      {
        static const char msg[] = "No parameter named \"";
        const size_t len = sizeof(msg) + wrapped::strlen(n) + 1;
        char *buff = static_cast<char*>(alloca(len+1));
        lib::strcpy(lib::strcpy(lib::strcpy(buff, msg), n), "\"");
        comm::report(comm::exit_fail, buff, len);
      }
    return p;
  }
}

namespace crpcut {
  istream_wrapper
  parameter_stream_traits<std::istream>
  ::make_value(const char *n)
  {
    return istream_wrapper(get_parameter_string(n));
  }

  stream::iastream
  parameter_stream_traits<relaxed<std::istream> >
  ::make_value(const char *n)
  {
    return stream::iastream(get_parameter_string(n));
  }
}