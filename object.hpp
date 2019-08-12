#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast.hpp"

namespace object
{

const char NULL_OBJ[] = "NULL";
const char ERROR_OBJ[] = "ERROR";

const char INTEGER_OBJ[] = "INTEGER";
const char BOOLEAN_OBJ[] = "BOOLEAN";

const char RETURN_VALUE_OBJ[] = "RETURN_VALUE";

const char FUNCTION_OBJ[] = "FUNCTION";

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
    explicit Integer(int64_t val) : value_{val} {};
    ObjectType Type() override;
    std::string Inspect() override;

  public:
    int64_t value_;
};

class Boolean : public Object
{
  public:
    explicit Boolean(bool val) : value_{val} {};
    ObjectType Type() override;
    std::string Inspect() override;

  public:
    bool value_;
};

class ReturnValue : public Object
{
  public:
    explicit ReturnValue(std::shared_ptr<Object> val) : value_{val} {};
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
    explicit Error(std::string err_msg);
    ObjectType Type() override;
    std::string Inspect() override;
};

class Environment
{
  public:
    Environment() = default;
    explicit Environment(std::shared_ptr<Environment> outer_env)
        : outer_env_{outer_env} {};
    ~Environment() = default;
    std::shared_ptr<Object> Get(std::string key);
    std::shared_ptr<Object> Set(std::string key, std::shared_ptr<Object> val);

  private:
    std::unordered_map<std::string, std::shared_ptr<Object>> store_;
    std::shared_ptr<Environment> outer_env_;
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
