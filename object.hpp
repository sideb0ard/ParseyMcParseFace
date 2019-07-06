#pragma once

#include <string>

namespace object
{

const std::string INTEGER_OBJ = "INTEGER";
const std::string BOOLEAN_OBJ = "BOOLEAN";
const std::string NULL_OBJ = "NULL";

using ObjectType = std::string;

class Object
{
  public:
    virtual ~Object() = default;
    virtual ObjectType Type() = 0;
    virtual std::string Inspect() = 0;
};

class Integer : public Object
{
  public:
    Integer(int64_t val) : value_{val} {};
    ObjectType Type() override;
    std::string Inspect() override;

  public:
    int64_t value_;
};

class Boolean : public Object
{
  public:
    ObjectType Type() override;
    std::string Inspect() override;

  public:
    bool value_;
};

class Null : public Object
{
  public:
    ObjectType Type() override;
    std::string Inspect() override;
};

} // namespace object
