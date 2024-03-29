#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "symbol.hpp"

#include <unordered_map>
#include <utility>
#include <vector>
#include <string>

class Expression {
public:
    Expression() = default;

    explicit Expression(const std::string&);

    virtual ~Expression() = default;

public:
    virtual int calculate(std::unordered_map<std::string, int>&) const = 0;

protected:
    std::vector<Symbol> _infix;
};

#endif // EXPRESSION_H