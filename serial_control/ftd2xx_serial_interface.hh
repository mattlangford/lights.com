#pragma once

#include "serial_control/abstract_serial_interface.hh"
#include "third_party/ftd2xx_libs/ftd2xx.h"

#include <memory>
#include <vector>

namespace serial
{

//
// Connects to an FTDI serial connection and has some nice wrappers C++11 around the
// gross C
//
class ftd2xx_serial
{
public: // constructor ////////////////////////////////////////////////////////
    serial_connection(const size_t baudrate, const size_t device_number = 0);

    serial_connection(const serial_connection &s);

    ~serial_connection();

public: // public methods /////////////////////////////////////////////////////
    //
    // Simple c++11 wrapper to write some data and check that it went through.
    // Give it a byte vector that you don't care about since it'll eat it
    //
    bool write_data(ByteVector_t data) const;

private: // methods ///////////////////////////////////////////////////////////
    //
    // Makes sure the status return FT_OK
    //
    inline bool status_okay(const FT_STATUS ft_status) const;

private: // members ///////////////////////////////////////////////////////////

    //
    // Handle to the device
    //
    FT_HANDLE ft_handle;

};

} // namespace serial
