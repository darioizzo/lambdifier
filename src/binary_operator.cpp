#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include <llvm/IR/Value.h>

#include <lambdifier/binary_operator.hpp>
#include <lambdifier/expression.hpp>
#include <lambdifier/llvm_state.hpp>

namespace lambdifier
{

binary_operator::binary_operator(char op, const expression &lhs, const expression &rhs) : op(op), lhs(lhs), rhs(rhs)
{
    if (op != '+' && op != '-' && op != '*' && op != '/') {
        throw std::invalid_argument("Invalid binary operator: " + std::string(1, op));
    }
}

binary_operator::binary_operator(const binary_operator &) = default;
binary_operator::binary_operator(binary_operator &&) noexcept = default;

binary_operator::~binary_operator() = default;

const expression &binary_operator::get_lhs() const
{
    return lhs;
}

const expression &binary_operator::get_rhs() const
{
    return rhs;
}

expression &binary_operator::access_lhs()
{
    return lhs;
}

expression &binary_operator::access_rhs()
{
    return rhs;
}

void binary_operator::set_lhs(expression e)
{
    lhs = std::move(e);
}

void binary_operator::set_rhs(expression e)
{
    rhs = std::move(e);
}

llvm::Value *binary_operator::codegen(llvm_state &s) const
{
    auto *l = lhs.codegen(s);
    auto *r = rhs.codegen(s);
    if (!l || !r) {
        return nullptr;
    }

    switch (op) {
        case '+':
            return s.get_builder().CreateFAdd(l, r, "addtmp");
        case '-':
            return s.get_builder().CreateFSub(l, r, "subtmp");
        case '*':
            return s.get_builder().CreateFMul(l, r, "multmp");
        default:
            assert(op == '/');
            return s.get_builder().CreateFDiv(l, r, "divtmp");
    }
}

std::string binary_operator::to_string() const
{
    return "(" + lhs.to_string() + " " + op + " " + rhs.to_string() + ")";
}

double binary_operator::evaluate(std::unordered_map<std::string, double> &values) const
{
    switch (op) {
        case '+':
            return lhs(values) + rhs(values);
        case '-':
            return lhs(values) - rhs(values);
        case '*':
            return lhs(values) * rhs(values);
        default:
            assert(op == '/');
            return lhs(values) / rhs(values);
    }
}

} // namespace lambdifier
