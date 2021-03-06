// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define _UNICODE
#define UNICODE
#define BOOST_TEST_MAIN
#define BOOST_TEST_IGNORE_SIGCHLD
#include <boost/test/included/unit_test.hpp>
#include <boost/process.hpp>
#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <string>

namespace bp = boost::process;
namespace bpi = boost::process::initializers;

BOOST_AUTO_TEST_CASE(set_cmd_line)
{
    using boost::unit_test::framework::master_test_suite;
    namespace bio = boost::iostreams;

    bp::pipe p = bp::create_pipe();

    {
        boost::filesystem::path exe = master_test_suite().argv[1];
        bio::file_descriptor_sink sink(p.sink, bio::close_handle);
        boost::system::error_code ec;
        bp::execute(
            bpi::run_exe(exe),
            bpi::set_cmd_line(L"test --echo-argv \"a b\""),
            bpi::bind_stdout(sink),
            bpi::set_on_error(ec)
        );
        BOOST_REQUIRE(!ec);
    }

    bio::file_descriptor_source source(p.source, bio::close_handle);
    bio::filtering_istream is;
    is.push(bio::newline_filter(bio::newline::posix));
    is.push(source);

    std::string s;
    std::getline(is, s);
    BOOST_CHECK_EQUAL(s, "\"test\" \"--echo-argv\" \"a b\"");
}
