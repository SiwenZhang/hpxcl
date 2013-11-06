// Copyright (c)    2013 Martin Stumpf
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef HPX_OPENCL_PROGRAM_HPP__
#define HPX_OPENCL_PROGRAM_HPP__


#include <hpx/include/components.hpp>

#include "server/program.hpp"

namespace hpx {
namespace opencl { 


    class program
      : public hpx::components::client_base<
          program, hpx::components::stub_base<server::program>
        >
    {
    
        typedef hpx::components::client_base<
            program, hpx::components::stub_base<server::program>
            > base_type;

        public:
            // Empty constructor, necessary for hpx purposes
            program(){}

            // Constructor
            program(hpx::future<hpx::naming::id_type> const& gid)
              : base_type(gid)
            {}
            
            /////////////////////////////////////////////////
            /// Exposed Component functionality
            /// 
            
            // Build the Program, blocking
            void build();
            void build(std::string build_options);
            // Build the program, non-blocking
            hpx::lcos::future<void> build_async();
            hpx::lcos::future<void> build_async(std::string build_options);

    };

}}



#endif// HPX_OPENCL_BUFFER_HPP__
