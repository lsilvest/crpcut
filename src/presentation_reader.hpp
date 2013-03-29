/*
 * Copyright 2011-2013 Bjorn Fahller <bjorn@fahller.se>
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

#ifndef PRESENTATION_READER_HPP
#define PRESENTATION_READER_HPP

#include "test_case_result.hpp"

#include "io.hpp"
namespace crpcut {
  class registrator_list;
  namespace output {
    class formatter;
  }

  template <typename T> class poll;

  class presentation_reader : public io
  {
  public:
    presentation_reader(poll<io>               &poller,
                        comm::rfile_descriptor &fd,
                        output::formatter      &fmt,
                        output::formatter      &summary_fmt,
                        bool                    verbose,
                        const char             *working_dir,
                        registrator_list       &reg);
    virtual ~presentation_reader();
    virtual bool read();
    virtual bool write();
    virtual void exception();
    unsigned num_failed() const;
  private:
    test_case_result *find_result_for(pid_t);
    void begin_test(test_case_result*);
    void end_test(test_phase phase, test_case_result *);
    void nonempty_dir(test_case_result *);
    void output_data(comm::type               t,
                     test_case_result        *result,
                     datatypes::fixed_string  msg,
                     datatypes::fixed_string  location);
    void blocked_test();
    datatypes::list_elem<test_case_result>  messages_;
    poll<io>                               &poller_;
    comm::rfile_descriptor                 &fd_;
    output::formatter                      &fmt_;
    output::formatter                      &summary_fmt_;
    const char                             *working_dir_;
    bool                                    verbose_;
    unsigned                                num_run_;
    unsigned                                num_failed_;
    registrator_list                       &reg_;
    presentation_reader();
    presentation_reader(const presentation_reader&);
    presentation_reader& operator=(const presentation_reader&);
  };
}

#endif // PRESENTATION_READER_HPP
