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

#include <gmock/gmock.h>
#include <crpcut.hpp>
#include "../poll.hpp"
#include "../registrator_list.hpp"

using testing::StrictMock;
namespace {

  class mock_poll : public crpcut::poll<crpcut::fdreader>
  {
  public:
    MOCK_METHOD3(do_add_fd, void(int, crpcut::fdreader*, int));
    MOCK_METHOD1(do_del_fd, void(int));
    MOCK_METHOD1(do_wait, descriptor(int));
    MOCK_CONST_METHOD0(do_num_fds, std::size_t());
  };

  typedef crpcut::crpcut_test_case_registrator reg;
  class mock_test_reg : public reg
  {
  public:
    typedef crpcut::tag::importance importance;
    mock_test_reg(const char *name, crpcut::namespace_info *ns)
    : reg(name, ns) {}
    MOCK_METHOD6(setup, void(crpcut::poll<crpcut::fdreader>&, pid_t,
                             int, int, int, int));
    MOCK_METHOD0(run_test_case, void());
    MOCK_CONST_METHOD0(crpcut_tag, crpcut::tag&());
    MOCK_CONST_METHOD0(get_importance, importance());
  };

  typedef crpcut::registrator_list list;

  struct fix
  {
    fix()
    : unnamed_namespace(0,0),
      reptil_namespace("reptil", &unnamed_namespace),
      insekt_namespace("insekt", &unnamed_namespace),
      apa("apa", &unnamed_namespace),
      katt("katt", &unnamed_namespace),
      ko("ko", &unnamed_namespace),
      reptil_orm("orm", &reptil_namespace),
      reptil_krokodil("krokodil", &reptil_namespace),
      insekt_mygga("mygga", &insekt_namespace),
      insekt_bi("bi", &insekt_namespace)
    {
      apa.link_before(the_list);
      katt.link_before(the_list);
      ko.link_before(the_list);
      reptil_orm.link_before(the_list);
      reptil_krokodil.link_before(the_list);
      insekt_mygga.link_before(the_list);
      insekt_bi.link_before(the_list);
      EXPECT_CALL(apa, get_importance()).
        WillRepeatedly(testing::Return(crpcut::tag::critical));
      EXPECT_CALL(katt, get_importance()).
        WillRepeatedly(testing::Return(crpcut::tag::ignored));
      EXPECT_CALL(ko, get_importance()).
        WillRepeatedly(testing::Return(crpcut::tag::disabled));
      EXPECT_CALL(reptil_orm, get_importance()).
        WillRepeatedly(testing::Return(crpcut::tag::non_critical));
      EXPECT_CALL(reptil_krokodil, get_importance()).
        WillRepeatedly(testing::Return(crpcut::tag::non_critical));
      EXPECT_CALL(insekt_mygga, get_importance()).
        WillRepeatedly(testing::Return(crpcut::tag::critical));
      EXPECT_CALL(insekt_bi, get_importance()).
        WillRepeatedly(testing::Return(crpcut::tag::critical));
    }
    list the_list;
    crpcut::namespace_info unnamed_namespace;
    crpcut::namespace_info reptil_namespace;
    crpcut::namespace_info insekt_namespace;
    StrictMock<mock_test_reg> apa;
    StrictMock<mock_test_reg> katt;
    StrictMock<mock_test_reg> ko;
    StrictMock<mock_test_reg> reptil_orm;
    StrictMock<mock_test_reg> reptil_krokodil;
    StrictMock<mock_test_reg> insekt_mygga;
    StrictMock<mock_test_reg> insekt_bi;
  };

  template <typename T, typename U>
  void match_and_remove(T& t, U u)
  {
    typename T::iterator i = std::find(t.begin(), t.end(), u);
    if (i == t.end()) FAIL << "Couldn't find " << *u;
    std::swap(*i, t.back());
    t.pop_back();
  }

  template <typename T>
  void match_all(T& t, list &r)
  {
    for (reg *i = r.next(); i != &r; i = i->next())
      {
        match_and_remove(t, i);
      }
    ASSERT_TRUE(t.size() == 0U);
  }
}

