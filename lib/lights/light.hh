#pragma once

#include "node.hh"
#include <memory>
#include <vector>

namespace light {
class Light : public runner::Node {
public:
    Light(uint16_t start, uint16_t channels);

    virtual ~Light() = default;

    virtual void write_to_buffer(std::vector<uint8_t>& buffer) = 0;

    uint16_t start() const { return start_; }
    uint16_t channels() const { return channels_; }

protected:
    uint8_t cast(float value) const;

private:
    uint16_t start_;
    uint16_t channels_;
};

class Universe {
public:
    template <typename T, typename...Args>
    std::shared_ptr<Light> add_light(Args&&... args) {
        static_assert(std::is_base_of_v<Light, T>, "Lights must derive from base class Light");
        return lights_.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
    }

    std::vector<uint8_t> build_buffer() const;

    void set_max_channel(uint16_t channel);

private:
    uint16_t max_channel_ = 512;
    std::vector<std::shared_ptr<Light>> lights_;
};

}