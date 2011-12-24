/*
 * Copyright 2011 Bjorn Fahller <bjorn@fahller.se>
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

#include "presentation.hpp"
#include "presentation_output.hpp"
#include "presentation_reader.hpp"
#include "pipe_pair.hpp"
#include "posix_encapsulation.hpp"
#include "posix_error.hpp"
#include "io.hpp"
#include "poll.hpp"
#include "output_buffer.hpp"

namespace crpcut {

  int start_presenter_process(int fd, output::formatter& fmt, int verbose)
  {
    pipe_pair p("communication pipe for presenter process");

    pid_t pid = wrapped::fork();
    if (pid < 0)
      {
        throw posix_error(errno, "forking presenter process");
      }
    if (pid != 0)
      {
        return p.for_writing(pipe_pair::release_ownership);
      }
    int presenter_pipe = p.for_reading();

    poll<io, 2> poller;
    presentation_reader r(poller, presenter_pipe, fmt, verbose);
    presentation_output o(poller, fd);
    while (poller.num_fds() > 0)
      {
        poll<io, 2>::descriptor desc = poller.wait();
        bool exc = false;
        if (desc.read())  exc |= desc->read();
        if (desc.write()) exc |= desc->write();
        if (desc.hup() || exc)   desc->exception();
        bool output_change = o.enabled() == output::buffer::is_empty();
        if (output_change) o.enable(!o.enabled());
      }
    wrapped::exit(0);
    return 0;
  }
}
