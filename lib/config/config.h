/*
 *  This file is generated with Embedded Proto, PLEASE DO NOT EDIT!
 *  source: config.proto
 */

// This file is generated. Please do not edit!
#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <MessageInterface.h>
#include <WireFormatter.h>
#include <Fields.h>
#include <MessageSizeCalculator.h>
#include <ReadBufferSection.h>
#include <RepeatedFieldFixedSize.h>
#include <FieldStringBytes.h>
#include <Errors.h>
#include <Defines.h>
#include <limits>

// Include external proto definitions

namespace config {

class AdderNode final: public ::EmbeddedProto::MessageInterface
{
  public:
    AdderNode() = default;
    AdderNode(const AdderNode& rhs )
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
    }

    AdderNode(const AdderNode&& rhs ) noexcept
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
    }

    ~AdderNode() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      SATURATING = 1,
      INPUTS = 2
    };

    AdderNode& operator=(const AdderNode& rhs)
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
      return *this;
    }

    AdderNode& operator=(const AdderNode&& rhs) noexcept
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
      return *this;
    }

    static constexpr char const* SATURATING_NAME = "saturating";
    inline void clear_saturating() { saturating_.clear(); }
    inline void set_saturating(const bool& value) { saturating_ = value; }
    inline void set_saturating(const bool&& value) { saturating_ = value; }
    inline bool& mutable_saturating() { return saturating_.get(); }
    inline const bool& get_saturating() const { return saturating_.get(); }
    inline bool saturating() const { return saturating_.get(); }

    static constexpr char const* INPUTS_NAME = "inputs";
    inline void clear_inputs() { inputs_.clear(); }
    inline void set_inputs(const uint32_t& value) { inputs_ = value; }
    inline void set_inputs(const uint32_t&& value) { inputs_ = value; }
    inline uint32_t& mutable_inputs() { return inputs_.get(); }
    inline const uint32_t& get_inputs() const { return inputs_.get(); }
    inline uint32_t inputs() const { return inputs_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((false != saturating_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = saturating_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SATURATING), buffer, false);
      }

      if((0U != inputs_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = inputs_.serialize_with_id(static_cast<uint32_t>(FieldNumber::INPUTS), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::SATURATING:
            return_value = saturating_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::INPUTS:
            return_value = inputs_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_saturating();
      clear_inputs();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::SATURATING:
          name = SATURATING_NAME;
          break;
        case FieldNumber::INPUTS:
          name = INPUTS_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = saturating_.to_string(left_chars, indent_level + 2, SATURATING_NAME, true);
      left_chars = inputs_.to_string(left_chars, indent_level + 2, INPUTS_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::boolean saturating_ = false;
      EmbeddedProto::uint32 inputs_ = 0U;

};

class Connection final: public ::EmbeddedProto::MessageInterface
{
  public:
    Connection() = default;
    Connection(const Connection& rhs )
    {
      set_from_node(rhs.get_from_node());
      set_from_output(rhs.get_from_output());
      set_to_node(rhs.get_to_node());
      set_to_input(rhs.get_to_input());
    }

    Connection(const Connection&& rhs ) noexcept
    {
      set_from_node(rhs.get_from_node());
      set_from_output(rhs.get_from_output());
      set_to_node(rhs.get_to_node());
      set_to_input(rhs.get_to_input());
    }

    ~Connection() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      FROM_NODE = 1,
      FROM_OUTPUT = 2,
      TO_NODE = 3,
      TO_INPUT = 4
    };

    Connection& operator=(const Connection& rhs)
    {
      set_from_node(rhs.get_from_node());
      set_from_output(rhs.get_from_output());
      set_to_node(rhs.get_to_node());
      set_to_input(rhs.get_to_input());
      return *this;
    }

    Connection& operator=(const Connection&& rhs) noexcept
    {
      set_from_node(rhs.get_from_node());
      set_from_output(rhs.get_from_output());
      set_to_node(rhs.get_to_node());
      set_to_input(rhs.get_to_input());
      return *this;
    }

    static constexpr char const* FROM_NODE_NAME = "from_node";
    inline void clear_from_node() { from_node_.clear(); }
    inline void set_from_node(const uint32_t& value) { from_node_ = value; }
    inline void set_from_node(const uint32_t&& value) { from_node_ = value; }
    inline uint32_t& mutable_from_node() { return from_node_.get(); }
    inline const uint32_t& get_from_node() const { return from_node_.get(); }
    inline uint32_t from_node() const { return from_node_.get(); }

    static constexpr char const* FROM_OUTPUT_NAME = "from_output";
    inline void clear_from_output() { from_output_.clear(); }
    inline void set_from_output(const uint32_t& value) { from_output_ = value; }
    inline void set_from_output(const uint32_t&& value) { from_output_ = value; }
    inline uint32_t& mutable_from_output() { return from_output_.get(); }
    inline const uint32_t& get_from_output() const { return from_output_.get(); }
    inline uint32_t from_output() const { return from_output_.get(); }

    static constexpr char const* TO_NODE_NAME = "to_node";
    inline void clear_to_node() { to_node_.clear(); }
    inline void set_to_node(const uint32_t& value) { to_node_ = value; }
    inline void set_to_node(const uint32_t&& value) { to_node_ = value; }
    inline uint32_t& mutable_to_node() { return to_node_.get(); }
    inline const uint32_t& get_to_node() const { return to_node_.get(); }
    inline uint32_t to_node() const { return to_node_.get(); }

    static constexpr char const* TO_INPUT_NAME = "to_input";
    inline void clear_to_input() { to_input_.clear(); }
    inline void set_to_input(const uint32_t& value) { to_input_ = value; }
    inline void set_to_input(const uint32_t&& value) { to_input_ = value; }
    inline uint32_t& mutable_to_input() { return to_input_.get(); }
    inline const uint32_t& get_to_input() const { return to_input_.get(); }
    inline uint32_t to_input() const { return to_input_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((0U != from_node_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = from_node_.serialize_with_id(static_cast<uint32_t>(FieldNumber::FROM_NODE), buffer, false);
      }

      if((0U != from_output_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = from_output_.serialize_with_id(static_cast<uint32_t>(FieldNumber::FROM_OUTPUT), buffer, false);
      }

      if((0U != to_node_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = to_node_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TO_NODE), buffer, false);
      }

      if((0U != to_input_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = to_input_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TO_INPUT), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::FROM_NODE:
            return_value = from_node_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::FROM_OUTPUT:
            return_value = from_output_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::TO_NODE:
            return_value = to_node_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::TO_INPUT:
            return_value = to_input_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_from_node();
      clear_from_output();
      clear_to_node();
      clear_to_input();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::FROM_NODE:
          name = FROM_NODE_NAME;
          break;
        case FieldNumber::FROM_OUTPUT:
          name = FROM_OUTPUT_NAME;
          break;
        case FieldNumber::TO_NODE:
          name = TO_NODE_NAME;
          break;
        case FieldNumber::TO_INPUT:
          name = TO_INPUT_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = from_node_.to_string(left_chars, indent_level + 2, FROM_NODE_NAME, true);
      left_chars = from_output_.to_string(left_chars, indent_level + 2, FROM_OUTPUT_NAME, false);
      left_chars = to_node_.to_string(left_chars, indent_level + 2, TO_NODE_NAME, false);
      left_chars = to_input_.to_string(left_chars, indent_level + 2, TO_INPUT_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::uint32 from_node_ = 0U;
      EmbeddedProto::uint32 from_output_ = 0U;
      EmbeddedProto::uint32 to_node_ = 0U;
      EmbeddedProto::uint32 to_input_ = 0U;

};

template<
    uint32_t ConstantNode_values_REP_LENGTH
>
class ConstantNode final: public ::EmbeddedProto::MessageInterface
{
  public:
    ConstantNode() = default;
    ConstantNode(const ConstantNode& rhs )
    {
      set_values(rhs.get_values());
    }

    ConstantNode(const ConstantNode&& rhs ) noexcept
    {
      set_values(rhs.get_values());
    }

    ~ConstantNode() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      VALUES = 1
    };

    ConstantNode& operator=(const ConstantNode& rhs)
    {
      set_values(rhs.get_values());
      return *this;
    }

    ConstantNode& operator=(const ConstantNode&& rhs) noexcept
    {
      set_values(rhs.get_values());
      return *this;
    }

    static constexpr char const* VALUES_NAME = "values";
    inline const EmbeddedProto::floatfixed& values(uint32_t index) const { return values_[index]; }
    inline void clear_values() { values_.clear(); }
    inline void set_values(uint32_t index, const EmbeddedProto::floatfixed& value) { values_.set(index, value); }
    inline void set_values(uint32_t index, const EmbeddedProto::floatfixed&& value) { values_.set(index, value); }
    inline void set_values(const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::floatfixed, ConstantNode_values_REP_LENGTH>& values) { values_ = values; }
    inline void add_values(const EmbeddedProto::floatfixed& value) { values_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::floatfixed, ConstantNode_values_REP_LENGTH>& mutable_values() { return values_; }
    inline EmbeddedProto::floatfixed& mutable_values(uint32_t index) { return values_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::floatfixed, ConstantNode_values_REP_LENGTH>& get_values() const { return values_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::floatfixed, ConstantNode_values_REP_LENGTH>& values() const { return values_; }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = values_.serialize_with_id(static_cast<uint32_t>(FieldNumber::VALUES), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::VALUES:
            return_value = values_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_values();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::VALUES:
          name = VALUES_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = values_.to_string(left_chars, indent_level + 2, VALUES_NAME, true);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::floatfixed, ConstantNode_values_REP_LENGTH> values_;

};

class DividerNode final: public ::EmbeddedProto::MessageInterface
{
  public:
    DividerNode() = default;
    DividerNode(const DividerNode& rhs )
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
    }

    DividerNode(const DividerNode&& rhs ) noexcept
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
    }

    ~DividerNode() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      SATURATING = 1,
      INPUTS = 2
    };

    DividerNode& operator=(const DividerNode& rhs)
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
      return *this;
    }

    DividerNode& operator=(const DividerNode&& rhs) noexcept
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
      return *this;
    }

    static constexpr char const* SATURATING_NAME = "saturating";
    inline void clear_saturating() { saturating_.clear(); }
    inline void set_saturating(const bool& value) { saturating_ = value; }
    inline void set_saturating(const bool&& value) { saturating_ = value; }
    inline bool& mutable_saturating() { return saturating_.get(); }
    inline const bool& get_saturating() const { return saturating_.get(); }
    inline bool saturating() const { return saturating_.get(); }

    static constexpr char const* INPUTS_NAME = "inputs";
    inline void clear_inputs() { inputs_.clear(); }
    inline void set_inputs(const uint32_t& value) { inputs_ = value; }
    inline void set_inputs(const uint32_t&& value) { inputs_ = value; }
    inline uint32_t& mutable_inputs() { return inputs_.get(); }
    inline const uint32_t& get_inputs() const { return inputs_.get(); }
    inline uint32_t inputs() const { return inputs_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((false != saturating_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = saturating_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SATURATING), buffer, false);
      }

      if((0U != inputs_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = inputs_.serialize_with_id(static_cast<uint32_t>(FieldNumber::INPUTS), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::SATURATING:
            return_value = saturating_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::INPUTS:
            return_value = inputs_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_saturating();
      clear_inputs();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::SATURATING:
          name = SATURATING_NAME;
          break;
        case FieldNumber::INPUTS:
          name = INPUTS_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = saturating_.to_string(left_chars, indent_level + 2, SATURATING_NAME, true);
      left_chars = inputs_.to_string(left_chars, indent_level + 2, INPUTS_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::boolean saturating_ = false;
      EmbeddedProto::uint32 inputs_ = 0U;

};

class MultiplierNode final: public ::EmbeddedProto::MessageInterface
{
  public:
    MultiplierNode() = default;
    MultiplierNode(const MultiplierNode& rhs )
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
    }

    MultiplierNode(const MultiplierNode&& rhs ) noexcept
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
    }

    ~MultiplierNode() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      SATURATING = 1,
      INPUTS = 2
    };

    MultiplierNode& operator=(const MultiplierNode& rhs)
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
      return *this;
    }

    MultiplierNode& operator=(const MultiplierNode&& rhs) noexcept
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
      return *this;
    }

    static constexpr char const* SATURATING_NAME = "saturating";
    inline void clear_saturating() { saturating_.clear(); }
    inline void set_saturating(const bool& value) { saturating_ = value; }
    inline void set_saturating(const bool&& value) { saturating_ = value; }
    inline bool& mutable_saturating() { return saturating_.get(); }
    inline const bool& get_saturating() const { return saturating_.get(); }
    inline bool saturating() const { return saturating_.get(); }

    static constexpr char const* INPUTS_NAME = "inputs";
    inline void clear_inputs() { inputs_.clear(); }
    inline void set_inputs(const uint32_t& value) { inputs_ = value; }
    inline void set_inputs(const uint32_t&& value) { inputs_ = value; }
    inline uint32_t& mutable_inputs() { return inputs_.get(); }
    inline const uint32_t& get_inputs() const { return inputs_.get(); }
    inline uint32_t inputs() const { return inputs_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((false != saturating_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = saturating_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SATURATING), buffer, false);
      }

      if((0U != inputs_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = inputs_.serialize_with_id(static_cast<uint32_t>(FieldNumber::INPUTS), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::SATURATING:
            return_value = saturating_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::INPUTS:
            return_value = inputs_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_saturating();
      clear_inputs();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::SATURATING:
          name = SATURATING_NAME;
          break;
        case FieldNumber::INPUTS:
          name = INPUTS_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = saturating_.to_string(left_chars, indent_level + 2, SATURATING_NAME, true);
      left_chars = inputs_.to_string(left_chars, indent_level + 2, INPUTS_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::boolean saturating_ = false;
      EmbeddedProto::uint32 inputs_ = 0U;

};

class SubtractorNode final: public ::EmbeddedProto::MessageInterface
{
  public:
    SubtractorNode() = default;
    SubtractorNode(const SubtractorNode& rhs )
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
    }

    SubtractorNode(const SubtractorNode&& rhs ) noexcept
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
    }

    ~SubtractorNode() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      SATURATING = 1,
      INPUTS = 2
    };

    SubtractorNode& operator=(const SubtractorNode& rhs)
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
      return *this;
    }

    SubtractorNode& operator=(const SubtractorNode&& rhs) noexcept
    {
      set_saturating(rhs.get_saturating());
      set_inputs(rhs.get_inputs());
      return *this;
    }

    static constexpr char const* SATURATING_NAME = "saturating";
    inline void clear_saturating() { saturating_.clear(); }
    inline void set_saturating(const bool& value) { saturating_ = value; }
    inline void set_saturating(const bool&& value) { saturating_ = value; }
    inline bool& mutable_saturating() { return saturating_.get(); }
    inline const bool& get_saturating() const { return saturating_.get(); }
    inline bool saturating() const { return saturating_.get(); }

    static constexpr char const* INPUTS_NAME = "inputs";
    inline void clear_inputs() { inputs_.clear(); }
    inline void set_inputs(const uint32_t& value) { inputs_ = value; }
    inline void set_inputs(const uint32_t&& value) { inputs_ = value; }
    inline uint32_t& mutable_inputs() { return inputs_.get(); }
    inline const uint32_t& get_inputs() const { return inputs_.get(); }
    inline uint32_t inputs() const { return inputs_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((false != saturating_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = saturating_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SATURATING), buffer, false);
      }

      if((0U != inputs_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = inputs_.serialize_with_id(static_cast<uint32_t>(FieldNumber::INPUTS), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::SATURATING:
            return_value = saturating_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::INPUTS:
            return_value = inputs_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_saturating();
      clear_inputs();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::SATURATING:
          name = SATURATING_NAME;
          break;
        case FieldNumber::INPUTS:
          name = INPUTS_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = saturating_.to_string(left_chars, indent_level + 2, SATURATING_NAME, true);
      left_chars = inputs_.to_string(left_chars, indent_level + 2, INPUTS_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::boolean saturating_ = false;
      EmbeddedProto::uint32 inputs_ = 0U;

};

template<
    uint32_t UniverseNode_lights_REP_LENGTH, 
    uint32_t UniverseNode_lights_Light_name_LENGTH, 
    uint32_t UniverseNode_lights_Light_channels_REP_LENGTH
>
class UniverseNode final: public ::EmbeddedProto::MessageInterface
{
  public:
    UniverseNode() = default;
    UniverseNode(const UniverseNode& rhs )
    {
      set_lights(rhs.get_lights());
      set_max_channel(rhs.get_max_channel());
    }

    UniverseNode(const UniverseNode&& rhs ) noexcept
    {
      set_lights(rhs.get_lights());
      set_max_channel(rhs.get_max_channel());
    }

    ~UniverseNode() override = default;

    template<
        uint32_t Light_name_LENGTH, 
        uint32_t Light_channels_REP_LENGTH
    >
    class Light final: public ::EmbeddedProto::MessageInterface
    {
      public:
        Light() = default;
        Light(const Light& rhs )
        {
          set_name(rhs.get_name());
          set_channels(rhs.get_channels());
        }

        Light(const Light&& rhs ) noexcept
        {
          set_name(rhs.get_name());
          set_channels(rhs.get_channels());
        }

        ~Light() override = default;

        enum class FieldNumber : uint32_t
        {
          NOT_SET = 0,
          NAME = 1,
          CHANNELS = 2
        };

        Light& operator=(const Light& rhs)
        {
          set_name(rhs.get_name());
          set_channels(rhs.get_channels());
          return *this;
        }

        Light& operator=(const Light&& rhs) noexcept
        {
          set_name(rhs.get_name());
          set_channels(rhs.get_channels());
          return *this;
        }

        static constexpr char const* NAME_NAME = "name";
        inline void clear_name() { name_.clear(); }
        inline ::EmbeddedProto::FieldString<Light_name_LENGTH>& mutable_name() { return name_; }
        inline void set_name(const ::EmbeddedProto::FieldString<Light_name_LENGTH>& rhs) { name_.set(rhs); }
        inline const ::EmbeddedProto::FieldString<Light_name_LENGTH>& get_name() const { return name_; }
        inline const char* name() const { return name_.get_const(); }

        static constexpr char const* CHANNELS_NAME = "channels";
        inline const EmbeddedProto::uint32& channels(uint32_t index) const { return channels_[index]; }
        inline void clear_channels() { channels_.clear(); }
        inline void set_channels(uint32_t index, const EmbeddedProto::uint32& value) { channels_.set(index, value); }
        inline void set_channels(uint32_t index, const EmbeddedProto::uint32&& value) { channels_.set(index, value); }
        inline void set_channels(const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::uint32, Light_channels_REP_LENGTH>& values) { channels_ = values; }
        inline void add_channels(const EmbeddedProto::uint32& value) { channels_.add(value); }
        inline ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::uint32, Light_channels_REP_LENGTH>& mutable_channels() { return channels_; }
        inline EmbeddedProto::uint32& mutable_channels(uint32_t index) { return channels_[index]; }
        inline const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::uint32, Light_channels_REP_LENGTH>& get_channels() const { return channels_; }
        inline const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::uint32, Light_channels_REP_LENGTH>& channels() const { return channels_; }


        ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = name_.serialize_with_id(static_cast<uint32_t>(FieldNumber::NAME), buffer, false);
          }

          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = channels_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CHANNELS), buffer, false);
          }

          return return_value;
        };

        ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
          ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
          uint32_t id_number = 0;
          FieldNumber id_tag = FieldNumber::NOT_SET;

          ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
          while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
          {
            id_tag = static_cast<FieldNumber>(id_number);
            switch(id_tag)
            {
              case FieldNumber::NAME:
                return_value = name_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::CHANNELS:
                return_value = channels_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::NOT_SET:
                return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
                break;

              default:
                return_value = skip_unknown_field(buffer, wire_type);
                break;
            }

            if(::EmbeddedProto::Error::NO_ERRORS == return_value)
            {
              // Read the next tag.
              tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
            }
          }

          // When an error was detect while reading the tag but no other errors where found, set it in the return value.
          if((::EmbeddedProto::Error::NO_ERRORS == return_value)
             && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
             && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
          {
            return_value = tag_value;
          }

          return return_value;
        };

        void clear() override
        {
          clear_name();
          clear_channels();

        }

        static char const* field_number_to_name(const FieldNumber fieldNumber)
        {
          char const* name = nullptr;
          switch(fieldNumber)
          {
            case FieldNumber::NAME:
              name = NAME_NAME;
              break;
            case FieldNumber::CHANNELS:
              name = CHANNELS_NAME;
              break;
            default:
              name = "Invalid FieldNumber";
              break;
          }
          return name;
        }

    #ifdef MSG_TO_STRING

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
        {
          return this->to_string(str, 0, nullptr, true);
        }

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
        {
          ::EmbeddedProto::string_view left_chars = str;
          int32_t n_chars_used = 0;

          if(!first_field)
          {
            // Add a comma behind the previous field.
            n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
            if(0 < n_chars_used)
            {
              // Update the character pointer and characters left in the array.
              left_chars.data += n_chars_used;
              left_chars.size -= n_chars_used;
            }
          }

          if(nullptr != name)
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
            }
          }
          else
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
            }
          }
          
          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          left_chars = name_.to_string(left_chars, indent_level + 2, NAME_NAME, true);
          left_chars = channels_.to_string(left_chars, indent_level + 2, CHANNELS_NAME, false);
      
          if( 0 == indent_level) 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
          }
          else 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
          }

          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          return left_chars;
        }

    #endif // End of MSG_TO_STRING

      private:


          ::EmbeddedProto::FieldString<Light_name_LENGTH> name_;
          ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::uint32, Light_channels_REP_LENGTH> channels_;

    };

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      LIGHTS = 1,
      MAX_CHANNEL = 2
    };

    UniverseNode& operator=(const UniverseNode& rhs)
    {
      set_lights(rhs.get_lights());
      set_max_channel(rhs.get_max_channel());
      return *this;
    }

    UniverseNode& operator=(const UniverseNode&& rhs) noexcept
    {
      set_lights(rhs.get_lights());
      set_max_channel(rhs.get_max_channel());
      return *this;
    }

    static constexpr char const* LIGHTS_NAME = "lights";
    inline const Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>& lights(uint32_t index) const { return lights_[index]; }
    inline void clear_lights() { lights_.clear(); }
    inline void set_lights(uint32_t index, const Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>& value) { lights_.set(index, value); }
    inline void set_lights(uint32_t index, const Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>&& value) { lights_.set(index, value); }
    inline void set_lights(const ::EmbeddedProto::RepeatedFieldFixedSize<Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>, UniverseNode_lights_REP_LENGTH>& values) { lights_ = values; }
    inline void add_lights(const Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>& value) { lights_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>, UniverseNode_lights_REP_LENGTH>& mutable_lights() { return lights_; }
    inline Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>& mutable_lights(uint32_t index) { return lights_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>, UniverseNode_lights_REP_LENGTH>& get_lights() const { return lights_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>, UniverseNode_lights_REP_LENGTH>& lights() const { return lights_; }

    static constexpr char const* MAX_CHANNEL_NAME = "max_channel";
    inline void clear_max_channel() { max_channel_.clear(); }
    inline void set_max_channel(const uint32_t& value) { max_channel_ = value; }
    inline void set_max_channel(const uint32_t&& value) { max_channel_ = value; }
    inline uint32_t& mutable_max_channel() { return max_channel_.get(); }
    inline const uint32_t& get_max_channel() const { return max_channel_.get(); }
    inline uint32_t max_channel() const { return max_channel_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = lights_.serialize_with_id(static_cast<uint32_t>(FieldNumber::LIGHTS), buffer, false);
      }

      if((0U != max_channel_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = max_channel_.serialize_with_id(static_cast<uint32_t>(FieldNumber::MAX_CHANNEL), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::LIGHTS:
            return_value = lights_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::MAX_CHANNEL:
            return_value = max_channel_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_lights();
      clear_max_channel();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::LIGHTS:
          name = LIGHTS_NAME;
          break;
        case FieldNumber::MAX_CHANNEL:
          name = MAX_CHANNEL_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = lights_.to_string(left_chars, indent_level + 2, LIGHTS_NAME, true);
      left_chars = max_channel_.to_string(left_chars, indent_level + 2, MAX_CHANNEL_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      ::EmbeddedProto::RepeatedFieldFixedSize<Light<UniverseNode_lights_Light_name_LENGTH, UniverseNode_lights_Light_channels_REP_LENGTH>, UniverseNode_lights_REP_LENGTH> lights_;
      EmbeddedProto::uint32 max_channel_ = 0U;

};

template<
    uint32_t Node_name_LENGTH, 
    uint32_t Node_constant_node_ConstantNode_values_REP_LENGTH, 
    uint32_t Node_universe_node_UniverseNode_lights_REP_LENGTH, 
    uint32_t Node_universe_node_UniverseNode_lights_Light_name_LENGTH, 
    uint32_t Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH
>
class Node final: public ::EmbeddedProto::MessageInterface
{
  public:
    Node() = default;
    Node(const Node& rhs )
    {
      set_name(rhs.get_name());
      if(rhs.get_which_config() != which_config_)
      {
        // First delete the old object in the oneof.
        clear_config();
      }

      switch(rhs.get_which_config())
      {
        case FieldNumber::CONSTANT_NODE:
          set_constant_node(rhs.get_constant_node());
          break;

        case FieldNumber::ADDER_NODE:
          set_adder_node(rhs.get_adder_node());
          break;

        case FieldNumber::SUBTRACTOR_NODE:
          set_subtractor_node(rhs.get_subtractor_node());
          break;

        case FieldNumber::MULTIPLIER_NODE:
          set_multiplier_node(rhs.get_multiplier_node());
          break;

        case FieldNumber::DIVIDER_NODE:
          set_divider_node(rhs.get_divider_node());
          break;

        case FieldNumber::UNIVERSE_NODE:
          set_universe_node(rhs.get_universe_node());
          break;

        default:
          break;
      }

    }

    Node(const Node&& rhs ) noexcept
    {
      set_name(rhs.get_name());
      if(rhs.get_which_config() != which_config_)
      {
        // First delete the old object in the oneof.
        clear_config();
      }

      switch(rhs.get_which_config())
      {
        case FieldNumber::CONSTANT_NODE:
          set_constant_node(rhs.get_constant_node());
          break;

        case FieldNumber::ADDER_NODE:
          set_adder_node(rhs.get_adder_node());
          break;

        case FieldNumber::SUBTRACTOR_NODE:
          set_subtractor_node(rhs.get_subtractor_node());
          break;

        case FieldNumber::MULTIPLIER_NODE:
          set_multiplier_node(rhs.get_multiplier_node());
          break;

        case FieldNumber::DIVIDER_NODE:
          set_divider_node(rhs.get_divider_node());
          break;

        case FieldNumber::UNIVERSE_NODE:
          set_universe_node(rhs.get_universe_node());
          break;

        default:
          break;
      }

    }

    ~Node() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      NAME = 1,
      CONSTANT_NODE = 2,
      ADDER_NODE = 3,
      SUBTRACTOR_NODE = 4,
      MULTIPLIER_NODE = 5,
      DIVIDER_NODE = 6,
      UNIVERSE_NODE = 7
    };

    Node& operator=(const Node& rhs)
    {
      set_name(rhs.get_name());
      if(rhs.get_which_config() != which_config_)
      {
        // First delete the old object in the oneof.
        clear_config();
      }

      switch(rhs.get_which_config())
      {
        case FieldNumber::CONSTANT_NODE:
          set_constant_node(rhs.get_constant_node());
          break;

        case FieldNumber::ADDER_NODE:
          set_adder_node(rhs.get_adder_node());
          break;

        case FieldNumber::SUBTRACTOR_NODE:
          set_subtractor_node(rhs.get_subtractor_node());
          break;

        case FieldNumber::MULTIPLIER_NODE:
          set_multiplier_node(rhs.get_multiplier_node());
          break;

        case FieldNumber::DIVIDER_NODE:
          set_divider_node(rhs.get_divider_node());
          break;

        case FieldNumber::UNIVERSE_NODE:
          set_universe_node(rhs.get_universe_node());
          break;

        default:
          break;
      }

      return *this;
    }

    Node& operator=(const Node&& rhs) noexcept
    {
      set_name(rhs.get_name());
      if(rhs.get_which_config() != which_config_)
      {
        // First delete the old object in the oneof.
        clear_config();
      }

      switch(rhs.get_which_config())
      {
        case FieldNumber::CONSTANT_NODE:
          set_constant_node(rhs.get_constant_node());
          break;

        case FieldNumber::ADDER_NODE:
          set_adder_node(rhs.get_adder_node());
          break;

        case FieldNumber::SUBTRACTOR_NODE:
          set_subtractor_node(rhs.get_subtractor_node());
          break;

        case FieldNumber::MULTIPLIER_NODE:
          set_multiplier_node(rhs.get_multiplier_node());
          break;

        case FieldNumber::DIVIDER_NODE:
          set_divider_node(rhs.get_divider_node());
          break;

        case FieldNumber::UNIVERSE_NODE:
          set_universe_node(rhs.get_universe_node());
          break;

        default:
          break;
      }

      return *this;
    }

    static constexpr char const* NAME_NAME = "name";
    inline void clear_name() { name_.clear(); }
    inline ::EmbeddedProto::FieldString<Node_name_LENGTH>& mutable_name() { return name_; }
    inline void set_name(const ::EmbeddedProto::FieldString<Node_name_LENGTH>& rhs) { name_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<Node_name_LENGTH>& get_name() const { return name_; }
    inline const char* name() const { return name_.get_const(); }

    FieldNumber get_which_config() const { return which_config_; }

    static constexpr char const* CONSTANT_NODE_NAME = "constant_node";
    inline bool has_constant_node() const
    {
      return FieldNumber::CONSTANT_NODE == which_config_;
    }
    inline void clear_constant_node()
    {
      if(FieldNumber::CONSTANT_NODE == which_config_)
      {
        which_config_ = FieldNumber::NOT_SET;
        config_.constant_node_.~ConstantNode<Node_constant_node_ConstantNode_values_REP_LENGTH>();
      }
    }
    inline void set_constant_node(const ConstantNode<Node_constant_node_ConstantNode_values_REP_LENGTH>& value)
    {
      if(FieldNumber::CONSTANT_NODE != which_config_)
      {
        init_config(FieldNumber::CONSTANT_NODE);
      }
      config_.constant_node_ = value;
    }
    inline void set_constant_node(const ConstantNode<Node_constant_node_ConstantNode_values_REP_LENGTH>&& value)
    {
      if(FieldNumber::CONSTANT_NODE != which_config_)
      {
        init_config(FieldNumber::CONSTANT_NODE);
      }
      config_.constant_node_ = value;
    }
    inline ConstantNode<Node_constant_node_ConstantNode_values_REP_LENGTH>& mutable_constant_node()
    {
      if(FieldNumber::CONSTANT_NODE != which_config_)
      {
        init_config(FieldNumber::CONSTANT_NODE);
      }
      return config_.constant_node_;
    }
    inline const ConstantNode<Node_constant_node_ConstantNode_values_REP_LENGTH>& get_constant_node() const { return config_.constant_node_; }
    inline const ConstantNode<Node_constant_node_ConstantNode_values_REP_LENGTH>& constant_node() const { return config_.constant_node_; }

    static constexpr char const* ADDER_NODE_NAME = "adder_node";
    inline bool has_adder_node() const
    {
      return FieldNumber::ADDER_NODE == which_config_;
    }
    inline void clear_adder_node()
    {
      if(FieldNumber::ADDER_NODE == which_config_)
      {
        which_config_ = FieldNumber::NOT_SET;
        config_.adder_node_.~AdderNode();
      }
    }
    inline void set_adder_node(const AdderNode& value)
    {
      if(FieldNumber::ADDER_NODE != which_config_)
      {
        init_config(FieldNumber::ADDER_NODE);
      }
      config_.adder_node_ = value;
    }
    inline void set_adder_node(const AdderNode&& value)
    {
      if(FieldNumber::ADDER_NODE != which_config_)
      {
        init_config(FieldNumber::ADDER_NODE);
      }
      config_.adder_node_ = value;
    }
    inline AdderNode& mutable_adder_node()
    {
      if(FieldNumber::ADDER_NODE != which_config_)
      {
        init_config(FieldNumber::ADDER_NODE);
      }
      return config_.adder_node_;
    }
    inline const AdderNode& get_adder_node() const { return config_.adder_node_; }
    inline const AdderNode& adder_node() const { return config_.adder_node_; }

    static constexpr char const* SUBTRACTOR_NODE_NAME = "subtractor_node";
    inline bool has_subtractor_node() const
    {
      return FieldNumber::SUBTRACTOR_NODE == which_config_;
    }
    inline void clear_subtractor_node()
    {
      if(FieldNumber::SUBTRACTOR_NODE == which_config_)
      {
        which_config_ = FieldNumber::NOT_SET;
        config_.subtractor_node_.~SubtractorNode();
      }
    }
    inline void set_subtractor_node(const SubtractorNode& value)
    {
      if(FieldNumber::SUBTRACTOR_NODE != which_config_)
      {
        init_config(FieldNumber::SUBTRACTOR_NODE);
      }
      config_.subtractor_node_ = value;
    }
    inline void set_subtractor_node(const SubtractorNode&& value)
    {
      if(FieldNumber::SUBTRACTOR_NODE != which_config_)
      {
        init_config(FieldNumber::SUBTRACTOR_NODE);
      }
      config_.subtractor_node_ = value;
    }
    inline SubtractorNode& mutable_subtractor_node()
    {
      if(FieldNumber::SUBTRACTOR_NODE != which_config_)
      {
        init_config(FieldNumber::SUBTRACTOR_NODE);
      }
      return config_.subtractor_node_;
    }
    inline const SubtractorNode& get_subtractor_node() const { return config_.subtractor_node_; }
    inline const SubtractorNode& subtractor_node() const { return config_.subtractor_node_; }

    static constexpr char const* MULTIPLIER_NODE_NAME = "multiplier_node";
    inline bool has_multiplier_node() const
    {
      return FieldNumber::MULTIPLIER_NODE == which_config_;
    }
    inline void clear_multiplier_node()
    {
      if(FieldNumber::MULTIPLIER_NODE == which_config_)
      {
        which_config_ = FieldNumber::NOT_SET;
        config_.multiplier_node_.~MultiplierNode();
      }
    }
    inline void set_multiplier_node(const MultiplierNode& value)
    {
      if(FieldNumber::MULTIPLIER_NODE != which_config_)
      {
        init_config(FieldNumber::MULTIPLIER_NODE);
      }
      config_.multiplier_node_ = value;
    }
    inline void set_multiplier_node(const MultiplierNode&& value)
    {
      if(FieldNumber::MULTIPLIER_NODE != which_config_)
      {
        init_config(FieldNumber::MULTIPLIER_NODE);
      }
      config_.multiplier_node_ = value;
    }
    inline MultiplierNode& mutable_multiplier_node()
    {
      if(FieldNumber::MULTIPLIER_NODE != which_config_)
      {
        init_config(FieldNumber::MULTIPLIER_NODE);
      }
      return config_.multiplier_node_;
    }
    inline const MultiplierNode& get_multiplier_node() const { return config_.multiplier_node_; }
    inline const MultiplierNode& multiplier_node() const { return config_.multiplier_node_; }

    static constexpr char const* DIVIDER_NODE_NAME = "divider_node";
    inline bool has_divider_node() const
    {
      return FieldNumber::DIVIDER_NODE == which_config_;
    }
    inline void clear_divider_node()
    {
      if(FieldNumber::DIVIDER_NODE == which_config_)
      {
        which_config_ = FieldNumber::NOT_SET;
        config_.divider_node_.~DividerNode();
      }
    }
    inline void set_divider_node(const DividerNode& value)
    {
      if(FieldNumber::DIVIDER_NODE != which_config_)
      {
        init_config(FieldNumber::DIVIDER_NODE);
      }
      config_.divider_node_ = value;
    }
    inline void set_divider_node(const DividerNode&& value)
    {
      if(FieldNumber::DIVIDER_NODE != which_config_)
      {
        init_config(FieldNumber::DIVIDER_NODE);
      }
      config_.divider_node_ = value;
    }
    inline DividerNode& mutable_divider_node()
    {
      if(FieldNumber::DIVIDER_NODE != which_config_)
      {
        init_config(FieldNumber::DIVIDER_NODE);
      }
      return config_.divider_node_;
    }
    inline const DividerNode& get_divider_node() const { return config_.divider_node_; }
    inline const DividerNode& divider_node() const { return config_.divider_node_; }

    static constexpr char const* UNIVERSE_NODE_NAME = "universe_node";
    inline bool has_universe_node() const
    {
      return FieldNumber::UNIVERSE_NODE == which_config_;
    }
    inline void clear_universe_node()
    {
      if(FieldNumber::UNIVERSE_NODE == which_config_)
      {
        which_config_ = FieldNumber::NOT_SET;
        config_.universe_node_.~UniverseNode<Node_universe_node_UniverseNode_lights_REP_LENGTH, Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>();
      }
    }
    inline void set_universe_node(const UniverseNode<Node_universe_node_UniverseNode_lights_REP_LENGTH, Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>& value)
    {
      if(FieldNumber::UNIVERSE_NODE != which_config_)
      {
        init_config(FieldNumber::UNIVERSE_NODE);
      }
      config_.universe_node_ = value;
    }
    inline void set_universe_node(const UniverseNode<Node_universe_node_UniverseNode_lights_REP_LENGTH, Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>&& value)
    {
      if(FieldNumber::UNIVERSE_NODE != which_config_)
      {
        init_config(FieldNumber::UNIVERSE_NODE);
      }
      config_.universe_node_ = value;
    }
    inline UniverseNode<Node_universe_node_UniverseNode_lights_REP_LENGTH, Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>& mutable_universe_node()
    {
      if(FieldNumber::UNIVERSE_NODE != which_config_)
      {
        init_config(FieldNumber::UNIVERSE_NODE);
      }
      return config_.universe_node_;
    }
    inline const UniverseNode<Node_universe_node_UniverseNode_lights_REP_LENGTH, Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>& get_universe_node() const { return config_.universe_node_; }
    inline const UniverseNode<Node_universe_node_UniverseNode_lights_REP_LENGTH, Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>& universe_node() const { return config_.universe_node_; }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = name_.serialize_with_id(static_cast<uint32_t>(FieldNumber::NAME), buffer, false);
      }

      switch(which_config_)
      {
        case FieldNumber::CONSTANT_NODE:
          if(has_constant_node() && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = config_.constant_node_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CONSTANT_NODE), buffer, true);
          }
          break;

        case FieldNumber::ADDER_NODE:
          if(has_adder_node() && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = config_.adder_node_.serialize_with_id(static_cast<uint32_t>(FieldNumber::ADDER_NODE), buffer, true);
          }
          break;

        case FieldNumber::SUBTRACTOR_NODE:
          if(has_subtractor_node() && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = config_.subtractor_node_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SUBTRACTOR_NODE), buffer, true);
          }
          break;

        case FieldNumber::MULTIPLIER_NODE:
          if(has_multiplier_node() && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = config_.multiplier_node_.serialize_with_id(static_cast<uint32_t>(FieldNumber::MULTIPLIER_NODE), buffer, true);
          }
          break;

        case FieldNumber::DIVIDER_NODE:
          if(has_divider_node() && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = config_.divider_node_.serialize_with_id(static_cast<uint32_t>(FieldNumber::DIVIDER_NODE), buffer, true);
          }
          break;

        case FieldNumber::UNIVERSE_NODE:
          if(has_universe_node() && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = config_.universe_node_.serialize_with_id(static_cast<uint32_t>(FieldNumber::UNIVERSE_NODE), buffer, true);
          }
          break;

        default:
          break;
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::NAME:
            return_value = name_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CONSTANT_NODE:
          case FieldNumber::ADDER_NODE:
          case FieldNumber::SUBTRACTOR_NODE:
          case FieldNumber::MULTIPLIER_NODE:
          case FieldNumber::DIVIDER_NODE:
          case FieldNumber::UNIVERSE_NODE:
            return_value = deserialize_config(id_tag, buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_name();
      clear_config();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::NAME:
          name = NAME_NAME;
          break;
        case FieldNumber::CONSTANT_NODE:
          name = CONSTANT_NODE_NAME;
          break;
        case FieldNumber::ADDER_NODE:
          name = ADDER_NODE_NAME;
          break;
        case FieldNumber::SUBTRACTOR_NODE:
          name = SUBTRACTOR_NODE_NAME;
          break;
        case FieldNumber::MULTIPLIER_NODE:
          name = MULTIPLIER_NODE_NAME;
          break;
        case FieldNumber::DIVIDER_NODE:
          name = DIVIDER_NODE_NAME;
          break;
        case FieldNumber::UNIVERSE_NODE:
          name = UNIVERSE_NODE_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = name_.to_string(left_chars, indent_level + 2, NAME_NAME, true);
      left_chars = to_string_config(left_chars, indent_level + 2, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      ::EmbeddedProto::FieldString<Node_name_LENGTH> name_;

      FieldNumber which_config_ = FieldNumber::NOT_SET;
      union config
      {
        config() {}
        ~config() {}
        ConstantNode<Node_constant_node_ConstantNode_values_REP_LENGTH> constant_node_;
        AdderNode adder_node_;
        SubtractorNode subtractor_node_;
        MultiplierNode multiplier_node_;
        DividerNode divider_node_;
        UniverseNode<Node_universe_node_UniverseNode_lights_REP_LENGTH, Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH> universe_node_;
      };
      config config_;

      void init_config(const FieldNumber field_id)
      {
        if(FieldNumber::NOT_SET != which_config_)
        {
          // First delete the old object in the oneof.
          clear_config();
        }

        // C++11 unions only support nontrivial members when you explicitly call the placement new statement.
        switch(field_id)
        {
          case FieldNumber::CONSTANT_NODE:
            new(&config_.constant_node_) ConstantNode<Node_constant_node_ConstantNode_values_REP_LENGTH>;
            break;

          case FieldNumber::ADDER_NODE:
            new(&config_.adder_node_) AdderNode;
            break;

          case FieldNumber::SUBTRACTOR_NODE:
            new(&config_.subtractor_node_) SubtractorNode;
            break;

          case FieldNumber::MULTIPLIER_NODE:
            new(&config_.multiplier_node_) MultiplierNode;
            break;

          case FieldNumber::DIVIDER_NODE:
            new(&config_.divider_node_) DividerNode;
            break;

          case FieldNumber::UNIVERSE_NODE:
            new(&config_.universe_node_) UniverseNode<Node_universe_node_UniverseNode_lights_REP_LENGTH, Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>;
            break;

          default:
            break;
         }

         which_config_ = field_id;
      }

      void clear_config()
      {
        switch(which_config_)
        {
          case FieldNumber::CONSTANT_NODE:
            ::EmbeddedProto::destroy_at(&config_.constant_node_);
            break;
          case FieldNumber::ADDER_NODE:
            ::EmbeddedProto::destroy_at(&config_.adder_node_);
            break;
          case FieldNumber::SUBTRACTOR_NODE:
            ::EmbeddedProto::destroy_at(&config_.subtractor_node_);
            break;
          case FieldNumber::MULTIPLIER_NODE:
            ::EmbeddedProto::destroy_at(&config_.multiplier_node_);
            break;
          case FieldNumber::DIVIDER_NODE:
            ::EmbeddedProto::destroy_at(&config_.divider_node_);
            break;
          case FieldNumber::UNIVERSE_NODE:
            ::EmbeddedProto::destroy_at(&config_.universe_node_);
            break;
          default:
            break;
        }
        which_config_ = FieldNumber::NOT_SET;
      }

      ::EmbeddedProto::Error deserialize_config(const FieldNumber field_id, 
                                    ::EmbeddedProto::ReadBufferInterface& buffer,
                                    const ::EmbeddedProto::WireFormatter::WireType wire_type)
      {
        ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
        
        if(field_id != which_config_)
        {
          init_config(field_id);
        }

        switch(which_config_)
        {
          case FieldNumber::CONSTANT_NODE:
            return_value = config_.constant_node_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::ADDER_NODE:
            return_value = config_.adder_node_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::SUBTRACTOR_NODE:
            return_value = config_.subtractor_node_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::MULTIPLIER_NODE:
            return_value = config_.multiplier_node_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::DIVIDER_NODE:
            return_value = config_.divider_node_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::UNIVERSE_NODE:
            return_value = config_.universe_node_.deserialize_check_type(buffer, wire_type);
            break;
          default:
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS != return_value)
        {
          clear_config();
        }
        return return_value;
      }

#ifdef MSG_TO_STRING 
      ::EmbeddedProto::string_view to_string_config(::EmbeddedProto::string_view& str, const uint32_t indent_level, const bool first_field) const
      {
        ::EmbeddedProto::string_view left_chars = str;

        switch(which_config_)
        {
          case FieldNumber::CONSTANT_NODE:
            left_chars = config_.constant_node_.to_string(left_chars, indent_level, CONSTANT_NODE_NAME, first_field);
            break;
          case FieldNumber::ADDER_NODE:
            left_chars = config_.adder_node_.to_string(left_chars, indent_level, ADDER_NODE_NAME, first_field);
            break;
          case FieldNumber::SUBTRACTOR_NODE:
            left_chars = config_.subtractor_node_.to_string(left_chars, indent_level, SUBTRACTOR_NODE_NAME, first_field);
            break;
          case FieldNumber::MULTIPLIER_NODE:
            left_chars = config_.multiplier_node_.to_string(left_chars, indent_level, MULTIPLIER_NODE_NAME, first_field);
            break;
          case FieldNumber::DIVIDER_NODE:
            left_chars = config_.divider_node_.to_string(left_chars, indent_level, DIVIDER_NODE_NAME, first_field);
            break;
          case FieldNumber::UNIVERSE_NODE:
            left_chars = config_.universe_node_.to_string(left_chars, indent_level, UNIVERSE_NODE_NAME, first_field);
            break;
          default:
            break;
        }

        return left_chars;
      }

#endif // End of MSG_TO_STRING
};

template<
    uint32_t Runner_nodes_REP_LENGTH, 
    uint32_t Runner_nodes_Node_name_LENGTH, 
    uint32_t Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, 
    uint32_t Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, 
    uint32_t Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, 
    uint32_t Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH, 
    uint32_t Runner_connections_REP_LENGTH
>
class Runner final: public ::EmbeddedProto::MessageInterface
{
  public:
    Runner() = default;
    Runner(const Runner& rhs )
    {
      set_nodes(rhs.get_nodes());
      set_connections(rhs.get_connections());
    }

    Runner(const Runner&& rhs ) noexcept
    {
      set_nodes(rhs.get_nodes());
      set_connections(rhs.get_connections());
    }

    ~Runner() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      NODES = 1,
      CONNECTIONS = 2
    };

    Runner& operator=(const Runner& rhs)
    {
      set_nodes(rhs.get_nodes());
      set_connections(rhs.get_connections());
      return *this;
    }

    Runner& operator=(const Runner&& rhs) noexcept
    {
      set_nodes(rhs.get_nodes());
      set_connections(rhs.get_connections());
      return *this;
    }

    static constexpr char const* NODES_NAME = "nodes";
    inline const Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>& nodes(uint32_t index) const { return nodes_[index]; }
    inline void clear_nodes() { nodes_.clear(); }
    inline void set_nodes(uint32_t index, const Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>& value) { nodes_.set(index, value); }
    inline void set_nodes(uint32_t index, const Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>&& value) { nodes_.set(index, value); }
    inline void set_nodes(const ::EmbeddedProto::RepeatedFieldFixedSize<Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>, Runner_nodes_REP_LENGTH>& values) { nodes_ = values; }
    inline void add_nodes(const Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>& value) { nodes_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>, Runner_nodes_REP_LENGTH>& mutable_nodes() { return nodes_; }
    inline Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>& mutable_nodes(uint32_t index) { return nodes_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>, Runner_nodes_REP_LENGTH>& get_nodes() const { return nodes_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>, Runner_nodes_REP_LENGTH>& nodes() const { return nodes_; }

    static constexpr char const* CONNECTIONS_NAME = "connections";
    inline const Connection& connections(uint32_t index) const { return connections_[index]; }
    inline void clear_connections() { connections_.clear(); }
    inline void set_connections(uint32_t index, const Connection& value) { connections_.set(index, value); }
    inline void set_connections(uint32_t index, const Connection&& value) { connections_.set(index, value); }
    inline void set_connections(const ::EmbeddedProto::RepeatedFieldFixedSize<Connection, Runner_connections_REP_LENGTH>& values) { connections_ = values; }
    inline void add_connections(const Connection& value) { connections_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<Connection, Runner_connections_REP_LENGTH>& mutable_connections() { return connections_; }
    inline Connection& mutable_connections(uint32_t index) { return connections_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<Connection, Runner_connections_REP_LENGTH>& get_connections() const { return connections_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<Connection, Runner_connections_REP_LENGTH>& connections() const { return connections_; }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = nodes_.serialize_with_id(static_cast<uint32_t>(FieldNumber::NODES), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = connections_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CONNECTIONS), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::NODES:
            return_value = nodes_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CONNECTIONS:
            return_value = connections_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_nodes();
      clear_connections();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::NODES:
          name = NODES_NAME;
          break;
        case FieldNumber::CONNECTIONS:
          name = CONNECTIONS_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = nodes_.to_string(left_chars, indent_level + 2, NODES_NAME, true);
      left_chars = connections_.to_string(left_chars, indent_level + 2, CONNECTIONS_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      ::EmbeddedProto::RepeatedFieldFixedSize<Node<Runner_nodes_Node_name_LENGTH, Runner_nodes_Node_constant_node_ConstantNode_values_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_REP_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_name_LENGTH, Runner_nodes_Node_universe_node_UniverseNode_lights_Light_channels_REP_LENGTH>, Runner_nodes_REP_LENGTH> nodes_;
      ::EmbeddedProto::RepeatedFieldFixedSize<Connection, Runner_connections_REP_LENGTH> connections_;

};

} // End of namespace config
#endif // CONFIG_H