/*
 * Copyright 2009 Bjorn Fahller <bjorn@fahller.se>
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


#include <set>
#include <string>
#include <algorithm>

template <const char *(&locname)>
class sorted_names
{
  class comparator
  {
    typedef std::collate<char> coll_t;
  public:
    comparator() : loc(locname) {}
    bool operator()(const std::string &lh, const std::string &rh) const
    {
      const coll_t &coll = std::use_facet<coll_t>(loc);
      return coll.compare(lh.c_str(), lh.c_str()+lh.length(),
                          rh.c_str(), rh.c_str()+rh.length()) < 0;
    }
    std::locale loc;
  };
  typedef std::multiset<std::string, comparator> collection;
public:
  typedef typename collection::const_iterator iterator;
  void push(std::string name)
  {
    names.insert(name);
  }
  iterator begin() const
  {
    return names.begin();
  }
  iterator end()
  {
    return names.end();
  }
private:
  collection names;
};

