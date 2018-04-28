#pragma once

#include <cstdint>
#include <vector>

namespace serial
{

//
// What get's written
//
using ByteVector_t = std::vector<uint8_t>;

//
// Abstract representation of a serial interface, only allows writing right now
//
class abstract_serial_interface
{
public: // deconstructor //////////////////////////////////////////////////////
    virtual ~abstract_serial_interface() = default;

public: // public methods /////////////////////////////////////////////////////
    //
    // write the given data out onto the serial connection
    //
    virtual bool write_data(ByteVector_t data) const = 0;
};

} // namespace serial
