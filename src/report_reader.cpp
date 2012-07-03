/*
 * Copyright 2011-2012 Bjorn Fahller <bjorn@fahller.se>
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
#include "test_runner.hpp"
#include "clocks/clocks.hpp"
#include "wrapped/posix_encapsulation.hpp"
#include "posix_error.hpp"

namespace crpcut {

  report_reader
  ::report_reader(crpcut_test_case_registrator *r, comm::data_writer &response)
    : fdreader(r),
      response_(response)
  {
  }

  void
  report_reader
  ::set_timeout(void *buff, size_t len)
  {
    typedef clocks::monotonic::timestamp timestamp;
    assert(len == sizeof(timestamp));
    timestamp *ts = static_cast<timestamp*>(buff);

    cancel_timeout();

    *ts+= clocks::monotonic::timestamp_ms_absolute();
    reg_->set_timeout(*ts);
    assert(reg_->deadline_is_set());
  }

  void
  report_reader
  ::cancel_timeout()
  {
    reg_->clear_deadline();
  }

  bool
  report_reader
  ::do_read_data(bool do_reply)
  {
    comm::type t;
    int kill_mask = 0;
    try {
      read_loop(&t, sizeof(t));
      kill_mask = t & comm::kill_me;
      if (kill_mask) do_reply = false; // unconditionally
      t = static_cast < comm::type >(t & ~kill_mask);
      if (t == comm::exit_fail || t == comm::fail || kill_mask)
        {
          reg_->crpcut_register_success(false);
        }
      size_t len = 0;
      read_loop(&len, sizeof(len));
      char *buff = static_cast<char *>(::alloca(len));
      read_loop(buff, len);

      if (kill_mask)
        {
          if (len == 0)
            {
              static char msg[] =
                  "A child process spawned from the test has misbehaved. "
                  "Process group killed";
              buff = msg;
              len = sizeof(msg) - 1;
            }
          t = comm::exit_fail;
          reg_->set_phase(child);
          reg_->kill();
        }
      if (do_reply)
        {
          response_.write_loop(&len, sizeof(len));
        }

      switch (t)
        {
        case comm::set_timeout:
          set_timeout(buff, len);
          return true;
        case comm::cancel_timeout:
          assert(len == 0);
          cancel_timeout();
          return true;
        case comm::begin_test:
          reg_->set_phase(running);
          {
            void *addr = buff;
            struct timeval *start_time = static_cast<struct timeval*>(addr);
            reg_->set_cputime_at_start(*start_time);
          }
          return true;
        case comm::end_test:
          if (!reg_->crpcut_failed())
            {
              reg_->set_phase(destroying);
              return true;
            }
          {
            static char msg[] = "Earlier VERIFY failed";
            buff = msg;
            len = sizeof(msg) - 1;
            t = comm::exit_fail;
            break;
          }
        default:
          break; // silence warning
        }

      reg_->send_to_presentation(t, len, buff);
      if (t == comm::exit_ok || t == comm::exit_fail)
        {
          reg_->set_death_note();
        }
    }
    catch (posix_error &e)
    {
      if (!e.what() || e.get_errno() == EPIPE)
        {
          return false;
        }
      throw;
    }
    return !kill_mask;
  }
}
