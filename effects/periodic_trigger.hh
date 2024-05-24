#pragma once

#include <vector>

class PeriodicBase;

class PeriodicManager {
public:
    void tick();
    void add(PeriodicBase* periodic);

private:
    std::vector<PeriodicBase*> periodics_;
};

PeriodicManager periodic;

class PeriodicBase {
public:
    PeriodicBase(uint32_t rate=1000) : rate_(rate) {
        periodic.add(this);
    }

    virtual ~PeriodicBase() = default;

    void tick(uint32_t now) {
        if (now >= next_ && enabled_) {
            trigger_ready(now);
            next_ += rate_;
        }
    }

    void set_enabled(bool enabled) { enabled_ = enabled; }

protected:
    void set_rate(uint32_t rate) { rate_ = rate; next_ = millis() + rate_; }
    uint32_t get_rate() const { return rate_; }

    virtual void trigger_ready(uint32_t now) = 0;

private:
    uint32_t rate_;
    uint32_t next_ = 0;
    bool enabled_ = true;
};

void PeriodicManager::tick() {
    uint32_t now = millis();
    for (auto& p : periodics_) {
        p->tick(now);
    }
}

void PeriodicManager::add(PeriodicBase* periodic) {
    periodics_.push_back(periodic);
}
 
template <typename Effect>
class PeriodicTrigger final : public NestedEffect<Effect>, public PeriodicBase {
public:
    PeriodicTrigger(uint32_t rate=1000) : PeriodicBase(rate) { }

    ~PeriodicTrigger() override = default;

protected:
    SetConfigResult set_parent_config_json(const JsonObject& json) {
        SetConfigResult result = SetConfigResult::no_values_set();
        uint32_t rate = 0;
        if (result.maybe_set(json, "rate_ms", rate)) {
            set_rate(rate);
        }
        return result;
    }
    void get_parent_config_json(JsonObject& json) const { json["rate_ms"] = get_rate(); }

    String parent_type() const override { return "PeriodicTrigger"; }

    void trigger_ready(uint32_t now) override { this->trigger(now); }
};