TESTSUITE(registrator_list)
{

  TEST(construction_does_nothing)
  {
    list l;
  }

  TESTSUITE(illegal_ops, DEPENDS_ON(construction_does_nothing))
  {
    TEST(run_test_case_aborts,
       DEPENDS_ON(construction_does_nothing),
       EXPECT_SIGNAL_DEATH(SIGABRT),
       NO_CORE_FILE)
    {
      list l;
      l.run_test_case();
    }

    TEST(crpcut_tag_aborts,
         DEPENDS_ON(construction_does_nothing),
         EXPECT_SIGNAL_DEATH(SIGABRT),
         NO_CORE_FILE)
    {
      list l;
      l.crpcut_tag();
    }

    TEST(get_importance_aborts,
         DEPENDS_ON(construction_does_nothing),
         EXPECT_SIGNAL_DEATH(SIGABRT),
         NO_CORE_FILE)
    {
      list l;
      l.get_importance();
    }

    TEST(setup_aborts,
         DEPENDS_ON(construction_does_nothing),
         EXPECT_SIGNAL_DEATH(SIGABRT),
         NO_CORE_FILE)
    {
      list l;
      StrictMock<mock_poll> poller;
      l.setup(poller, pid_t(1), 1,2,3,4);
    }
  }

  TESTSUITE(filtering)
  {
    TEST(empty_name_list_filters_out_ignored_only, fix)
    {
      std::ostringstream os;
      const char * const p = 0;
      std::pair<unsigned, unsigned> rv =
          the_list.filter_out_or_throw(&p, os, 1);
      ASSERT_TRUE(rv.first == 7U);
      ASSERT_TRUE(rv.second == 7U);
      std::vector<reg*> expected;
      expected.push_back(&apa);
      expected.push_back(&ko);
      expected.push_back(&reptil_orm);
      expected.push_back(&reptil_krokodil);
      expected.push_back(&insekt_mygga);
      expected.push_back(&insekt_bi);
      match_all(expected, the_list);
    }

    TEST(single_suite_name_picks_only_tests_in_that_suite, fix)
    {
      std::ostringstream os;
      const char *p[] = { "insekt", 0 };
      std::pair<unsigned, unsigned> rv =
          the_list.filter_out_or_throw(p, os, 1);
      ASSERT_TRUE(rv.first == 2U);
      ASSERT_TRUE(rv.second == 7U);
      std::vector<reg*> expected;
      expected.push_back(&insekt_mygga);
      expected.push_back(&insekt_bi);
      match_all(expected, the_list);
    }

    TEST(suite_and_test_picks_all_in_suite_plus_singled_out, fix)
    {
      std::ostringstream os;
      const char *p[] = { "insekt", "reptil::orm", 0 };
      std::pair<unsigned, unsigned> rv =
          the_list.filter_out_or_throw(p, os, 1);
      ASSERT_TRUE(rv.first == 3U);
      ASSERT_TRUE(rv.second == 7U);
      std::vector<reg*> expected;
      expected.push_back(&reptil_orm);
      expected.push_back(&insekt_mygga);
      expected.push_back(&insekt_bi);
      match_all(expected, the_list);
    }

    TEST(unmatched_name_throws_with_singularis_msg_form, fix)
    {
      std::ostringstream os;
      const char *p[] = { "insekt", "tupp", 0 };
      ASSERT_THROW(the_list.filter_out_or_throw(p, os, 1), int);
      ASSERT_TRUE(os.str() == "tupp does not match any test names\n");
    }

    TEST(unmatched_names_throws_with_pluralis_msg_form, fix)
    {
      std::ostringstream os;
      const char *p[] = { "reptil", "insekt", "tupp", "lemur", 0 };
      ASSERT_THROW(the_list.filter_out_or_throw(p, os, 1), int);
      ASSERT_TRUE(os.str() == "tupp, lemur do not match any test names\n");
    }
  }
}







