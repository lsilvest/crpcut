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

#ifndef TEST_CASE_FACTORY_HPP_
#define TEST_CASE_FACTORY_HPP_

#include <crpcut.hpp>

namespace crpcut {

  class test_case_factory
  {
  public:
    static void set_charset(const char *set_name);
    static const char *get_charset();
    static const char *get_output_charset();
    static const char *get_illegal_rep();

    static int run_test(int argc, char *argv[],
                        std::ostream &os = std::cerr);
    static int run_test(int argc, const char *argv[],
                        std::ostream &os = std::cerr);
    static void introduce_name(pid_t pid, const char *name, size_t len);
    static void present(pid_t pid, comm::type t, test_phase phase,
                        size_t len, const char *buff);
    static bool tests_as_child_procs();
    static bool timeouts_enabled();
    static bool is_backtrace_enabled();
    static void set_deadline(crpcut_test_case_registrator *i);
    static void clear_deadline(crpcut_test_case_registrator *i);
    static void return_dir(unsigned num);
    static void test_succeeded(crpcut_test_case_registrator*);
    static const char *get_start_dir();
    static const char *get_parameter(const char *name);
    static bool is_naughty_child();
    static unsigned long calc_cputime(const struct timeval&);
  private:
    static test_case_factory& obj();
    test_case_factory();
    void list_tests(const char *const *names,
                    tag_list_root     &tags,
                    std::ostream      &os);
    void kill_presenter_process();
    void manage_children(std::size_t max_pending_children, poll<fdreader> &poller);
    void start_test(crpcut_test_case_registrator *i, poll<fdreader> &poller);

    int do_run(cli::interpreter *cli, std::ostream &os, tag_list_root &tags);
    void do_present(pid_t pid, comm::type t, test_phase phase,
                    size_t len, const char *buff);
    void do_introduce_name(pid_t pid, const char *name, size_t len);
    void do_set_deadline(crpcut_test_case_registrator *i);
    void do_clear_deadline(crpcut_test_case_registrator *i);
    void do_return_dir(unsigned num);
    const char *do_get_start_dir() const;
    void do_set_charset(const char *set_name);
    const char *do_get_charset() const;
    unsigned long do_calc_cputime(const struct timeval&);
    friend class crpcut_test_case_registrator;

    class registrator_list : public crpcut_test_case_registrator
    {
      virtual bool match_name(const char *) const { return false; }
      virtual std::ostream& print_name(std::ostream &os) const { return os; }
      virtual void crpcut_do_run_test_case() {}
      virtual tag& crpcut_tag() const { return crpcut_tag_info<crpcut::crpcut_none>::obj(); }
    };

    typedef buffer_vector<crpcut_test_case_registrator*> timeout_queue;

    cli::interpreter        *cli_;
    struct timeval           accumulated_cputime;
    pid_t                    current_pid;
    registrator_list         reg;
    unsigned                 pending_children;
    unsigned                 num_registered_tests;
    unsigned                 num_selected_tests;
    unsigned                 num_tests_run;
    unsigned                 num_successful_tests;
    int                      presenter_pipe;
    timeout_queue           *deadlines_;
    buffer_vector<unsigned> *working_dirs_;
    unsigned                 first_free_working_dir;
    char                     dirbase[PATH_MAX];
    char                     homedir[PATH_MAX];
    const char              *charset;
  };


}
#endif // TEST_CASE_FACTORY_HPP_