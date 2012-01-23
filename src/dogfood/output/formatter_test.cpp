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

#include "../../output/formatter.hpp"

template <size_t N>
crpcut::datatypes::fixed_string mks(const char (&f)[N])
{
  crpcut::datatypes::fixed_string rv = { f, N - 1 };
  return rv;
}

#define S(x) mks("\"" #x "\"")

TESTSUITE(output)
{
  TESTSUITE(formatter)
  {
    class access : public crpcut::output::formatter
    {
    public:
      using crpcut::output::formatter::phase_str;
    };
    TEST(phase_strings_are_correct)
    {
      typedef access a;
      ASSERT_TRUE(a::phase_str(crpcut::creating)    == S(creating));
      ASSERT_TRUE(a::phase_str(crpcut::running)     == S(running));
      ASSERT_TRUE(a::phase_str(crpcut::destroying)  == S(destroying));
      ASSERT_TRUE(a::phase_str(crpcut::post_mortem) == S(post_mortem));
      ASSERT_TRUE(a::phase_str(crpcut::child)       == S(child));
    }
  }
}