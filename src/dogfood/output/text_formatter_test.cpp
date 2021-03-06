/*
 * Copyright 2012-2013 Bjorn Fahller <bjorn@fahller.se>
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

#include <trompeloeil.hpp>
#include <crpcut.hpp>
#include "../../output/text_formatter.hpp"
#include "../../output/text_modifier.hpp"
#include "stream_buffer_mock.hpp"
#include "tag_mocks.hpp"

using trompeloeil::_;

namespace {
  static const char rules[] =
    "|"
    "<>|"
    "PASSED=<P>|"
    "FAILED=<F>|"
    "NCFAILED=<NF>|"
    "NCPASSED=<NP>|"
    "BLOCKED=<B>|"
    "PASSED_SUM=<PS>|"
    "FAILED_SUM=<FS>|"
    "NCPASSED_SUM=<NPS>|"
    "NCFAILED_SUM=<NFS>|"
    "BLOCKED_SUM=<BS>|";
  crpcut::output::text_modifier test_modifier(rules);
  crpcut::output::text_modifier empty_modifier(0);


  struct stream_buffer : public crpcut::output::buffer
  {
  public:
    typedef std::pair<const char*, std::size_t> buff;
    MAKE_CONST_MOCK0(get_buffer, buff());
    MAKE_MOCK0(advance, void());
    virtual ssize_t write(const char *data, std::size_t len)
    {
      os.write(data, std::streamsize(len));
      return ssize_t(len);
    }
    MAKE_CONST_MOCK0(is_empty, bool());
    std::ostringstream os;
  };

  class tag_list : public crpcut::tag_list_root
  {
  public:
    MAKE_MOCK0(fail, void());
    MAKE_MOCK0(pass, void());
    MAKE_CONST_MOCK0(num_failed, size_t());
    MAKE_CONST_MOCK0(num_passed, size_t());
    MAKE_CONST_MOCK0(get_name, crpcut::datatypes::fixed_string());
    MAKE_CONST_MOCK0(longest_tag_name, size_t());
    MAKE_MOCK1(set_importance, void(crpcut::tag::importance));
    MAKE_CONST_MOCK0(get_importance, crpcut::tag::importance());
  };

  class test_tag : public crpcut::tag
  {
  public:
    template <size_t N>
    test_tag(const char (&f)[N], tag_list *root)
      : crpcut::tag(N, root),
        name_(crpcut::datatypes::fixed_string::make(f)),
        name_x(NAMED_REQUIRE_CALL(*this, get_name())
               .RETURN(std::ref(name_)))
    {
    }
    MAKE_MOCK0(fail, void());
    MAKE_MOCK0(pass, void());
    MAKE_CONST_MOCK0(num_failed, size_t());
    MAKE_CONST_MOCK0(num_passed, size_t());
    MAKE_CONST_MOCK0(get_name, crpcut::datatypes::fixed_string());
    MAKE_MOCK1(set_importance, void(crpcut::tag::importance));
    MAKE_CONST_MOCK0(get_importance, crpcut::tag::importance());
    crpcut::datatypes::fixed_string name_;
    std::unique_ptr<trompeloeil::expectation> name_x;
  };
  static crpcut::datatypes::fixed_string empty_string = { "", 0 };


  class fix
  {
  protected:
    fix()
      : name_x(NAMED_ALLOW_CALL(tags, get_name())
               .RETURN(empty_string)),
        longest_x(NAMED_ALLOW_CALL(tags, longest_tag_name())
                  .RETURN(0U))
    {
    }
    mock::tag_list tags;
    std::unique_ptr<trompeloeil::expectation> name_x;
    std::unique_ptr<trompeloeil::expectation> longest_x;
  };

  const char *const no_char_conversion = 0;
}

#define _ "[[:space:]]*"
#define s crpcut::datatypes::fixed_string::make


TESTSUITE(output)
{

  TESTSUITE(text_formatter)
  {
    static const char* vec[] = { "apa", "katt", "orm", 0 };

    TEST(construction_and_immediate_destruction_has_no_effect, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        {
          crpcut::output::text_formatter obj(test_buffer,
                                             "one",
                                             vec,
                                             tags,
                                             100,
                                             10,
                                             test_modifier,
                                             no_char_conversion);
        }
        ASSERT_TRUE(test_buffer.os.str() == "");
      }
    }

    TEST(stats_without_test_list_shows_only_pass_line_for_only_pass_results,
         fix)
    {
      REQUIRE_CALL(tags, num_passed())
        .RETURN(13U);
      REQUIRE_CALL(tags, num_failed())
        .RETURN(0U);
      REQUIRE_CALL(tags, get_importance())
        .RETURN(crpcut::tag::critical);

      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        {
          crpcut::output::text_formatter obj(test_buffer,
                                             "one",
                                             vec,
                                             tags,
                                             15, 14,
                                             test_modifier,
                                             no_char_conversion);
          obj.statistics(13, 0);
        }

        static const char re[] =
          "14 test cases selected\n\n"
                               _ "Sum" _ "Critical" _ "Non-critical\n"
          "<PS>PASSED"   _ ":" _ "13"  _ "13"       _ "0<>\n"
          "<BS>UNTESTED" _ ":" _ "1<>\n$";

        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m),
                    test_buffer.os.str());
      }
    }

    TEST(stats_without_test_list_shows_only_fail_line_for_only_fail_results,
         fix)
    {
      REQUIRE_CALL(tags, num_passed())
        .RETURN(0U);
      REQUIRE_CALL(tags, num_failed())
        .RETURN(13U);
      REQUIRE_CALL(tags, get_importance())
        .RETURN(crpcut::tag::critical);

      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        {
          crpcut::output::text_formatter obj(test_buffer,
                                             "one",
                                             vec,
                                             tags,
                                             15,13,
                                             test_modifier,
                                             no_char_conversion);
          obj.statistics(13, 13);
        }
        static const char re[] =
              "13 test cases selected\n\n"
                               _ "Sum" _ "Critical" _ "Non-critical\n"
          "<FS>FAILED"   _ ":" _ "13"  _ "13"       _ "0<>\n$";

        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m),
                    test_buffer.os.str());
      }
    }

#define MAKE_TAG(n) mock::test_tag n(#n, &tags)

    TEST(stats_with_mixed_crit_ncrit_pass_only_shows_pass_sum,
         fix)
    {
      ALLOW_CALL(tags, longest_tag_name())
        .RETURN(15U);
      REQUIRE_CALL(tags, num_passed())
        .RETURN(0U);
      REQUIRE_CALL(tags, num_failed())
        .RETURN(0U);
      REQUIRE_CALL(tags, get_importance())
        .RETURN(crpcut::tag::critical);

      MAKE_TAG(apa);
      ALLOW_CALL(apa, num_passed())
        .RETURN(5U);

      ALLOW_CALL(apa, num_failed())
        .RETURN(0U);

      REQUIRE_CALL(apa, get_importance())
        .RETURN(crpcut::tag::critical);

      MAKE_TAG(katt);
      ALLOW_CALL(katt, num_passed())
        .RETURN(3U);

      ALLOW_CALL(katt, num_failed())
        .RETURN(0U);

      REQUIRE_CALL(katt, get_importance())
        .RETURN(crpcut::tag::non_critical);

      MAKE_TAG(a_long_tag_name);
      ALLOW_CALL(a_long_tag_name, num_passed())
        .RETURN(1U);

      ALLOW_CALL(a_long_tag_name, num_failed())
        .RETURN(0U);

      REQUIRE_CALL(a_long_tag_name, get_importance())
        .RETURN(crpcut::tag::non_critical);

      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        {
          crpcut::output::text_formatter obj(test_buffer,
                                             "one",
                                             vec,
                                             tags,
                                             9,9,
                                             test_modifier,
                                             no_char_conversion);
          obj.statistics(9, 0);
        }
        static const char re[] =
              "9 test cases selected\n"
              " tag                 run  passed  failed\n"
           "<P>!apa                   5       5       0<>\n"
          "<NP>?katt                  3       3       0<>\n"
          "<NP>?a_long_tag_name       1       1       0<>\n"
          "\n"
                               _ "Sum" _ "Critical" _ "Non-critical\n"
          "<PS>PASSED"   _ ":" _ "9"   _ "5"        _ "4<>\n$"
          ;
        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m),
                    test_buffer.os.str());
      }
    }

    TEST(stats_with_mixed_crit_ncrit_fail_only_shows_fail_sum, fix)
    {
      ALLOW_CALL(tags, longest_tag_name()).RETURN(20U);
      REQUIRE_CALL(tags, num_passed()).RETURN(0U);
      REQUIRE_CALL(tags, num_failed()).RETURN(0U);
      REQUIRE_CALL(tags, get_importance()).RETURN(crpcut::tag::critical);

      MAKE_TAG(apa);
      ALLOW_CALL(apa, num_passed())
        .RETURN(0U);

      ALLOW_CALL(apa, num_failed())
        .RETURN(5U);

      REQUIRE_CALL(apa, get_importance())
        .RETURN(crpcut::tag::critical);

      MAKE_TAG(katt);
      ALLOW_CALL(katt, num_passed())
        .RETURN(0U);

      ALLOW_CALL(katt, num_failed())
        .RETURN(3U);

      REQUIRE_CALL(katt, get_importance())
        .RETURN(crpcut::tag::non_critical);

      MAKE_TAG(a_very_long_tag_name_);
      ALLOW_CALL(a_very_long_tag_name_, num_passed())
        .RETURN(0U);

      ALLOW_CALL(a_very_long_tag_name_, num_failed())
        .RETURN(1U);

      REQUIRE_CALL(a_very_long_tag_name_, get_importance())
        .RETURN(crpcut::tag::critical);

      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        {
          crpcut::output::text_formatter obj(test_buffer,
                                             "one",
                                             vec,
                                             tags,
                                             9,9,
                                             test_modifier,
                                             no_char_conversion);
          obj.statistics(9, 9);
        }
        static const char re[] =
              "9 test cases selected\n"
              " tag                      run  passed  failed\n"
           "<F>!apa                        5       0       5<>\n"
          "<NF>?katt                       3       0       3<>\n"
           "<F>!a_very_long_tag_name_      1       0       1<>\n"
          "\n"
                               _ "Sum" _ "Critical" _ "Non-critical\n"
          "<FS>FAILED"   _ ":" _ "9"   _ "6"        _ "3<>\n$"
          ;

        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m),
                    test_buffer.os.str());
      }
    }

    TEST(stats_with_mixed_crit_ncrit_fail_pass_only_shows_crit_sum, fix)
    {
      ALLOW_CALL(tags, longest_tag_name()).RETURN(10U);

      REQUIRE_CALL(tags, num_passed()).RETURN(0U);
      REQUIRE_CALL(tags, num_failed()).RETURN(0U);
      REQUIRE_CALL(tags, get_importance())
        .RETURN(crpcut::tag::critical);

      MAKE_TAG(apa);
      ALLOW_CALL(apa, num_passed())
        .RETURN(2U);

      ALLOW_CALL(apa, num_failed())
        .RETURN(3U);

      REQUIRE_CALL(apa, get_importance())
        .RETURN(crpcut::tag::critical);

      MAKE_TAG(katt);
      ALLOW_CALL(katt, num_passed())
        .RETURN(1U);

      ALLOW_CALL(katt, num_failed())
        .RETURN(2U);

      REQUIRE_CALL(katt, get_importance())
        .RETURN(crpcut::tag::non_critical);

      MAKE_TAG(a_tag_name);
      ALLOW_CALL(a_tag_name, num_passed())
        .RETURN(1U);

      ALLOW_CALL(a_tag_name, num_failed())
        .RETURN(1U);

      REQUIRE_CALL(a_tag_name, get_importance())
        .RETURN(crpcut::tag::critical);

      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        {
          crpcut::output::text_formatter obj(test_buffer,
                                             "one",
                                             vec,
                                             tags,
                                             10,10,
                                             test_modifier,
                                             no_char_conversion);
          obj.statistics(10, 6);
        }
        static const char re[] =
              "10 test cases selected\n"
              " tag            run  passed  failed\n"
           "<F>!apa              5       2       3<>\n"
          "<NF>?katt             3       1       2<>\n"
           "<F>!a_tag_name       2       1       1<>\n"
          "\n"
                               _ "Sum" _ "Critical" _ "Non-critical\n"
          "<PS>PASSED"   _ ":" _ "4"   _ "3"        _ "1<>\n"
          "<FS>FAILED"   _ ":" _ "6"   _ "4"        _ "2<>\n$"
          ;
        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m),
                    test_buffer.os.str());
      }
    }

    TEST(stats_with_mixed_nconly_shows_nc_sum, fix)
    {
      ALLOW_CALL(tags, longest_tag_name()).RETURN(20U);

      REQUIRE_CALL(tags, num_passed()).RETURN(0U);
      REQUIRE_CALL(tags, num_failed()).RETURN(0U);
      REQUIRE_CALL(tags, get_importance())
        .RETURN(crpcut::tag::critical);

      MAKE_TAG(apa);
      ALLOW_CALL(apa, num_passed())
        .RETURN(2U);

      ALLOW_CALL(apa, num_failed())
        .RETURN(3U);

      REQUIRE_CALL(apa, get_importance())
        .RETURN(crpcut::tag::non_critical);

      MAKE_TAG(katt);
      ALLOW_CALL(katt, num_passed())
        .RETURN(1U);

      ALLOW_CALL(katt, num_failed())
        .RETURN(2U);

      REQUIRE_CALL(katt, get_importance())
        .RETURN(crpcut::tag::non_critical);

      MAKE_TAG(a_very_long_tag_name);
      ALLOW_CALL(a_very_long_tag_name, num_passed())
        .RETURN(1U);

      ALLOW_CALL(a_very_long_tag_name, num_failed())
        .RETURN(1U);

      REQUIRE_CALL(a_very_long_tag_name, get_importance())
        .RETURN(crpcut::tag::non_critical);

      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        {
          crpcut::output::text_formatter obj(test_buffer,
                                             "one",
                                             vec,
                                             tags,
                                             10,10,
                                             test_modifier,
                                             no_char_conversion);
          obj.statistics(10, 6);
        }
        static const char re[] =
              "10 test cases selected\n"
              " tag                      run  passed  failed\n"
          "<NF>?apa                        5       2       3<>\n"
          "<NF>?katt                       3       1       2<>\n"
          "<NF>?a_very_long_tag_name       2       1       1<>\n"
          "\n"
                                _ "Sum" _ "Critical" _ "Non-critical\n"
          "<NPS>PASSED"   _ ":" _ "4"   _ "0"        _ "4<>\n"
          "<NFS>FAILED"   _ ":" _ "6"   _ "0"        _ "6<>\n$"
          ;
        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m),
                    test_buffer.os.str());
      }
    }


    TEST(tags_that_are_not_run_are_not_displayed_in_tag_result_summary, fix)
    {
      ALLOW_CALL(tags, longest_tag_name()).RETURN(20U);

      REQUIRE_CALL(tags, num_passed()).RETURN(0U);
      REQUIRE_CALL(tags, num_failed()).RETURN(0U);
      REQUIRE_CALL(tags, get_importance())
        .RETURN(crpcut::tag::critical);

      MAKE_TAG(apa);
      ALLOW_CALL(apa, num_passed())
        .RETURN(2U);

      ALLOW_CALL(apa, num_failed())
        .RETURN(3U);

      REQUIRE_CALL(apa, get_importance())
        .RETURN(crpcut::tag::non_critical);

      MAKE_TAG(katt);
      ALLOW_CALL(katt, num_passed())
        .RETURN(1U);

      ALLOW_CALL(katt, num_failed())
        .RETURN(2U);

      REQUIRE_CALL(katt, get_importance())
        .RETURN(crpcut::tag::non_critical);

      MAKE_TAG(a_very_long_tag_name);
      ALLOW_CALL(a_very_long_tag_name, num_passed())
        .RETURN(0U);

      ALLOW_CALL(a_very_long_tag_name, num_failed())
        .RETURN(0U);

      REQUIRE_CALL(a_very_long_tag_name, get_importance())
        .RETURN(crpcut::tag::non_critical);

      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        {
          crpcut::output::text_formatter obj(test_buffer,
                                             "one",
                                             vec,
                                             tags,
                                             8,8,
                                             test_modifier,
                                             no_char_conversion);
          obj.statistics(8, 5);
        }
        static const char re[] =
              "8 test cases selected\n"
              " tag                      run  passed  failed\n"
          "<NF>?apa                        5       2       3<>\n"
          "<NF>?katt                       3       1       2<>\n"
          "\n"
                                _ "Sum" _ "Critical" _ "Non-critical\n"
          "<NPS>PASSED"   _ ":" _ "3"   _ "0"        _ "3<>\n"
          "<NFS>FAILED"   _ ":" _ "5"   _ "0"        _ "5<>\n$"
          ;
        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m),
                    test_buffer.os.str());
      }
    }


    TEST(passed_critical_has_correct_decoration, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           1,1,
                                           test_modifier,
                                           no_char_conversion);
        obj.begin_case("apa", true, true, 100);
        obj.end_case();
        ASSERT_PRED(crpcut::regex("^<P>PASSED!: apa\n<>=*\n"),
                    test_buffer.os.str());
      }
    }

    TEST(passed_noncritical_has_correct_decoration, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           1,1,
                                           test_modifier,
                                           no_char_conversion);
        obj.begin_case("apa", true, false, 100);
        obj.end_case();
        ASSERT_PRED(crpcut::regex("^<NP>PASSED?: apa\n<>=*\n"),
                    test_buffer.os.str());
      }
    }

    TEST(failed_critical_has_correct_decoration, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           1,1,
                                           test_modifier,
                                           no_char_conversion);
        obj.begin_case("apa", false, true, 100);
        obj.end_case();
        ASSERT_PRED(crpcut::regex("^<F>FAILED!: apa\n<>=*\n"),
                    test_buffer.os.str());
      }
    }

    TEST(failed_noncritical_has_correct_decoration, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           1,1,
                                           test_modifier,
                                           no_char_conversion);
        obj.begin_case("apa", false, false, 100);
        obj.end_case();
        ASSERT_PRED(crpcut::regex("^<NF>FAILED?: apa\n<>=*\n"),
                    test_buffer.os.str());
      }
    }

    TEST(terminate_without_files_with_correct_phase_and_format, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           1,1,
                                           test_modifier,
                                           no_char_conversion);
        obj.begin_case("apa", false, false, 100);
        obj.terminate(crpcut::destroying, s("katt"), s("apa.cpp"), "");
        obj.end_case();
        const char re[] =
          "^<NF>FAILED?: apa\n"
          "phase=\"destroying\"" _ "-*\n"
          "apa.cpp\n"
          "katt\n"
          "-*\n"
          "<>=*\n"
          ;
        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m), test_buffer.os.str());
      }
    }

    TEST(terminate_with_files_with_correct_phase_and_format, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           1,1,
                                           test_modifier,
                                           no_char_conversion);
        obj.begin_case("apa", false, false, 100);
        obj.terminate(crpcut::creating, s("katt"), s("apa.cpp"), "/tmp/tmpfile");
        obj.end_case();
        const char re[] =
          "^<NF>FAILED?: apa\n"
          "/tmp/tmpfile is not empty!\n"
          "phase=\"creating\"" _ "-*\n"
          "apa.cpp\n"
          "katt\n"
          "-*\n"
          "<>=*\n"
          ;
        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m), test_buffer.os.str());
      }
    }

    TEST(multiple_prints_are_shown_in_sequence, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           1,1,
                                           test_modifier,
                                           no_char_conversion);
        obj.begin_case("apa", false, false, 100);
        obj.print(s("type"),
                  s("the quick brown fox jumps over the lazy dog"),
                  s(""));
        obj.print(s("morse"),
                  s("jag missade hissen\ni mississippi"),
                  s("apa.cpp"));
        obj.terminate(crpcut::creating, s("katt"), s("apa.cpp"), "");
        obj.end_case();
        const char re[] =
          "^<NF>FAILED?: apa\n"
          "type-*\n"
          "the quick brown fox jumps over the lazy dog\n"
          "-*\n"
          "morse-*\n"
          "apa.cpp\n"
          "jag missade hissen\n"
          "i mississippi\n"
          "-*\n"
          "phase=\"creating\"" _ "-*\n"
          "apa.cpp\n"
          "katt\n"
          "-*\n"
          "<>=*\n"
          ;
        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m),
                    test_buffer.os.str());
      }
    }

    TEST(full_report_with_non_empty_dir, fix)
    {
      REQUIRE_CALL(tags, longest_tag_name()).RETURN(0U);
      ALLOW_CALL(tags, get_importance())
        .RETURN(crpcut::tag::critical);
      ALLOW_CALL(tags, num_passed())
        .RETURN(29U);
      ALLOW_CALL(tags, num_failed())
        .RETURN(1U);
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           32,32,
                                           test_modifier,
                                           no_char_conversion);
        obj.begin_case("apa", false, false, 100);
        obj.terminate(crpcut::creating, s("katt"), s("apa.cpp"), "/tmp/tmpdir/hoppla");
        obj.end_case();

        obj.nonempty_dir("/tmp/tmpdir");
        obj.blocked_test(crpcut::tag::critical, "ko");
        obj.blocked_test(crpcut::tag::disabled, "tupp");
        obj.statistics(30, 1);
        const char re[] =
          "^<NF>FAILED?: apa\n"
          "/tmp/tmpdir/hoppla is not empty!\n"
          "phase=\"creating\"" _ "-*\n"
          "apa.cpp\n"
          "katt\n"
          "-*\n"
          "<>=*\n"
          "Files remain under /tmp/tmpdir\n"
          "The following tests were blocked from running:\n"
          "  !<B>ko<>\n"
          "  -<B>tupp<>\n"
          "32 test cases selected\n\n"
                               _ "Sum" _ "Critical" _ "Non-critical\n"
          "<PS>PASSED" _   ":" _  "29" _       "29" _            "0<>\n"
          "<FS>FAILED" _   ":" _   "1" _        "1" _            "0<>\n"
          "<BS>UNTESTED" _ ":" _ "2<>\n"
          ;
        ASSERT_PRED(crpcut::regex(re, crpcut::regex::m), test_buffer.os.str());
      }
    }

    TEST(replace_illegal_chars_when_output_is_ASCII, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           1,1,
                                           test_modifier,
                                           "ASCII");
        obj.begin_case("apa", false, false, 100);

        char buff[255];
        for (size_t i = 0; i < sizeof(buff); ++i)
          {
            buff[i] = char(i + 1);
          }

        obj.print(s("info"),
                  crpcut::datatypes::fixed_string::make(buff, 255),
                  crpcut::datatypes::fixed_string::make(""));
        obj.terminate(crpcut::creating, s("katt"), s("apa.cpp"), "");
        obj.end_case();
        const char pattern[] =
          "<NF>FAILED?: apa\n"
          "info---------------------------------------------------------------------------\n"
          "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
          "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
          " !\"#$%&'()*+,-./0123456789:;<=>?"
          "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
          "`abcdefghijklmnopqrstuvwxyz{|}~\x7f"
          "................................................................"
          "................................................................\n"
          "-------------------------------------------------------------------------------\n"
          "phase=\"creating\"  -------------------------------------------------------------\n"
          "apa.cpp\n"
          "katt\n"
          "-------------------------------------------------------------------------------\n"
          "<>===============================================================================\n"
          ;
        std::string str = test_buffer.os.str();
        std::ostringstream os;
        std::ostringstream chars;
        for (size_t i = 0; i < str.length(); ++i)
          {
            if ((i & 15) == 0) { os << std::setw(3) << std::dec << i; }
            chars << (isprint(str[i]) ? str[i] : '.');
            os << ' ' << std::hex << std::setw(2) << std::setfill('0')
               << unsigned((unsigned char)str[i]);
            if ((i & 15) == 15)
              {
                os << "  " << chars.str() << '\n';
                chars.str(std::string());
              }
          }
        INFO << os.str();
        for (size_t i = 0; i < sizeof(pattern); ++i)
          {
            if (pattern[i] != str[i])
              {
                FAIL << "i=" << i << " str[i]=" << str[i] << " pattern[i]=" << pattern[i];
              }
          }
      }
    }

    TEST(verbatim_copy_of_all_chars_when_output_is_not_defined, fix)
    {
      ASSERT_SCOPE_HEAP_LEAK_FREE
      {
        mock::stream_buffer test_buffer;
        crpcut::output::text_formatter obj(test_buffer,
                                           "one",
                                           vec,
                                           tags,
                                           1,1,
                                           test_modifier,
                                           no_char_conversion);
        obj.begin_case("apa", false, false, 100);

        char buff[255];
        for (size_t i = 0; i < sizeof(buff); ++i)
          {
            buff[i] = char(i + 1);
          }

        obj.print(s("info"),
                  crpcut::datatypes::fixed_string::make(buff, 255),
                  crpcut::datatypes::fixed_string::make(""));
        obj.terminate(crpcut::creating, s("katt"), s("apa.cpp"), "");
        obj.end_case();
        const char pattern[] =
          "<NF>FAILED?: apa\n"
          "info---------------------------------------------------------------------------\n"
          "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
          "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
          " !\"#$%&'()*+,-./0123456789:;<=>?"
          "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
          "`abcdefghijklmnopqrstuvwxyz{|}~\x7f"
          "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
          "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
          "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
          "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
          "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
          "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
          "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
          "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff\n"
          "-------------------------------------------------------------------------------\n"
          "phase=\"creating\"  -------------------------------------------------------------\n"
          "apa.cpp\n"
          "katt\n"
          "-------------------------------------------------------------------------------\n"
          "<>===============================================================================\n"
          ;
        std::string str = test_buffer.os.str();
        std::ostringstream os;
        std::ostringstream chars;
        for (size_t i = 0; i < str.length(); ++i)
          {
            if ((i & 15) == 0) { os << std::setw(3) << std::dec << i; }
            chars << (isprint(str[i]) ? str[i] : '.');
            os << ' ' << std::hex << std::setw(2) << std::setfill('0')
               << unsigned((unsigned char)str[i]);
            if ((i & 15) == 15)
              {
                os << "  " << chars.str() << '\n';
                chars.str(std::string());
              }
          }
        INFO << os.str();
        for (size_t i = 0; i < sizeof(pattern); ++i)
          {
            if (pattern[i] != str[i])
              {
                FAIL << "i=" << i << " str[i]=" << str[i] << " pattern[i]=" << pattern[i];
              }
          }
      }
    }
  }
}
