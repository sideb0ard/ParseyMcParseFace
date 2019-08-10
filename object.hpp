#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "ast.hpp"

namespace object
{

const std::string NULL_OBJ = "NULL";
const std::string ERROR_OBJ = "ERROR";

const std::string INTEGER_OBJ = "INTEGER";
const std::string BOOLEAN_OBJ = "BOOLEAN";

const std::string RETURN_VALUE_OBJ = "RETURN_VALUE";

const std::string FUNCTION_OBJ = "FUNCTION";

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
    Boolean(bool val) : value_{val} {};
    ObjectType Type() override;
    std::string Inspect() override;

  public:
    bool value_;
};

class ReturnValue : public Object
{
  public:
    ReturnValue(std::shared_ptr<Object> val) : value_{val} {};
    ObjectType Type() override;
    std::string Inspect() override;

  public:
    std::shared_ptr<Object> value_;
};

class Null : public Object
{
  public:
    ObjectType Type() override;
    std::string Inspect() override;
};

class Error : public Object
{
  public:
    std::string message_;

  public:
    Error() = default;
    Error(std::string err_msg);
    ObjectType Type() override;
    std::string Inspect() override;
};

class Environment
{
  public:
    Environment() = default;
    ~Environment() = default;
    std::shared_ptr<Object> Get(std::string key);
    void Set(std::string key, std::shared_ptr<Object> val);

  private:
    std::unordered_map<std::string, std::shared_ptr<Object>> store_;
};

class Function : public Object
{
  public:
    Function(std::vector<std::shared_ptr<ast::Identifier>> parameters,
             std::shared_ptr<Environment> env,
             std::shared_ptr<ast::BlockStatement> body)
        : parameters_{parameters}, env_{env}, body_{body} {};
    ~Function() = default;
    ObjectType Type() override;
    std::string Inspect() override;

  public:
    std::vector<std::shared_ptr<ast::Identifier>> parameters_;
    std::shared_ptr<Environment> env_;
    std::shared_ptr<ast::BlockStatement> body_;
};

} // namespace object
