#include "object.hpp"
#include <sstream>

namespace object
{

std::string Integer::Inspect()
{
    std::stringstream val;
    val << value_;
    return val.str();
}
ObjectType Integer::Type() { return INTEGER_OBJ; }

std::string Boolean::Inspect()
{
    std::stringstream val;
    val << (value_ ? "true" : "false");
    return val.str();
}
ObjectType Boolean::Type() { return BOOLEAN_OBJ; }

std::string Null::Inspect() { return "null"; }
ObjectType Null::Type() { return NULL_OBJ; }

std::string ReturnValue::Inspect() { return value_->Inspect(); }
ObjectType ReturnValue::Type() { return RETURN_VALUE_OBJ; }

Error::Error(std::string err_msg) : message_{err_msg} {}
std::string Error::Inspect() { return "ERROR: " + message_; }
ObjectType Error::Type() { return ERROR_OBJ; }

std::shared_ptr<Object> Environment::Get(std::string name)
{
    auto entry = store_.find(name);
    if (entry == store_.end())
        return nullptr;
    else
        return entry->second;
}

void Environment::Set(std::string key, std::shared_ptr<Object> val)
{
    store_[key] = val;
}

} // namespace object
