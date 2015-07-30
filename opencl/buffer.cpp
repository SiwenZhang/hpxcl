// Copyright (c)       2013 Martin Stumpf
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Header File
#include "buffer.hpp"

// Internal Dependencies
#include "server/buffer.hpp"

#include "lcos/event.hpp"

using hpx::opencl::buffer;


hpx::future<std::size_t>
buffer::size() const
{
    
    HPX_ASSERT(this->get_gid());
    typedef hpx::opencl::server::buffer::size_action func;

    return hpx::async<func>(this->get_gid());

}

buffer::send_result
buffer::enqueue_send_impl(
    const hpx::opencl::buffer &dst,
    std::size_t && src_offset,
    std::size_t && dst_offset,
    std::size_t && size,
    hpx::opencl::util::resolved_events && dependencies )
{
    using hpx::opencl::lcos::event;
    HPX_ASSERT(this->get_gid()); 
    HPX_ASSERT(dependencies.are_from_devices(device_gid, dst.device_gid));

    // create events
    event<void> src_event( device_gid );
    event<void> dst_event( dst.device_gid );
    
    // send command to server class
    if(!local_ptr){
        // is remote call
        typedef hpx::opencl::server::buffer::enqueue_send_action func;
        hpx::apply<func>( this->get_gid(),
                          dst.get_gid(),
                          src_event.get_event_id(),
                          dst_event.get_event_id(),
                          src_offset,
                          dst_offset,
                          size,
                          std::move(dependencies.event_ids),
                          std::move(dependencies.device_ids) ); 
    } else {
        // is local call

        hpx::apply(
            []( boost::shared_ptr<hpx::opencl::server::buffer> local_ptr,
                hpx::naming::id_type dst_id,
                hpx::naming::id_type && src_event_id,
                hpx::naming::id_type && dst_event_id,
                std::size_t src_offset,
                std::size_t dst_offset,
                std::size_t size,
                hpx::opencl::util::resolved_events && dependencies )
            {
                local_ptr->enqueue_send( dst_id,
                                         std::move(src_event_id),
                                         std::move(dst_event_id),
                                         src_offset,
                                         dst_offset,
                                         size,
                                         std::move(dependencies.event_ids),
                                         std::move(dependencies.device_ids) );
            },
            local_ptr,
            dst.get_gid(),
            src_event.get_event_id(),
            dst_event.get_event_id(),
            src_offset,
            dst_offset,
            size,
            std::move(dependencies));

    }

    // return futures
    return send_result( std::move(src_event.get_future()),
                        std::move(dst_event.get_future()) );
}


hpx::future<hpx::serialization::serialize_buffer<char> >
buffer::enqueue_read_impl(
    std::size_t && offset,
    std::size_t && size,
    hpx::opencl::util::resolved_events && dependencies )
{
    using hpx::opencl::lcos::event;
    typedef hpx::serialization::serialize_buffer<char> buffer_type;

    HPX_ASSERT(dependencies.are_from_device(device_gid));

    // create local event
    event<buffer_type> ev( device_gid );
   
    // send command to server class
    typedef hpx::opencl::server::buffer::enqueue_read_action func;
    hpx::apply<func>( this->get_gid(),
                      ev.get_event_id(),
                      offset,
                      size,
                      std::move(dependencies.event_ids) ); 

    // return future connected to event
    return ev.get_future();
}
