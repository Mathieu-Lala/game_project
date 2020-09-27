// note : black box :
//  engine::Event -> Json
//  Json -> engine::Event

#include <nlohmann/json.hpp>
#include "Engine/details/overloaded.hpp"
#include "Engine/Event.hpp"

namespace nlohmann {

template<>
struct adl_serializer<std::chrono::steady_clock::duration> {

    static
    void to_json(nlohmann::json &j, const std::chrono::steady_clock::duration &duration)
    {
        j = nlohmann::json{{ "nanoseconds", std::chrono::nanoseconds{duration}.count() }};
    }

    static
    void from_json(const nlohmann::json &j, std::chrono::steady_clock::duration &duration)
    {
        std::uint64_t value = j.at("nanoseconds");
        duration = std::chrono::nanoseconds{value};
    }
};

} // namespace nlohmann

namespace engine {

template<typename EventType, typename... Param>
void serialize(nlohmann::json &j, const Param &...param)
{
    auto make_inner = [&] {
        nlohmann::json innerObj;
        std::size_t index = 0;

        (innerObj.emplace(EventType::elements[index++], param), ...);

        return innerObj;
    };

    nlohmann::json outerObj;
    outerObj.emplace(EventType::name, make_inner());
    j = outerObj;
}

template<typename EventType, typename... Param>
void deserialize(const nlohmann::json &j, Param &... param)
{
    // annoying conversion to string necessary for key lookup with .at?
    const auto &top = j.at(std::string{EventType::name});

    if (top.size() != sizeof...(Param)) {
        throw std::logic_error("Deserialization size mismatch");
    }

    std::size_t cur_elem = 0;
    (top.at(std::string{EventType::elements[cur_elem++]}).get_to(param), ...);
}



template<typename EventType>
void to_json(nlohmann::json &j, [[maybe_unused]] const EventType &)
    requires(EventType::elements.empty())
{
    serialize<EventType>(j);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event)
    requires(EventType::elements.size() == 1)
{
    const auto &[elem0] = event;
    serialize<EventType>(j, elem0);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event)
    requires(EventType::elements.size() == 2)
{
    const auto &[elem0, elem1] = event;
    serialize<EventType>(j, elem0, elem1);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event)
    requires(EventType::elements.size() == 3)
{
    const auto &[elem0, elem1, elem2] = event;
    serialize<EventType>(j, elem0, elem1, elem2);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event)
    requires(EventType::elements.size() == 4)
{
    const auto &[elem0, elem1, elem2, elem3] = event;
    serialize<EventType>(j, elem0, elem1, elem2, elem3);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event)
    requires(EventType::elements.size() == 5)
{
    const auto &[elem0, elem1, elem2, elem3, elem4] = event;
    serialize<EventType>(j, elem0, elem1, elem2, elem3, elem4);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event)
    requires(EventType::elements.size() == 6)
{
    const auto &[elem0, elem1, elem2, elem3, elem4, elem5] = event;
    serialize<EventType>(j, elem0, elem1, elem2, elem3, elem4, elem5);
}

template<typename EventType>
void from_json(const nlohmann::json &j, [[maybe_unused]] const EventType &)
    requires(EventType::elements.empty())
{
    deserialize<EventType>(j);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event)
    requires(EventType::elements.size() == 1)
{
    auto &[elem0] = event;
    deserialize<EventType>(j, elem0);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event)
    requires(EventType::elements.size() == 2)
{
    auto &[elem0, elem1] = event;
    deserialize<EventType>(j, elem0, elem1);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event)
    requires(EventType::elements.size() == 3)
{
    auto &[elem0, elem1, elem2] = event;
    deserialize<EventType>(j, elem0, elem1, elem2);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event)
    requires(EventType::elements.size() == 4)
{
    auto &[elem0, elem1, elem2, elem3] = event;
    deserialize<EventType>(j, elem0, elem1, elem2, elem3);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event)
    requires(EventType::elements.size() == 5)
{
    auto &[elem0, elem1, elem2, elem3, elem4] = event;
    deserialize<EventType>(j, elem0, elem1, elem2, elem3, elem4);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event)
    requires(EventType::elements.size() == 6)
{
    auto &[elem0, elem1, elem2, elem3, elem4, elem5] = event;
    deserialize<EventType>(j, elem0, elem1, elem2, elem3, elem4, elem5);
}


template<typename ... T>
void choose_variant(const nlohmann::json &j, std::variant<std::monostate, T...> &variant)
{
    bool matched = false;

    auto try_variant = [&]<typename Variant>(){
        if (!matched) {
            try {
                Variant obj{};
                from_json(j, obj);
                variant = obj;
                matched = true;
            } catch (const std::exception &) {}
        }
    };

    (try_variant.template operator()<T>(), ...);
}

inline
void from_json(const nlohmann::json &j, Event &event)
{
    choose_variant(j, event);
}

inline
void to_json(nlohmann::json &j, const Event &event)
{
    std::visit(overloaded{
        []([[maybe_unused]] const std::monostate &) {},
        [&j](const auto &e) { to_json(j, e); } },
        event);
}

} // namespace engine
